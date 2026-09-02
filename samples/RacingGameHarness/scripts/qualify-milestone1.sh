#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
sample_dir="$(cd "${script_dir}/.." && pwd)"
workspace_dir="$(cd "${sample_dir}/../../.." && pwd)"
artifact_root="${RACING_ARTIFACT_ROOT:-/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master}"
evidence_dir="${artifact_root}/evidence/cna-opengl33/milestone1"
effect_evidence_dir="${artifact_root}/evidence/cna-opengl33/milestone3"
static_evidence_dir="${artifact_root}/evidence/cna-opengl33/milestone4"
fna_static_evidence_dir="${artifact_root}/evidence/fna-static-scene-oracle"
content_root="${artifact_root}/evidence/xna4-authentic-build/Debug/Content"
jobs="${CNA_BUILD_JOBS:-8}"
fna3d_source="${CNA_FNA3D_SOURCE_DIR:-${artifact_root}/cna-native-opengl33/fna3d-3240147-mojo-6333f74}"
fna_root="${RACING_FNA_ROOT:-/rv/data/library/github.com/FNA-XNA/FNA}"
fna_assembly="${RACING_FNA_ASSEMBLY:-${fna_root}/bin/Release/net8.0/FNA.dll}"
fna_runtime_root="${RACING_FNA_RUNTIME_ROOT:-/rv/tmp/FNA}"
fna_effect_root="${RACING_FNA_EFFECT_ROOT:-/rv/tmp/RacingGame/RacingGame/Assets/Shaders/FNA}"
fna_sdl_lib="${RACING_FNA_SDL_LIB:-${workspace_dir}/cnanext/.sdl-prebuilt-Linux-x86_64-wayland/install/lib}"
racing_source_root="${artifact_root}/xna4-original/RacingGameWindows1/RacingGame"
export CCACHE_DIR="${CNA_CCACHE_DIR:-/rv/cnaccache}"

if (( jobs > 8 )); then
    jobs=8
fi
if (( jobs < 1 )); then
    jobs=1
fi

mkdir -p "${evidence_dir}" "${static_evidence_dir}" \
    "${fna_static_evidence_dir}"

configure_and_build() {
    local build_dir="$1"
    shift
    cmake -S "${sample_dir}" -B "${build_dir}" -G Ninja \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCNA_SOURCE_DIR="${workspace_dir}/cnanext" \
        -DCNA_SHARP_RUNTIME_ROOT="${workspace_dir}/sharp-runtimenext" \
        -DFETCHCONTENT_SOURCE_DIR_FNA3D="${fna3d_source}" \
        -DCMAKE_C_COMPILER_LAUNCHER=ccache \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        "$@"
    cmake --build "${build_dir}" --target RacingGameHarness_cna_samples --parallel "${jobs}"
}

launch_harness() {
    local binary="$1"
    local capture="$2"
    local log="$3"
    local glx="$4"
    local static_capture="$5"

    if [[ -n "${RACING_XVFB_DISPLAY:-}" ]]; then
        env DISPLAY="${RACING_XVFB_DISPLAY}" HARNESS_BINARY="${binary}" \
            HARNESS_CAPTURE="${capture}" HARNESS_LOG="${log}" \
            HARNESS_GLXINFO="${glx}" HARNESS_CONTENT_ROOT="${content_root}" \
            HARNESS_EFFECT_EVIDENCE="${effect_evidence_dir}" \
            HARNESS_STATIC_SCENE_CAPTURE="${static_capture}" \
            "${script_dir}/run-milestone1-xvfb.sh"
    else
        xvfb-run -a -s '-screen 0 1024x768x24 +extension GLX +render -noreset' \
            env HARNESS_BINARY="${binary}" HARNESS_CAPTURE="${capture}" \
                HARNESS_LOG="${log}" HARNESS_GLXINFO="${glx}" \
                HARNESS_CONTENT_ROOT="${content_root}" \
                HARNESS_EFFECT_EVIDENCE="${effect_evidence_dir}" \
                HARNESS_STATIC_SCENE_CAPTURE="${static_capture}" \
                "${script_dir}/run-milestone1-xvfb.sh"
    fi
}

run_harness() {
    local build_dir="$1"
    local suffix="$2"
    local binary="${build_dir}/RacingGameHarness_cna_samples"
    local capture="${evidence_dir}/capture-${suffix}.ppm"
    local log="${evidence_dir}/harness-${suffix}.log"
    local glx="${evidence_dir}/glxinfo-${suffix}.log"
    local static_capture="${static_evidence_dir}/static-scene-${suffix}.ppm"

    launch_harness "${binary}" "${capture}" "${log}" "${glx}" \
        "${static_capture}"

    if rg -n '^\[FAIL\]' "${log}"; then
        return 1
    fi
    rg -n '^=== Racing Harness: [0-9]+/[0-9]+ PASS ===$' "${log}"
    magick "${capture}" "${evidence_dir}/capture-${suffix}.png"
    magick "${static_capture}" \
        "${static_evidence_dir}/static-scene-${suffix}.png"
}

classify_lsan() {
    local build_dir="$1"
    local binary="${build_dir}/RacingGameHarness_cna_samples"
    local capture="${evidence_dir}/capture-lsan.ppm"
    local log="${evidence_dir}/harness-lsan-classification.log"
    local glx="${evidence_dir}/glxinfo-lsan.log"
    local static_capture="${static_evidence_dir}/static-scene-lsan.ppm"
    local status=0

    set +e
    ASAN_OPTIONS='detect_leaks=1:halt_on_error=1' \
    UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1' \
        launch_harness "${binary}" "${capture}" "${log}" "${glx}" \
            "${static_capture}"
    status=$?
    set -e

    rg -n '^=== Racing Harness: [0-9]+/[0-9]+ PASS ===$' "${log}"
    if rg -q 'LeakSanitizer: detected memory leaks' "${log}"; then
        local non_mesa_frames
        non_mesa_frames="$(rg '^    #[1-9][0-9]* ' "${log}" \
            | rg -v 'libGLX_mesa\.so' || true)"
        if [[ -n "${non_mesa_frames}" ]]; then
            echo "LeakSanitizer found a non-Mesa allocation frame:" >&2
            echo "${non_mesa_frames}" >&2
            return 1
        fi
        rg -n '^SUMMARY: AddressSanitizer: .* leaked in .* allocation' "${log}"
        echo "LeakSanitizer report is wholly rooted in external libGLX_mesa frames (exit ${status})."
    elif (( status != 0 )); then
        echo "Sanitized harness failed without a classifiable Mesa LeakSanitizer report." >&2
        return "${status}"
    else
        echo "LeakSanitizer completed with no leak report."
    fi
}

run_fna_static_scene_oracle() {
    local project="${sample_dir}/oracle/xna4/StaticSceneOracle.FNA.csproj"
    local build_dir="${artifact_root}/cna-native-opengl33/static-scene-fna"
    local output_dir="${build_dir}/bin"
    local native_path="${fna_sdl_lib}:${fna_runtime_root}/lib/FNA3D/build:${fna_runtime_root}/lib/FAudio/build"
    local oracle=(dotnet "${output_dir}/StaticSceneOracle.FNA.dll"
        "${content_root}" "${fna_effect_root}" "${fna_static_evidence_dir}")

    dotnet restore "${project}" \
        -p:FnaAssemblyPath="${fna_assembly}" \
        -p:RacingSourceRoot="${racing_source_root}" \
        -p:BaseIntermediateOutputPath="${build_dir}/obj/"
    dotnet build "${project}" --no-restore -m:"${jobs}" \
        -p:FnaAssemblyPath="${fna_assembly}" \
        -p:RacingSourceRoot="${racing_source_root}" \
        -p:BaseIntermediateOutputPath="${build_dir}/obj/" \
        -p:OutputPath="${output_dir}/"
    ln -sfn "${content_root}" "${output_dir}/Content"

    if [[ -n "${RACING_XVFB_DISPLAY:-}" ]]; then
        env -u WAYLAND_DISPLAY DISPLAY="${RACING_XVFB_DISPLAY}" \
            SDL_VIDEODRIVER=x11 FNA3D_FORCE_DRIVER=OpenGL \
            LD_LIBRARY_PATH="${native_path}" "${oracle[@]}"
    else
        xvfb-run -a \
            -s '-screen 0 1024x768x24 +extension GLX +render -noreset' \
            env -u WAYLAND_DISPLAY SDL_VIDEODRIVER=x11 \
                FNA3D_FORCE_DRIVER=OpenGL LD_LIBRARY_PATH="${native_path}" \
                "${oracle[@]}"
    fi
    rg -n '^RESULT PASS$' \
        "${fna_static_evidence_dir}/fna-static-scene.txt"
    magick "${fna_static_evidence_dir}/fna-static-scene.ppm" \
        "${fna_static_evidence_dir}/fna-static-scene.png"
}

debug_build="${artifact_root}/cna-native-opengl33/milestone1-debug"
asan_build="${artifact_root}/cna-native-opengl33/milestone1-asan"

configure_and_build "${debug_build}"
run_harness "${debug_build}" debug

configure_and_build "${asan_build}" \
    '-DCMAKE_CXX_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer' \
    '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined'
classify_lsan "${asan_build}"
ASAN_OPTIONS='detect_leaks=0:halt_on_error=1' \
UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1' \
run_harness "${asan_build}" asan

run_fna_static_scene_oracle
python3 "${sample_dir}/oracle/xna4/compare-static-scene.py" \
    "${fna_static_evidence_dir}/fna-static-scene.ppm" \
    "${static_evidence_dir}/static-scene-debug.ppm" \
    | tee "${static_evidence_dir}/comparison-debug.txt"
python3 "${sample_dir}/oracle/xna4/compare-static-scene.py" \
    "${fna_static_evidence_dir}/fna-static-scene.ppm" \
    "${static_evidence_dir}/static-scene-asan.ppm" \
    | tee "${static_evidence_dir}/comparison-asan.txt"

cmake -LA -N "${debug_build}" >"${evidence_dir}/cmake-cache-debug.txt"
cmake -LA -N "${asan_build}" >"${evidence_dir}/cmake-cache-asan.txt"
git -C "${workspace_dir}/cna-samples" rev-parse HEAD \
    >"${evidence_dir}/cna-samples-head.txt"
git -C "${workspace_dir}/cnanext" rev-parse HEAD \
    >"${evidence_dir}/cna-head.txt"
git -C "${workspace_dir}/sharp-runtimenext" rev-parse HEAD \
    >"${evidence_dir}/sharp-runtime-head.txt"
git -C /rv/tmp/RacingGame rev-parse HEAD \
    >"${evidence_dir}/racing-source-head.txt"
sha256sum "${evidence_dir}"/capture-*.ppm "${evidence_dir}"/capture-*.png \
    >"${evidence_dir}/capture-sha256.txt"
sha256sum "${static_evidence_dir}"/static-scene-*.ppm \
    "${static_evidence_dir}"/static-scene-*.png \
    >"${static_evidence_dir}/static-scene-sha256.txt"
sha256sum "${fna_static_evidence_dir}"/fna-static-scene.ppm \
    "${fna_static_evidence_dir}"/fna-static-scene.png \
    >"${fna_static_evidence_dir}/static-scene-sha256.txt"

echo "Racing cumulative Milestone 1-4 qualification passed (Debug + ASan/UBSan, classified LSan, OPENGL33, FNA comparison)."
