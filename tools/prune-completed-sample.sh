#!/usr/bin/env bash
# Prunes the artifact root of a COMPLETED sample down to its products and evidence.
#
# A sample's artifact root under /rv/tmp/samples holds two very different kinds of
# thing. The products -- the original XNA executable, the native OPENGLES3 binary, the
# WEBGL2 bundle, the exact content, the captures -- are what a later reader needs and
# what publication needs. The intermediates -- a complete build of CNA per sample, its
# vendored dependencies, CMake scaffolding for all 64 samples the root project
# configures, build trees for other samples that happened to be built in this root,
# one-off browser profiles -- are none of those, and every one of them is reproducible
# from the `scripts/` this same root retains.
#
# Measured before this script existed: 19 completed samples occupied 16.9 GB, of which
# roughly 15 GB was intermediates. See rules.md, "Pruning a completed sample".
#
# Build trees are located by their CMakeCache.txt rather than by a fixed path, because
# earlier sessions used two different layouts (`<tree>/` and `<tree>/build/`). A
# product found under a nested `build/` is moved up so every pruned sample ends up the
# same shape.
#
# The default is a dry run. Nothing is deleted without --apply.
set -euo pipefail

BASE="${CNA_SAMPLES_ARTIFACT_BASE:-/rv/tmp/samples}"
REPO="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PLAN="$REPO/plan.md"

apply=0
all=0
force=0
keep_symbols=0
port_override=""
targets=()

usage() {
    cat <<'EOF'
Usage: prune-completed-sample.sh [options] [SAMPLE-nnn-UpstreamDir ...]

  --apply                 delete for real (default: dry run, deletes nothing)
  --all                   every SAMPLE-* root under the artifact base
  --port-name NAMES       the sample's directory name(s) under samples/ in this
                          repository, comma-separated, when plan.md does not
                          name them
  --keep-debug-symbols    do not strip the retained native executable
  --base DIR              artifact base (default /rv/tmp/samples)
  --force                 proceed even when the guard says a build tree would lose
                          its last product
  -h, --help              this text

A sample is refused unless its plan.md row is marked complete, because an unfinished
sample's build tree is still in use.
EOF
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        --apply) apply=1; shift;;
        --all) all=1; shift;;
        --force) force=1; shift;;
        --keep-debug-symbols) keep_symbols=1; shift;;
        --port-name) port_override="$2"; shift 2;;
        --base) BASE="$2"; shift 2;;
        -h|--help) usage; exit 0;;
        -*) echo "unknown option: $1" >&2; usage >&2; exit 2;;
        *) targets+=("$1"); shift;;
    esac
done

if [[ $all -eq 1 ]]; then
    mapfile -t targets < <(cd "$BASE" && ls -d SAMPLE-* 2>/dev/null | sort)
fi
if [[ ${#targets[@]} -eq 0 ]]; then
    echo "no sample named; pass a SAMPLE-nnn-UpstreamDir or --all" >&2
    exit 2
fi
if [[ ${#targets[@]} -gt 1 && -n "$port_override" ]]; then
    echo "--port-name applies to a single sample" >&2
    exit 2
fi

human() { numfmt --to=iec --suffix=B --format='%.1f' "${1:-0}" 2>/dev/null || echo "${1}B"; }
bytes_of() { du -sb "$1" 2>/dev/null | cut -f1 || echo 0; }
# Products are what must survive: a native executable or a WebAssembly module.
count_products() {
    local dir="$1"
    [[ -d "$dir" ]] || { echo 0; return; }
    { find "$dir" -type f \( -executable -o -name '*.wasm' \) \
        ! -name '*.cmake' ! -name '*.sh' ! -name '*.py' 2>/dev/null || true; } | wc -l
}

total_before=0
total_freed=0
exit_code=0

for target in "${targets[@]}"; do
    root="$BASE/$target"
    if [[ ! -d "$root" ]]; then
        echo "!! $target: no such artifact root under $BASE" >&2
        exit_code=1
        continue
    fi

    sample_id="$(sed -E 's/^(SAMPLE-[0-9]+).*/\1/' <<<"$target")"
    row="$(grep -E "^\| $sample_id \|" "$PLAN" | head -1 || true)"
    if [[ -z "$row" ]]; then
        echo "!! $target: $sample_id has no row in plan.md" >&2
        exit_code=1; continue
    fi
    status="$(awk -F'|' '{print $(NF-1)}' <<<"$row" | tr -d ' ')"
    if [[ "$status" != "✅" ]]; then
        echo "!! $target: plan.md row is '$status', not complete -- refusing" >&2
        exit_code=1; continue
    fi

    # An upstream sample can ship more than one runnable product, and then the plan
    # row cites one missing.md per port. Every one of them must survive the prune.
    ports=()
    if [[ -n "$port_override" ]]; then
        IFS=',' read -r -a ports <<<"$port_override"
    else
        mapfile -t ports < <(grep -oE 'samples/[A-Za-z0-9_]+/missing\.md' <<<"$row" |
                             cut -d/ -f2 | awk '!seen[$0]++')
    fi
    if [[ ${#ports[@]} -eq 0 ]]; then
        echo "!! $target: plan.md row does not name samples/<Name>/missing.md; pass --port-name" >&2
        exit_code=1; continue
    fi
    is_port() { local n="$1" p; for p in "${ports[@]}"; do [[ "$p" == "$n" ]] && return 0; done; return 1; }

    before="$(bytes_of "$root")"
    victims=()
    promotions=()   # "nested-product-dir<TAB>destination"
    strip_targets=()
    refused=0

    # --- every CMake build tree in this root, wherever it sits ----------------------
    mapfile -t caches < <(find "$root" -maxdepth 3 -name CMakeCache.txt 2>/dev/null | sort)
    for cache in "${caches[@]}"; do
        tree="$(dirname "$cache")"
        top="${tree#$root/}"; top="${top%%/*}"

        # A tree that is not one of the two canonical ones is a one-off variant; the
        # whole top-level directory goes.
        if [[ "$top" != "cna-native-opengles3" && "$top" != "cna-web-webgl2" ]]; then
            victims+=("$root/$top")
            continue
        fi

        # The guard exists to catch a wrong port name, so it only applies to a sample
        # that is supposed to build something. A sample with no target in this
        # repository -- an evidence-backed non-port such as SAMPLE-004 -- legitimately
        # has no product of its own, and everything under samples/ there belongs to
        # some other sample.
        products_before=0
        for p in "${ports[@]}"; do
            [[ -f "$REPO/samples/$p/CMakeLists.txt" ]] && \
                products_before="$(count_products "$tree/samples")" && break
        done
        kept_products=0
        for p in "${ports[@]}"; do
            kept_products=$((kept_products + $(count_products "$tree/samples/$p")))
        done
        if [[ "$products_before" -gt 0 && "$kept_products" -eq 0 ]]; then
            echo "!! $target: $top holds $products_before product(s) but none under ${ports[*]/#/samples/}" >&2
            echo "   the port name is probably wrong; pass --port-name, or --force to proceed" >&2
            refused=1
            continue
        fi

        # Everything in a CMake build tree is reproducible, so the rule is generic
        # rather than a list of known directory names -- the layouts differ between
        # sessions (CNA_BUILD, or modules/ + SHARP_RUNTIME/ + easy-gl/, or a nested
        # build/). Two things are spared: the samples/ directory, handled below, and a
        # loose product file at the tree root, which is an audit probe rather than an
        # intermediate (SAMPLE-004's effect-blob probes are the case in point).
        for entry in "$tree"/* "$tree"/.ninja_*; do
            [[ -e "$entry" ]] || continue
            [[ "$(basename "$entry")" == "samples" ]] && continue
            if [[ -f "$entry" && ( -x "$entry" || "$entry" == *.wasm ) && "$entry" != *.cmake ]]; then
                continue
            fi
            victims+=("$entry")
        done

        if [[ -d "$tree/samples" ]]; then
            for s in "$tree/samples"/*/; do
                [[ -d "$s" ]] || continue
                if is_port "$(basename "$s")"; then
                    for junk in CMakeFiles Makefile cmake_install.cmake; do
                        [[ -e "$s$junk" ]] && victims+=("$s$junk")
                    done
                    if [[ "$top" == "cna-native-opengles3" ]]; then
                        while IFS= read -r -d '' f; do strip_targets+=("$f"); done \
                            < <(find "$s" -maxdepth 1 -type f -executable ! -name '*.cmake' -print0)
                    fi
                else
                    victims+=("${s%/}")
                fi
            done
        fi

        # Nested layout: lift the product out of <tree>/build/ so every pruned sample
        # ends up with the same <tree>/samples/<port>/ shape.
        if [[ "$tree" != "$root/$top" ]]; then
            for p in "${ports[@]}"; do
                [[ -d "$tree/samples/$p" ]] && \
                    promotions+=("$tree/samples/$p"$'\t'"$root/$top")
            done
        fi
    done

    # --- browser profiles left behind by capture runs --------------------------------
    for d in "$root"/chrome-profile-*; do
        [[ -d "$d" ]] && victims+=("$d")
    done

    # --- the original build: keep bin/, drop what produced it -------------------------
    for junk in obj pipeline-runner; do
        [[ -e "$root/xna4-build/$junk" ]] && victims+=("$root/xna4-build/$junk")
    done
    while IFS= read -r -d '' f; do victims+=("$f"); done \
        < <(find "$root/xna4-build" -maxdepth 1 -type d -name '*frames*' -print0 2>/dev/null)
    # build-original.sh copies Content into bin/; drop the copy nothing runs from, and
    # only when the two are provably identical.
    if [[ -d "$root/xna4-build/Content" && -d "$root/xna4-build/bin/Content" ]]; then
        if diff -rq "$root/xna4-build/Content" "$root/xna4-build/bin/Content" >/dev/null 2>&1; then
            victims+=("$root/xna4-build/Content")
        fi
    fi

    if [[ $refused -eq 1 && $force -eq 0 ]]; then
        exit_code=1
        continue
    fi

    freed=0
    for v in "${victims[@]}"; do freed=$((freed + $(bytes_of "$v"))); done

    printf '%-44s %10s -> %10s   frees %10s   (%d paths)\n' \
        "$target" "$(human "$before")" "$(human $((before - freed)))" \
        "$(human "$freed")" "${#victims[@]}"

    if [[ $apply -eq 0 ]]; then
        for v in "${victims[@]}"; do
            printf '      rm -rf %s  (%s)\n' "${v#$root/}" "$(human "$(bytes_of "$v")")"
        done
        for p in "${promotions[@]:-}"; do
            [[ -n "$p" ]] || continue
            printf '      mv %s -> %s/\n' "${p%%$'\t'*}" "${p##*$'\t'}"
        done
        for e in "${strip_targets[@]:-}"; do
            [[ -n "$e" ]] || continue
            printf '      strip %s  (now %s)\n' "${e#$root/}" "$(human "$(bytes_of "$e")")"
        done
        # Anything at the top level this policy does not recognise is left alone and
        # reported, so an unusual artifact is a decision rather than a casualty.
        for d in "$root"/*; do
            [[ -e "$d" ]] || continue
            case "$(basename "$d")" in
                xna4-original|xna4-build|cna-native-opengles3|cna-web-webgl2|scripts|evidence|MANIFEST.md) ;;
                chrome-profile-*|cna-native-*|cna-web-*) ;;
                *) printf '      KEPT (unrecognised) %s  (%s)\n' \
                       "${d#$root/}" "$(human "$(bytes_of "$d")")";;
            esac
        done
    else
        for v in "${victims[@]}"; do rm -rf -- "$v"; done
        for p in "${promotions[@]:-}"; do
            [[ -n "$p" ]] || continue
            src="${p%%$'\t'*}"; dest="${p##*$'\t'}"
            if [[ -d "$src" ]]; then
                mkdir -p "$dest/samples"
                mv "$src" "$dest/samples/"
                # the now-empty wrapper directory
                find "$dest" -mindepth 1 -maxdepth 1 -type d -empty -delete 2>/dev/null || true
            fi
        done
        if [[ $keep_symbols -eq 0 ]]; then
            for e in "${strip_targets[@]:-}"; do
                [[ -n "$e" && -f "$e" ]] && strip "$e" 2>/dev/null || true
            done
            # promoted products moved after strip_targets was collected
            for p in "${ports[@]}"; do
                while IFS= read -r -d '' f; do
                    strip "$f" 2>/dev/null || true
                done < <(find "$root/cna-native-opengles3/samples/$p" -maxdepth 1 \
                            -type f -executable ! -name '*.cmake' -print0 2>/dev/null)
            done
        fi
        # Leftover empty directories from the deletions -- inside the build trees only.
        # An empty directory elsewhere in the root may be deliberate (SAMPLE-003 keeps
        # an empty build-tools/), and this policy does not touch what it did not plan
        # to remove.
        for tree in "$root/cna-native-opengles3" "$root/cna-web-webgl2"; do
            [[ -d "$tree" ]] && find "$tree" -mindepth 1 -type d -empty -delete 2>/dev/null || true
        done

        after="$(bytes_of "$root")"
        freed=$((before - after))
        cat >"$root/MANIFEST.md" <<EOF
# $target — pruned artifact root

Pruned on $(date -u '+%Y-%m-%d %H:%M UTC') by \`tools/prune-completed-sample.sh\`, after
$sample_id was marked complete in \`plan.md\`.

Size before: $(human "$before") — after: $(human "$after").

## What is kept, and why

| Path | Why |
|---|---|
| \`xna4-original/\` | The exact upstream snapshot this audit compared against. Not reproducible if upstream moves. |
| \`scripts/\` | Builds, runs, captures and analyses everything else. This is what makes the deletions safe. |
| \`evidence/\` | Captures, logs and hashes cited by the sample's \`missing.md\`. |
$(for d in "$root"/xna4-build/*bin*/; do
    [[ -d "$d" ]] || continue
    printf '| `xna4-build/%s/` | An original XNA 4.0 executable with its framework DLLs and content — runnable as it stands. |\n' \
        "$(basename "$d")"
done)
$(for p in "${ports[@]}"; do
    printf '| `cna-native-opengles3/samples/%s/` | The native OPENGLES3 executable and its content. |\n' "$p"
    printf '| `cna-web-webgl2/samples/%s/` | The complete WEBGL2 bundle (`.html`, `.js`, `.wasm`, `.data`), self-contained and publishable. |\n' "$p"
done)

## What was removed

A per-sample build of CNA and its vendored dependencies, CMake scaffolding for every
sample the root project configures, build trees for other samples built in this root,
one-off build-tree variants, browser profiles and frame recordings, and the
intermediate directories of the original content build. All of it is reproducible from
\`scripts/\` and \`xna4-original/\`.

The retained native executable$([[ ${#ports[@]} -gt 1 ]] && echo s) $([[ ${#ports[@]} -gt 1 ]] && echo are || echo is) stripped and carr$([[ ${#ports[@]} -gt 1 ]] && echo y || echo ies) a \`RUNPATH\` into
the \`cnanext\` checkout's prebuilt SDL, so it needs that checkout in place to run.

## Restoring the build trees

\`\`\`bash
root=$root
\$root/scripts/build-original.sh            # original content + executable

cmake -S $REPO -B \$root/cna-native-opengles3 -DCMAKE_BUILD_TYPE=Release
cmake --build \$root/cna-native-opengles3 --target ${ports[*]/%/_cna_samples} -j\$(nproc)

/home/robertvokac/emsdk/upstream/emscripten/emcmake cmake \\
      -S $REPO -B \$root/cna-web-webgl2 -DCMAKE_BUILD_TYPE=Release
cmake --build \$root/cna-web-webgl2 --target ${ports[*]/%/_cna_samples} -j\$(nproc)
\`\`\`
EOF
        printf '      pruned; wrote MANIFEST.md; now %s\n' "$(human "$after")"
    fi

    total_before=$((total_before + before))
    total_freed=$((total_freed + freed))
done

echo "-----------------------------------------------------------------------------"
printf '%-44s %10s -> %10s   frees %10s\n' "TOTAL (${#targets[@]} samples)" \
    "$(human "$total_before")" "$(human $((total_before - total_freed)))" "$(human "$total_freed")"
if [[ $apply -eq 0 ]]; then
    echo "(dry run — nothing was deleted; the figures exclude the strip saving; add --apply)"
fi
exit $exit_code
