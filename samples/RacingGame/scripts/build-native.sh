#!/usr/bin/env bash
set -euo pipefail

script_dir=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
sample_dir=$(cd -- "${script_dir}/.." && pwd)
workspace_dir=$(cd -- "${sample_dir}/../../.." && pwd)
artifact_root=${RACING_ARTIFACT_ROOT:-/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master}
content_root=${RACING_CONTENT_ROOT:-${artifact_root}/evidence/xna4-authentic-build/Debug/Content}
native_renderer=${RACING_NATIVE_RENDERER:-OPENGL33}
native_renderers=${RACING_NATIVE_RENDERERS:-}
native_renderers=${native_renderers//,/;}
supported_renderers='OPENGLES3;OPENGL33;VULKAN;WEBGPU;SDL_GPU;FNA3D'

case ";${supported_renderers};" in
    *";${native_renderer};"*) ;;
    *)
        echo "Unsupported native Racing renderer: ${native_renderer}" >&2
        echo "Expected one of: ${supported_renderers}" >&2
        exit 1
        ;;
esac
if [[ -n "${native_renderers}" ]]; then
    IFS=';' read -r -a renderer_list <<< "${native_renderers}"
    for renderer in "${renderer_list[@]}"; do
        case ";${supported_renderers};" in
            *";${renderer};"*) ;;
            *)
                echo "Unsupported native Racing renderer in set: ${renderer}" >&2
                echo "Expected identities from: ${supported_renderers}" >&2
                exit 1
                ;;
        esac
    done
    case ";${native_renderers};" in
        *";${native_renderer};"*) ;;
        *)
            echo "RACING_NATIVE_RENDERER must belong to RACING_NATIVE_RENDERERS" >&2
            exit 1
            ;;
    esac
    renderer_suffix=multi
else
    renderer_suffix=$(printf '%s' "${native_renderer}" | tr '[:upper:]' '[:lower:]')
fi
native_build_root=${RACING_NATIVE_BUILD_ROOT:-${artifact_root}/cna-native-${renderer_suffix}}
cna_source_dir=${RACING_CNA_SOURCE_DIR:-${workspace_dir}/cna}
sharp_runtime_root=${RACING_SHARP_RUNTIME_ROOT:-${workspace_dir}/sharp-runtime}
fna3d_source=${CNA_FNA3D_SOURCE_DIR:-${workspace_dir}/cna-samples/cmake-build-release/_deps/fna3d-src}
export CCACHE_DIR=${CCACHE_DIR:-/rv/cnaccache}
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

cmake_arguments=(
    -S "${sample_dir}"
    -B "${native_build_root}"
    -G Ninja
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_C_COMPILER_LAUNCHER=ccache
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
    -DCNA_SOURCE_DIR="${cna_source_dir}"
    -DCNA_SHARP_RUNTIME_ROOT="${sharp_runtime_root}"
    -DCNA_GRAPHICS_RENDERER="${native_renderer}"
    -DFETCHCONTENT_SOURCE_DIR_FNA3D="${fna3d_source}"
    -DRACING_NATIVE_CONTENT_ROOT="${content_root}"
)
if [[ -n "${native_renderers}" ]]; then
    cmake_arguments+=("-DCNA_GRAPHICS_RENDERERS=${native_renderers}")
fi
if [[ -n "${CNA_WEBGPU_ROOT:-}" ]]; then
    cmake_arguments+=("-DCNA_WEBGPU_ROOT=${CNA_WEBGPU_ROOT}")
fi

cmake "${cmake_arguments[@]}"

build_arguments=(--target RacingGame_cna_samples --parallel "${CMAKE_BUILD_PARALLEL_LEVEL:-12}")

exec cmake --build "${native_build_root}" "${build_arguments[@]}" "$@"
