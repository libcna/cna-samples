#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
sample_dir="$(cd "${script_dir}/.." && pwd)"
workspace_dir="$(cd "${sample_dir}/../../.." && pwd)"
artifact_root="${RACING_ARTIFACT_ROOT:-/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master}"
evidence_dir="${artifact_root}/evidence/cna-opengl33/milestone1"
effect_evidence_dir="${artifact_root}/evidence/cna-opengl33/milestone3"
static_evidence_dir="${artifact_root}/evidence/cna-opengl33/milestone4"
drivable_evidence_dir="${artifact_root}/evidence/cna-opengl33/milestone5"
hud_evidence_dir="${artifact_root}/evidence/cna-opengl33/milestone7"
lifecycle_evidence_dir="${artifact_root}/evidence/cna-opengl33/milestone8"
runtime_evidence_dir="${artifact_root}/evidence/cna-opengl33/milestone9"
fna_static_evidence_dir="${artifact_root}/evidence/fna-static-scene-oracle"
physics_evidence_dir="${artifact_root}/evidence/physics-oracle"
track_evidence_dir="${artifact_root}/evidence/fna-track-oracle"
content_root="${artifact_root}/evidence/xna4-authentic-build/Debug/Content"
jobs="${CNA_BUILD_JOBS:-8}"
fna3d_source="${CNA_FNA3D_SOURCE_DIR:-${artifact_root}/cna-native-opengl33/fna3d-3240147-mojo-6333f74}"
fna_root="${RACING_FNA_ROOT:-/rv/data/library/github.com/FNA-XNA/FNA}"
fna_assembly="${RACING_FNA_ASSEMBLY:-${fna_root}/bin/Release/net8.0/FNA.dll}"
fna_runtime_root="${RACING_FNA_RUNTIME_ROOT:-/rv/tmp/FNA}"
fna_effect_root="${RACING_FNA_EFFECT_ROOT:-/rv/tmp/RacingGame/RacingGame/Assets/Shaders/FNA}"
fna_sdl_lib="${RACING_FNA_SDL_LIB:-${workspace_dir}/cnanext/.sdl-prebuilt-Linux-x86_64-wayland/install/lib}"
racing_source_root="${artifact_root}/xna4-original/RacingGameWindows1/RacingGame"
export CCACHE_DIR="${CNA_CCACHE_DIR:-$HOME/.cache/ccache}"
export CCACHE_BASEDIR="${CCACHE_BASEDIR:-/rv}"
export XDG_DATA_HOME="${artifact_root}/evidence/storage"

if (( jobs > 8 )); then
    jobs=8
fi
if (( jobs < 1 )); then
    jobs=1
fi

mkdir -p "${evidence_dir}" "${static_evidence_dir}" \
    "${drivable_evidence_dir}" \
    "${hud_evidence_dir}" \
    "${lifecycle_evidence_dir}" \
    "${runtime_evidence_dir}" \
    "${fna_static_evidence_dir}" "${physics_evidence_dir}" \
    "${track_evidence_dir}" "${XDG_DATA_HOME}"

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
    cmake --build "${build_dir}" \
        --target RacingGame_cna_samples RacingGameHarness_cna_samples RacingGamePhysicsProbe \
            RacingGameTrackProbe RacingGameDrivableSceneProbe \
            RacingGameSettingsProbe RacingGameScreenFlowProbe \
            RacingGameMenuScreensProbe RacingGameRaceReturnProbe \
            RacingGameDesktopRuntimeProbe RacingGameDeviceResetProbe \
        --parallel "${jobs}"
}

run_desktop_runtime_probe() {
    local build_dir="$1"
    local suffix="$2"
    local binary="${build_dir}/RACING_GAME_BUILD/RacingGameDesktopRuntimeProbe"
    local log="${runtime_evidence_dir}/desktop-runtime-${suffix}.log"

    if [[ -n "${RACING_XVFB_DISPLAY:-}" ]]; then
        env -u WAYLAND_DISPLAY DISPLAY="${RACING_XVFB_DISPLAY}" \
            SDL_VIDEODRIVER=x11 "${binary}" "${content_root}" \
            >"${log}" 2>&1
    else
        xvfb-run -a \
            -s '-screen 0 1280x720x24 +extension GLX +render -noreset' \
            env -u WAYLAND_DISPLAY SDL_VIDEODRIVER=x11 \
                "${binary}" "${content_root}" >"${log}" 2>&1
    fi
    rg -n '^=== Racing Desktop Runtime: PASS ===$' "${log}"
}

run_device_reset_probe() {
    local build_dir="$1"
    local suffix="$2"
    local binary="${build_dir}/RACING_GAME_BUILD/RacingGameDeviceResetProbe"
    local capture="${runtime_evidence_dir}/device-reset-${suffix}.ppm"
    local log="${runtime_evidence_dir}/device-reset-${suffix}.log"

    if [[ -n "${RACING_XVFB_DISPLAY:-}" ]]; then
        env -u WAYLAND_DISPLAY DISPLAY="${RACING_XVFB_DISPLAY}" \
            SDL_VIDEODRIVER=x11 "${binary}" "${content_root}" "${capture}" \
            >"${log}" 2>&1
    else
        xvfb-run -a \
            -s '-screen 0 1280x720x24 +extension GLX +render -noreset' \
            env -u WAYLAND_DISPLAY SDL_VIDEODRIVER=x11 \
                "${binary}" "${content_root}" "${capture}" \
                >"${log}" 2>&1
    fi
    rg -n '^=== Racing Device Reset: PASS ===$' "${log}"
    magick "${capture}" "${runtime_evidence_dir}/device-reset-${suffix}.png"
}

run_settings_probe() {
    local build_dir="$1"
    local suffix="$2"
    local binary="${build_dir}/RACING_GAME_BUILD/RacingGameSettingsProbe"
    local log="${lifecycle_evidence_dir}/settings-${suffix}.log"

    "${binary}" "RacingGameSettingsQualification-${suffix}" >"${log}" 2>&1
    rg -n '^=== Racing Settings: PASS ===$' "${log}"
}

run_lifecycle_probe() {
    local build_dir="$1"
    local suffix="$2"
    local target="$3"
    local evidence_name="$4"
    local expected="$5"
    local binary="${build_dir}/RACING_GAME_BUILD/${target}"
    local capture="${lifecycle_evidence_dir}/${evidence_name}-${suffix}.ppm"
    local log="${lifecycle_evidence_dir}/${evidence_name}-${suffix}.log"

    ln -sfn "${content_root}" "${build_dir}/RACING_GAME_BUILD/Content"
    if [[ -n "${RACING_XVFB_DISPLAY:-}" ]]; then
        env -u WAYLAND_DISPLAY DISPLAY="${RACING_XVFB_DISPLAY}" \
            SDL_VIDEODRIVER=x11 "${binary}" "${content_root}" "${capture}" \
            >"${log}" 2>&1
    else
        xvfb-run -a \
            -s '-screen 0 1280x720x24 +extension GLX +render -noreset' \
            env -u WAYLAND_DISPLAY SDL_VIDEODRIVER=x11 \
                "${binary}" "${content_root}" "${capture}" \
                >"${log}" 2>&1
    fi
    rg -n "^${expected}$" "${log}"
    magick "${capture}" \
        "${lifecycle_evidence_dir}/${evidence_name}-${suffix}.png"
}

run_lifecycle_suite() {
    local build_dir="$1"
    local suffix="$2"

    run_settings_probe "${build_dir}" "${suffix}"
    run_lifecycle_probe "${build_dir}" "${suffix}" \
        RacingGameScreenFlowProbe screen-flow \
        '=== Racing Screen Flow: PASS ==='
    run_lifecycle_probe "${build_dir}" "${suffix}" \
        RacingGameMenuScreensProbe menu-screens \
        '=== Racing Menu Screens: PASS ==='
    run_lifecycle_probe "${build_dir}" "${suffix}" \
        RacingGameRaceReturnProbe race-return \
        '=== Racing Race Return: PASS ==='
    run_desktop_runtime_probe "${build_dir}" "${suffix}"
    run_device_reset_probe "${build_dir}" "${suffix}"
}

run_drivable_scene() {
    local build_dir="$1"
    local suffix="$2"
    local binary="${build_dir}/RACING_GAME_BUILD/RacingGameDrivableSceneProbe"
    local capture="${drivable_evidence_dir}/drivable-${suffix}.ppm"
    local log="${drivable_evidence_dir}/drivable-${suffix}.log"

    ln -sfn "${content_root}" "${build_dir}/RACING_GAME_BUILD/Content"
    if [[ -n "${RACING_XVFB_DISPLAY:-}" ]]; then
        env -u WAYLAND_DISPLAY DISPLAY="${RACING_XVFB_DISPLAY}" \
            SDL_VIDEODRIVER=x11 "${binary}" "${content_root}" "${capture}" \
            >"${log}" 2>&1
    else
        xvfb-run -a \
            -s '-screen 0 1280x720x24 +extension GLX +render -noreset' \
            env -u WAYLAND_DISPLAY SDL_VIDEODRIVER=x11 \
                "${binary}" "${content_root}" "${capture}" \
                >"${log}" 2>&1
    fi
    rg -n '^=== Racing Drivable Scene: PASS ===$' "${log}"
    magick "${capture}" "${drivable_evidence_dir}/drivable-${suffix}.png"
}

launch_harness() {
    local binary="$1"
    local capture="$2"
    local log="$3"
    local glx="$4"
    local static_capture="$5"
    local hud_capture="$6"

    if [[ -n "${RACING_XVFB_DISPLAY:-}" ]]; then
        env DISPLAY="${RACING_XVFB_DISPLAY}" HARNESS_BINARY="${binary}" \
            HARNESS_CAPTURE="${capture}" HARNESS_LOG="${log}" \
            HARNESS_GLXINFO="${glx}" HARNESS_CONTENT_ROOT="${content_root}" \
            HARNESS_EFFECT_EVIDENCE="${effect_evidence_dir}" \
            HARNESS_STATIC_SCENE_CAPTURE="${static_capture}" \
            HARNESS_HUD_CAPTURE="${hud_capture}" \
            "${script_dir}/run-milestone1-xvfb.sh"
    else
        xvfb-run -a -s '-screen 0 1024x768x24 +extension GLX +render -noreset' \
            env HARNESS_BINARY="${binary}" HARNESS_CAPTURE="${capture}" \
                HARNESS_LOG="${log}" HARNESS_GLXINFO="${glx}" \
                HARNESS_CONTENT_ROOT="${content_root}" \
                HARNESS_EFFECT_EVIDENCE="${effect_evidence_dir}" \
                HARNESS_STATIC_SCENE_CAPTURE="${static_capture}" \
                HARNESS_HUD_CAPTURE="${hud_capture}" \
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
    local hud_capture="${hud_evidence_dir}/hud-${suffix}.ppm"

    launch_harness "${binary}" "${capture}" "${log}" "${glx}" \
        "${static_capture}" "${hud_capture}"

    if rg -n '^\[FAIL\]' "${log}"; then
        return 1
    fi
    rg -n '^=== Racing Harness: [0-9]+/[0-9]+ PASS ===$' "${log}"
    magick "${capture}" "${evidence_dir}/capture-${suffix}.png"
    magick "${static_capture}" \
        "${static_evidence_dir}/static-scene-${suffix}.png"
    magick "${hud_capture}" "${hud_evidence_dir}/hud-${suffix}.png"
}

classify_lsan() {
    local build_dir="$1"
    local binary="${build_dir}/RacingGameHarness_cna_samples"
    local capture="${evidence_dir}/capture-lsan.ppm"
    local log="${evidence_dir}/harness-lsan-classification.log"
    local glx="${evidence_dir}/glxinfo-lsan.log"
    local static_capture="${static_evidence_dir}/static-scene-lsan.ppm"
    local hud_capture="${hud_evidence_dir}/hud-lsan.ppm"
    local status=0

    set +e
    ASAN_OPTIONS='detect_leaks=1:halt_on_error=1' \
    UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1' \
        launch_harness "${binary}" "${capture}" "${log}" "${glx}" \
            "${static_capture}" "${hud_capture}"
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

run_physics_oracle() {
    local build_dir="$1"
    local suffix="$2"
    local native_report="${physics_evidence_dir}/cna-${suffix}.txt"
    local fna_project="${sample_dir}/oracle/xna4/PhysicsOracle.FNA.csproj"
    local fna_build_dir="${artifact_root}/cna-native-opengl33/physics-fna"
    local fna_output_dir="${fna_build_dir}/bin"
    local fna_report="${physics_evidence_dir}/fna.txt"

    "${build_dir}/RACING_GAME_BUILD/RacingGamePhysicsProbe" \
        "${native_report}"
    rg -n '^RESULT PASS$' "${native_report}"

    if [[ "${suffix}" == debug ]]; then
        dotnet restore "${fna_project}" \
            -p:FnaAssemblyPath="${fna_assembly}" \
            -p:RacingSourceRoot="${racing_source_root}" \
            -p:BaseIntermediateOutputPath="${fna_build_dir}/obj/"
        dotnet build "${fna_project}" --no-restore -m:"${jobs}" \
            -p:FnaAssemblyPath="${fna_assembly}" \
            -p:RacingSourceRoot="${racing_source_root}" \
            -p:BaseIntermediateOutputPath="${fna_build_dir}/obj/" \
            -p:OutputPath="${fna_output_dir}/"
        dotnet "${fna_output_dir}/PhysicsOracle.FNA.dll" "${fna_report}"
    fi
    if [[ ! -f "${fna_report}" ]]; then
        echo "Missing FNA physics oracle report: ${fna_report}" >&2
        return 1
    fi
    rg -n '^RESULT PASS$' "${fna_report}"
    python3 "${sample_dir}/oracle/xna4/compare-physics-oracle.py" \
        "${fna_report}" "${native_report}" \
        | tee "${physics_evidence_dir}/comparison-${suffix}.txt"
}

run_track_oracle() {
    local build_dir="$1"
    local suffix="$2"
    local native_report="${track_evidence_dir}/cna-${suffix}.txt"
    local fna_report="${track_evidence_dir}/fna.txt"
    local fna_project="${sample_dir}/oracle/xna4/TrackOracle.FNA.csproj"
    local fna_build_dir="${artifact_root}/cna-native-opengl33/track-fna"
    local fna_output_dir="${fna_build_dir}/bin"

    ln -sfn "${content_root}" \
        "${build_dir}/RACING_GAME_BUILD/Content"
    "${build_dir}/RACING_GAME_BUILD/RacingGameTrackProbe" \
        "${native_report}"
    rg -n '^RESULT PASS$' "${native_report}"

    if [[ "${suffix}" == debug ]]; then
        dotnet restore "${fna_project}" \
            -p:FnaAssemblyPath="${fna_assembly}" \
            -p:RacingSourceRoot="${racing_source_root}" \
            -p:BaseIntermediateOutputPath="${fna_build_dir}/obj/"
        dotnet build "${fna_project}" --no-restore -m:"${jobs}" \
            -p:FnaAssemblyPath="${fna_assembly}" \
            -p:RacingSourceRoot="${racing_source_root}" \
            -p:BaseIntermediateOutputPath="${fna_build_dir}/obj/" \
            -p:OutputPath="${fna_output_dir}/"
        ln -sfn "${content_root}" "${fna_output_dir}/Content"
        env SDL_VIDEODRIVER=dummy LD_LIBRARY_PATH="${fna_sdl_lib}" \
            dotnet "${fna_output_dir}/TrackOracle.FNA.dll" \
                "${content_root}" "${fna_report}"
    fi
    if [[ ! -f "${fna_report}" ]]; then
        echo "Missing FNA track oracle report: ${fna_report}" >&2
        return 1
    fi
    rg -n '^RESULT PASS$' "${fna_report}"
    python3 "${sample_dir}/../RacingGame/scripts/compare-track-oracles.py" \
        "${fna_report}" "${native_report}" \
        | tee "${track_evidence_dir}/comparison-${suffix}.txt"
}

debug_build="${artifact_root}/cna-native-opengl33/milestone1-debug"
asan_build="${artifact_root}/cna-native-opengl33/milestone1-asan"

configure_and_build "${debug_build}"
run_physics_oracle "${debug_build}" debug
run_track_oracle "${debug_build}" debug
run_drivable_scene "${debug_build}" debug
run_harness "${debug_build}" debug
run_lifecycle_suite "${debug_build}" debug

configure_and_build "${asan_build}" \
    '-DCMAKE_CXX_FLAGS=-fsanitize=address,undefined -fno-omit-frame-pointer' \
    '-DCMAKE_EXE_LINKER_FLAGS=-fsanitize=address,undefined'
ASAN_OPTIONS='detect_leaks=0:halt_on_error=1' \
UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1' \
run_physics_oracle "${asan_build}" asan
ASAN_OPTIONS='detect_leaks=0:halt_on_error=1' \
UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1' \
run_track_oracle "${asan_build}" asan
ASAN_OPTIONS='detect_leaks=0:halt_on_error=1' \
UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1' \
run_drivable_scene "${asan_build}" asan
classify_lsan "${asan_build}"
ASAN_OPTIONS='detect_leaks=0:halt_on_error=1' \
UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1' \
run_harness "${asan_build}" asan
ASAN_OPTIONS='detect_leaks=0:halt_on_error=1' \
UBSAN_OPTIONS='halt_on_error=1:print_stacktrace=1' \
run_lifecycle_suite "${asan_build}" asan

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
sha256sum "${drivable_evidence_dir}"/drivable-*.ppm \
    "${drivable_evidence_dir}"/drivable-*.png \
    >"${drivable_evidence_dir}/drivable-sha256.txt"
sha256sum "${hud_evidence_dir}"/hud-*.ppm \
    "${hud_evidence_dir}"/hud-*.png \
    >"${hud_evidence_dir}/hud-sha256.txt"
sha256sum "${lifecycle_evidence_dir}"/*.ppm \
    "${lifecycle_evidence_dir}"/*.png \
    >"${lifecycle_evidence_dir}/lifecycle-sha256.txt"

echo "Racing cumulative qualification passed (Debug + ASan/UBSan, classified LSan, OPENGL33, FNA comparison, screen/audio/persistence lifecycle)."
