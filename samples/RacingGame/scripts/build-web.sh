#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
sample_dir=$(cd -- "${script_dir}/.." && pwd)
workspace_dir=$(cd -- "${sample_dir}/../../.." && pwd)
artifact_root=${RACING_ARTIFACT_ROOT:-/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master}
content_root=${RACING_CONTENT_ROOT:-${artifact_root}/evidence/xna4-authentic-build/Debug/Content}
web_build_root=${RACING_WEB_BUILD_ROOT:-${artifact_root}/cna-web-webgl2}
emsdk_root=${EMSDK_ROOT:-/home/robertvokac/emsdk}
cna_source_dir=${RACING_CNA_SOURCE_DIR:-${workspace_dir}/cnanext}
sharp_runtime_root=${RACING_SHARP_RUNTIME_ROOT:-${workspace_dir}/sharp-runtimenext}
export CCACHE_DIR=${CCACHE_DIR:-$HOME/.cache/ccache}
export CCACHE_BASEDIR=${CCACHE_BASEDIR:-/rv}

if [[ ! -f "${content_root}/Models/Car.xnb" ]]; then
    echo "Missing authentic XNA content: ${content_root}/Models/Car.xnb" >&2
    exit 1
fi
if [[ ! -x "${emsdk_root}/upstream/emscripten/emcmake" ]]; then
    echo "Missing Emscripten emcmake: ${emsdk_root}/upstream/emscripten/emcmake" >&2
    exit 1
fi

"${emsdk_root}/upstream/emscripten/emcmake" cmake \
    -S "${sample_dir}" \
    -B "${web_build_root}" \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCNA_SOURCE_DIR="${cna_source_dir}" \
    -DCNA_SHARP_RUNTIME_ROOT="${sharp_runtime_root}" \
    -DCNA_ENABLE_VIDEO=OFF \
    -DRACING_WEB_CONTENT_ROOT="${content_root}"

build_arguments=(--target RacingGame_cna_samples --parallel)
if [[ -n "${CMAKE_BUILD_PARALLEL_LEVEL:-}" ]]; then
    build_arguments+=("${CMAKE_BUILD_PARALLEL_LEVEL}")
fi

exec cmake --build "${web_build_root}" "${build_arguments[@]}" "$@"
