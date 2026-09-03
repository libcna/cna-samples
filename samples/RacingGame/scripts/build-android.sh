#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
sample_dir=$(cd -- "${script_dir}/.." && pwd)
artifact_root=${RACING_ARTIFACT_ROOT:-/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master}
content_root=${RACING_CONTENT_ROOT:-${artifact_root}/evidence/xna4-authentic-build/Debug/Content}
android_build_root=${RACING_ANDROID_BUILD_ROOT:-${artifact_root}/cna-android-arm64/gradle}
android_variant=${RACING_ANDROID_VARIANT:-debug}

case "${android_variant}" in
    debug)
        gradle_task=:app:assembleDebug
        ;;
    benchmark)
        gradle_task=:app:assembleBenchmark
        ;;
    *)
        echo "RACING_ANDROID_VARIANT must be debug or benchmark, got: ${android_variant}" >&2
        exit 2
        ;;
esac

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
    "${gradle_task}" "$@"
