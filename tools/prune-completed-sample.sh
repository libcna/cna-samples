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
# Build trees are located by their build markers rather than by a fixed path, because
# earlier sessions used two different layouts (`<tree>/` and `<tree>/build/`). A
# product found under a nested `build/` is moved up so every pruned sample ends up the
# same shape.
#
# The marker is any of CMakeCache.txt, compile_commands.json, build.ninja or Makefile,
# not CMakeCache.txt alone. A prune that is interrupted part-way -- a machine restart
# during --apply, which happened on 2026-09-07 -- deletes the cache early and leaves the
# rest of the tree behind, and a discovery keyed only on the cache then reports "0 paths"
# on a root that is still carrying its whole _deps and every other sample's build output.
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

# A sample that has both a shipping product and a diagnostic-host variant builds the same
# content twice, into two trees this policy deliberately keeps. On SAMPLE-070 that is two
# byte-identical 504 MB Content trees -- more than half the pruned root, duplicated. Nothing
# can be deleted (both products need their content beside them), but the duplicates do not
# need two copies of every block, so the retained files are consolidated with hardlinks.
#
# Scoped to the one artifact root on purpose: linking across samples would make one sample's
# cleanup silently reach into another's. Build outputs are immutable here, and a build tool
# that later writes into this root replaces files rather than editing them in place, which
# breaks the link safely.
dedupe_root() {
    local root="$1"
    command -v hardlink >/dev/null 2>&1 || return 0
    hardlink --content --quiet "$root" 2>/dev/null || true
}

dedupe_report() {
    local root="$1" out
    command -v hardlink >/dev/null 2>&1 || return 0
    out="$(hardlink --content --dry-run "$root" 2>/dev/null | grep -iE 'saved|freed' | tail -1)"
    [[ -n "$out" ]] && printf '      dedupe (hardlink identical files): %s\n' "$out"
    return 0
}

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
    #
    # Rows write that citation two ways. Most name the file outright,
    # `samples/<Name>/missing.md`; thirteen use the repository's brace shorthand,
    # `samples/<Name>/{missing,diff}.md`, which cites the same file and reads the same to a
    # person. Matching only the first spelling made this script refuse those thirteen -- among
    # them SAMPLE-067 and SAMPLE-068 -- and demand --port-name for a row that does name its
    # ports. Accept both, and take the name from the path either way.
    ports=()
    if [[ -n "$port_override" ]]; then
        IFS=',' read -r -a ports <<<"$port_override"
    else
        mapfile -t ports < <(grep -oE 'samples/[A-Za-z0-9_]+/(missing\.md|\{[^}]*missing[^}]*\}\.md)' \
                             <<<"$row" | cut -d/ -f2 | awk '!seen[$0]++')
    fi
    if [[ ${#ports[@]} -eq 0 ]]; then
        echo "!! $target: plan.md row names no samples/<Name>/missing.md (plain or brace form); pass --port-name" >&2
        exit_code=1; continue
    fi
    is_port() { local n="$1" p; for p in "${ports[@]}"; do [[ "$p" == "$n" ]] && return 0; done; return 1; }

    # Which top-level directory is this root's native build tree. The canonical name is
    # cna-native-opengles3, but roots exist that name it for the configuration instead
    # (SAMPLE-070 used cna-native-opengles3-release). Treating anything but the exact name
    # as a one-off variant deleted SAMPLE-070's native product outright on 2026-09-07 --
    # binary, content and all -- and wrote a MANIFEST with an empty row where the product
    # should have been listed. Identify it by what it holds: a native tree is one with a
    # built product under samples/<port>/.
    native_top="cna-native-opengles3"
    if [[ ! -d "$root/$native_top" ]]; then
        for cand in "$root"/cna-native-*; do
            [[ -d "$cand" ]] || continue
            for p in "${ports[@]}"; do
                if [[ "$(count_products "$cand/samples/$p")" -gt 0 ]]; then
                    native_top="$(basename "$cand")"
                    break 2
                fi
            done
        done
    fi

    # ...and a root may hold MORE THAN ONE of them. SAMPLE-072 keeps a debug and a release
    # native tree, each with its own product and its own nine-capture evidence directory. The
    # 2026-09-07 repair above only covered a root whose canonical name was absent; with both
    # present, native_top became the debug tree and the release tree fell into the "one-off
    # variant, the whole top-level directory goes" branch below -- 141 MB including a qualified
    # product. Every top-level tree that holds a product for this port is a product tree.
    product_tops=()
    for cand in "$root"/cna-native-* "$root"/cna-web-*; do
        [[ -d "$cand" ]] || continue
        for p in "${ports[@]}"; do
            if [[ "$(count_products "$cand/samples/$p")" -gt 0 ]]; then
                product_tops+=("$(basename "$cand")")
                break
            fi
        done
    done
    is_product_top() { local n="$1" t; for t in "${product_tops[@]:-}"; do [[ "$t" == "$n" ]] && return 0; done; return 1; }

    before="$(bytes_of "$root")"
    victims=()
    promotions=()   # "nested-product-dir<TAB>destination"
    strip_targets=()
    refused=0

    # --- every CMake build tree in this root, wherever it sits ----------------------
    mapfile -t caches < <(find "$root" -maxdepth 3 \
                              \( -name CMakeCache.txt -o -name compile_commands.json \
                                 -o -name build.ninja -o -name Makefile \) \
                              2>/dev/null | sort -u)
    seen_trees=()
    for cache in "${caches[@]}"; do
        tree="$(dirname "$cache")"
        already=0
        for t in "${seen_trees[@]:-}"; do [[ "$t" == "$tree" ]] && { already=1; break; }; done
        [[ $already -eq 1 ]] && continue
        seen_trees+=("$tree")
        top="${tree#$root/}"; top="${top%%/*}"

        # A tree that is not one of the two canonical ones is a one-off variant; the
        # whole top-level directory goes.
        if [[ "$top" != "$native_top" && "$top" != "cna-web-webgl2" ]] && ! is_product_top "$top"; then
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
                    if [[ "$top" == "$native_top" || "$top" == cna-native-* ]]; then
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
    #
    # An upstream sample with several runnable products builds each into its own
    # subdirectory, so xna4-build is either <root>/xna4-build/{obj,bin,...} for one
    # product or <root>/xna4-build/<Product>/{obj,bin,...} for several. Sweep both
    # depths: the one-product form was the only one handled, which left SAMPLE-068's
    # seven per-product obj/ directories behind.
    original_builds=("$root/xna4-build")
    while IFS= read -r -d '' d; do original_builds+=("$d"); done \
        < <(find "$root/xna4-build" -mindepth 1 -maxdepth 1 -type d -print0 2>/dev/null)
    for build in "${original_builds[@]}"; do
        for junk in obj pipeline-runner; do
            [[ -e "$build/$junk" ]] && victims+=("$build/$junk")
        done
        while IFS= read -r -d '' f; do victims+=("$f"); done \
            < <(find "$build" -maxdepth 1 -type d -name '*frames*' -print0 2>/dev/null)
        # `xna4-build/Content*/` is BuildContent's own output root: it is the genuine XNA 4.0
        # reference that other work compares CNA's build against byte for byte, and it is
        # addressed BY PATH from outside this repository -- plan_xna_sample_xnb_sweep.md on the
        # xnapipeline branch freezes a 7,726-file corpus that way.
        #
        # This used to delete it whenever `bin/Content/` held a provably identical copy. Nothing
        # was ever lost (the guard made sure of that, and the 21 files removed from seven samples
        # on 2026-09-09 were all recovered from their twins), but the reference moved, which broke
        # a sweep in another session that had every right to expect a stable path. The saving was
        # not worth it either: these roots are 40 KB to 256 KB, and `dedupe_root()` already
        # reclaims every duplicated byte by hardlinking identical files, without deleting a path
        # anyone might be pointing at.
        #
        # So: keep both. If a future rule wants one of the twins gone, it should be `bin/Content/`
        # -- the copy the executable happens to run from -- and never the output root.
    done

    if [[ $refused -eq 1 && $force -eq 0 ]]; then
        exit_code=1
        continue
    fi

    # Victims come from several independent rules, so one can sit inside another -- a build
    # tree's CNA_BUILD/modules and that same tree's per-sample directories, for instance. The
    # estimate below sums bytes per path, so a nested victim was counted twice and a dry run
    # could report freeing more than the root holds: SAMPLE-098 printed
    # "332.9MB -> -202558618B   frees 526.1MB" on 2026-09-09, a negative size after. Drop any
    # victim an earlier one already contains -- `rm -rf` of the ancestor removes it anyway --
    # so the figure, the path count and the deletions all describe the same set. Sorting first
    # puts a parent before its children ('/a' < '/a/b'), and the trailing '/' in the test keeps
    # a sibling like '/a-b' from looking nested under '/a'.
    if [[ ${#victims[@]} -gt 0 ]]; then
        mapfile -t victims < <(printf '%s\n' "${victims[@]}" | LC_ALL=C sort -u)
        outermost=()
        for v in "${victims[@]}"; do
            nested=0
            for k in "${outermost[@]:-}"; do
                [[ -n "$k" ]] || continue
                if [[ "$v" == "$k"/* ]]; then nested=1; break; fi
            done
            [[ $nested -eq 0 ]] && outermost+=("$v")
        done
        victims=("${outermost[@]}")
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
                xna4-original|xna4-build|cna-web-webgl2|scripts|evidence|MANIFEST.md) ;;
                "$native_top") ;;
                chrome-profile-*|cna-native-*|cna-web-*) ;;
                *) printf '      KEPT (unrecognised) %s  (%s)\n' \
                       "${d#$root/}" "$(human "$(bytes_of "$d")")";;
            esac
        done
        dedupe_report "$root"
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
                done < <(find "$root/$native_top/samples/$p" -maxdepth 1 \
                            -type f -executable ! -name '*.cmake' -print0 2>/dev/null)
            done
        fi
        # Leftover empty directories from the deletions -- inside the build trees only.
        # An empty directory elsewhere in the root may be deliberate (SAMPLE-003 keeps
        # an empty build-tools/), and this policy does not touch what it did not plan
        # to remove.
        for tree in "$root/$native_top" "$root/cna-web-webgl2"; do
            [[ -d "$tree" ]] && find "$tree" -mindepth 1 -type d -empty -delete 2>/dev/null || true
        done

        dedupe_root "$root"

        after="$(bytes_of "$root")"
        freed=$((before - after))
        native_targets=()
        web_targets=()
        for p in "${ports[@]}"; do
            [[ -d "$root/$native_top/samples/$p" ]] && \
                native_targets+=("${p}_cna_samples")
            [[ -d "$root/cna-web-webgl2/samples/$p" ]] && \
                web_targets+=("${p}_cna_samples")
        done
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
    for t in "${product_tops[@]:-$native_top}"; do
        [[ -d "$root/$t/samples/$p" ]] || continue
        case "$t" in
            cna-web-*) printf '| `%s/samples/%s/` | The complete WEBGL2 bundle (`.html`, `.js`, `.wasm`, `.data`), self-contained and publishable. |\n' "$t" "$p";;
            *)         printf '| `%s/samples/%s/` | A native OPENGLES3 executable and its content. |\n' "$t" "$p";;
        esac
    done
done)

## What was removed

A per-sample build of CNA and its vendored dependencies, CMake scaffolding for every
sample the root project configures, build trees for other samples built in this root,
one-off build-tree variants, browser profiles and frame recordings, and the
intermediate directories of the original content build. All of it is reproducible from
\`scripts/\` and \`xna4-original/\`.

$(if [[ ${#native_targets[@]} -gt 0 ]]; then
    printf 'The retained native executable%s %s stripped and carr%s a `RUNPATH` into\n' \
        "$([[ ${#native_targets[@]} -gt 1 ]] && echo s)" \
        "$([[ ${#native_targets[@]} -gt 1 ]] && echo are || echo is)" \
        "$([[ ${#native_targets[@]} -gt 1 ]] && echo y || echo ies)"
    printf "the \`cnanext\` checkout's prebuilt SDL, so it needs that checkout in place to run."
fi)

## Restoring the build trees

\`\`\`bash
root=$root
\$root/scripts/build-original.sh            # original content + executable

$(if [[ ${#native_targets[@]} -gt 0 ]]; then
    printf 'cmake -S %s -B $root/%s -DCMAKE_BUILD_TYPE=Release\n' "$REPO" "$native_top"
    printf 'cmake --build $root/%s --target %s -j$(nproc)\n' "$native_top" \
        "${native_targets[*]}"
fi)
$(if [[ ${#web_targets[@]} -gt 0 ]]; then
    printf '/home/robertvokac/emsdk/upstream/emscripten/emcmake cmake \\\n'
    printf '      -S %s -B $root/cna-web-webgl2 -DCMAKE_BUILD_TYPE=Release\n' "$REPO"
    printf 'cmake --build $root/cna-web-webgl2 --target %s -j$(nproc)\n' \
        "${web_targets[*]}"
fi)
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
