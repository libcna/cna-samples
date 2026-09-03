#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
sample_dir=$(cd -- "${script_dir}/.." && pwd)
artifact_root=${RACING_ARTIFACT_ROOT:-/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master}
content_root=${RACING_CONTENT_ROOT:-${artifact_root}/evidence/xna4-authentic-build/Debug/Content}
android_build_root=${RACING_ANDROID_BUILD_ROOT:-${artifact_root}/cna-android-arm64/gradle}

if [[ ! -f "${content_root}/Models/Car.xnb" ]]; then
    echo "Missing authentic XNA content: ${content_root}/Models/Car.xnb" >&2
    exit 1
fi

export ANDROID_HOME=${ANDROID_HOME:-/home/robertvokac/Android/Sdk}
export ANDROID_SDK_ROOT=${ANDROID_SDK_ROOT:-${ANDROID_HOME}}
export CMAKE_BUILD_PARALLEL_LEVEL=${CMAKE_BUILD_PARALLEL_LEVEL:-2}
export RACING_CONTENT_ROOT=${content_root}
export RACING_ANDROID_BUILD_ROOT=${android_build_root}

cd "${sample_dir}/android"
exec ./gradlew --offline \
    --project-cache-dir "${android_build_root}/project-cache" \
    :app:assembleDebug "$@"
