#!/usr/bin/env bash
# Exercise deletion guards and retained products with a disposable SAMPLE-104 fixture.
set -euo pipefail
repo="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
pruner="$repo/tools/prune-completed-sample.sh"
base="$(mktemp -d)"
trap 'rm -rf -- "$base"' EXIT
sample=SAMPLE-104-PerformanceUtility_4_0
root="$base/$sample"
mkdir -p "$root"/{xna4-original,scripts,evidence,cna-native-opengles3/samples/PerformanceUtility,cna-web-webgl2/samples/PerformanceUtility}
printf 'original\n' > "$root/xna4-original/source.cs"
printf 'rebuild\n' > "$root/scripts/build.sh"
printf 'receipt\n' > "$root/evidence/result.txt"
printf '#!/bin/sh\nexit 0\n' > "$root/cna-native-opengles3/samples/PerformanceUtility/PerformanceUtility_cna_samples"
chmod +x "$root/cna-native-opengles3/samples/PerformanceUtility/PerformanceUtility_cna_samples"
for ext in html js wasm data; do
    printf '%s product\n' "$ext" > "$root/cna-web-webgl2/samples/PerformanceUtility/PerformanceUtility_cna_samples.$ext"
done
touch "$root/cna-native-opengles3/CMakeCache.txt" "$root/cna-web-webgl2/build.ninja"
mkdir -p "$root/cna-native-opengles3/CNA_BUILD"
printf 'reproducible intermediate\n' > "$root/cna-native-opengles3/CNA_BUILD/object.o"
before="$(find "$root" -type f -exec sha256sum {} + | LC_ALL=C sort)"
expect_exit() {
    local expected="$1" actual=0
    shift
    bash "$pruner" --base "$base" "$@" > "$base/output" 2>&1 || actual=$?
    if [[ "$actual" -ne "$expected" ]]; then
        cat "$base/output" >&2
        echo "expected exit $expected, got $actual: $*" >&2
        exit 1
    fi
}
expect_exit 1 "$sample"
expect_exit 2 --allow-partial --all
expect_exit 2 --allow-partial "$sample" "$sample"
expect_exit 2 --allow-partial --allow-deferred "$sample"
expect_exit 2 --allow-deferred --allow-partial "$sample"
expect_exit 2 --allow-partial --allow-cancelled "$sample"
expect_exit 2 --allow-cancelled --allow-partial "$sample"
expect_exit 1 --allow-deferred "$sample"
mkdir -p "$base/SAMPLE-105-PushNotificationsSample_4_0"
expect_exit 1 --allow-partial SAMPLE-105-PushNotificationsSample_4_0
expect_exit 0 --allow-partial --keep-debug-symbols "$sample"
[[ "$before" == "$(find "$root" -type f -exec sha256sum {} + | LC_ALL=C sort)" ]]
expect_exit 0 --allow-partial --keep-debug-symbols --apply "$sample"
[[ ! -e "$root/cna-native-opengles3/CNA_BUILD" && ! -e "$root/cna-web-webgl2/build.ninja" ]]
while read -r hash path; do
    case "$path" in
        */CMakeCache.txt|*/build.ninja|*/CNA_BUILD/*) continue;;
    esac
    [[ "$(sha256sum "$path" | cut -d' ' -f1)" == "$hash" ]]
done <<< "$before"
rg -q 'status remains partial' "$root/MANIFEST.md"
rg -q 'Unfinished features remain incomplete' "$root/MANIFEST.md"
rg -q 'owner-approved partial scope' "$root/MANIFEST.md"
expect_exit 0 --allow-partial --keep-debug-symbols "$sample"
rg -q '\(0 paths\)' "$base/output"
echo 'PASS: partial-root guards, dry-run immutability, product/evidence retention, partial manifest and repeat prune'
