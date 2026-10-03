#!/usr/bin/env bash
set -euo pipefail

artifact_root=${RACING_ARTIFACT_ROOT:-/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master}
web_build_root=${RACING_WEB_BUILD_ROOT:-${artifact_root}/cna-web-webgl2-current-20261003}
output_root=${RACING_ENDORA_OUTPUT_ROOT:-${artifact_root}/evidence/endora-package-current-20261003}
script_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

readonly deploy_files=(
    RacingGame_cna_samples.js
    RacingGame_cna_samples.wasm
    RacingGame_cna_samples.data
    RacingGame-content-models.js
    RacingGame-content-models.data
    RacingGame-content-landscape.js
    RacingGame-content-landscape.data
    RacingGame-content-textures.js
    RacingGame-content-textures.data
)

for file in "${deploy_files[@]}" RacingGame_cna_samples.html; do
    test -f "${web_build_root}/${file}" || {
        echo "Missing Web deployment file: ${web_build_root}/${file}" >&2
        exit 1
    }
done

landscape_size=$(stat -c %s \
    "${web_build_root}/RacingGame-content-landscape.data")
if [[ "${landscape_size}" != 178814172 ]]; then
    echo "Unexpected Landscape package size: ${landscape_size}" >&2
    exit 1
fi

mkdir -p "${output_root}"
install -m 0644 "${web_build_root}/RacingGame_cna_samples.html" \
    "${output_root}/index.html"
for file in "${deploy_files[@]}"; do
    install -m 0644 "${web_build_root}/${file}" "${output_root}/${file}"
done
install -m 0644 "${script_dir}/endora.htaccess" "${output_root}/.htaccess"

if find "${output_root}" -maxdepth 1 -type f -name '*.part*' -print -quit |
    grep -q .; then
    echo "Split deployment artifact found in ${output_root}" >&2
    exit 1
fi

(
    cd "${output_root}"
    sha256sum .htaccess index.html "${deploy_files[@]}" >SHA256SUMS
)

payload_size=$(find "${output_root}" -maxdepth 1 -type f \
    ! -name .htaccess ! -name SHA256SUMS -printf '%s\n' |
    awk '{sum += $1} END {print sum + 0}')
if [[ "${payload_size}" != 300037131 ]]; then
    echo "Unexpected deployable payload size: ${payload_size}" >&2
    exit 1
fi

echo "ENDORA_PACKAGE=${output_root}"
echo "ENDORA_PAYLOAD_BYTES=${payload_size}"
echo "ENDORA_LANDSCAPE_BYTES=${landscape_size}"
echo "ENDORA_SPLIT_FILES=0"
echo "REDISTRIBUTION_STATUS=BLOCKED_MISSING_CANONICAL_LICENSE"
