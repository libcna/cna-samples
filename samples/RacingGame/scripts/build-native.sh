#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
sample_dir=$(cd -- "${script_dir}/.." && pwd)
workspace_dir=$(cd -- "${sample_dir}/../../.." && pwd)
artifact_root=${RACING_ARTIFACT_ROOT:-/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master}
content_root=${RACING_CONTENT_ROOT:-${artifact_root}/evidence/xna4-authentic-build/Debug/Content}
native_renderer=${RACING_NATIVE_RENDERER:-OPENGL33}
case "${native_renderer}" in
    OPENGL33|OPENGLES3) ;;
    *)
        echo "Unsupported native Racing renderer: ${native_renderer}" >&2
        echo "Expected OPENGL33 or OPENGLES3" >&2
        exit 1
        ;;
esac
renderer_suffix=$(printf '%s' "${native_renderer}" | tr '[:upper:]' '[:lower:]')
native_build_root=${RACING_NATIVE_BUILD_ROOT:-${artifact_root}/cna-native-${renderer_suffix}}
cna_source_dir=${RACING_CNA_SOURCE_DIR:-${workspace_dir}/cna}
sharp_runtime_root=${RACING_SHARP_RUNTIME_ROOT:-${workspace_dir}/sharp-runtime}
fna3d_source=${CNA_FNA3D_SOURCE_DIR:-${workspace_dir}/cna-samples/cmake-build-release/_deps/fna3d-src}
export CCACHE_DIR=${CCACHE_DIR:-$HOME/.cache/ccache}
export CCACHE_BASEDIR=${CCACHE_BASEDIR:-/rv}

if [[ ! -f "${content_root}/Models/Car.xnb" ||
      ! -f "${content_root}/Textures/ingame.xnb" ]]; then
    echo "Missing authentic XNA content below ${content_root}" >&2
    exit 1
fi
if [[ ! -f "${fna3d_source}/CMakeLists.txt" ]]; then
    echo "Missing reusable FNA3D source: ${fna3d_source}" >&2
    exit 1
fi

cmake -S "${sample_dir}" -B "${native_build_root}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
    -DCNA_SOURCE_DIR="${cna_source_dir}" \
    -DCNA_SHARP_RUNTIME_ROOT="${sharp_runtime_root}" \
    -DCNA_GRAPHICS_RENDERER="${native_renderer}" \
    -DFETCHCONTENT_SOURCE_DIR_FNA3D="${fna3d_source}" \
    -DRACING_NATIVE_CONTENT_ROOT="${content_root}"

build_arguments=(--target RacingGame_cna_samples --parallel)
if [[ -n "${CMAKE_BUILD_PARALLEL_LEVEL:-}" ]]; then
    build_arguments+=("${CMAKE_BUILD_PARALLEL_LEVEL}")
fi

exec cmake --build "${native_build_root}" "${build_arguments[@]}" "$@"
