#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
harness_dir="$(cd "${script_dir}/.." && pwd)"
workspace_dir="$(cd "${harness_dir}/../../.." && pwd)"
artifact_root="${RACING_ARTIFACT_ROOT:-/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master}"
build_dir="${artifact_root}/cna-native-opengl33/milestone9-release"
content_root="${artifact_root}/evidence/xna4-authentic-build/Debug/Content"
evidence_dir="${artifact_root}/evidence/cna-opengl33/milestone9/release"
physics_evidence="${artifact_root}/evidence/physics-oracle"
track_evidence="${artifact_root}/evidence/fna-track-oracle"
fna3d_source="${CNA_FNA3D_SOURCE_DIR:-${artifact_root}/cna-native-opengl33/fna3d-3240147-mojo-6333f74}"
jobs="${CNA_BUILD_JOBS:-8}"
export CCACHE_DIR="${CNA_CCACHE_DIR:-/rv/cnaccache}"
export XDG_DATA_HOME="${evidence_dir}/storage"

if (( jobs > 8 )); then jobs=8; fi
if (( jobs < 1 )); then jobs=1; fi
mkdir -p "${evidence_dir}" "${XDG_DATA_HOME}"

cmake -S "${harness_dir}" -B "${build_dir}" -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCNA_SOURCE_DIR="${workspace_dir}/cnanext" \
    -DCNA_SHARP_RUNTIME_ROOT="${workspace_dir}/sharp-runtimenext" \
    -DFETCHCONTENT_SOURCE_DIR_FNA3D="${fna3d_source}" \
    -DCMAKE_C_COMPILER_LAUNCHER=ccache \
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
cmake --build "${build_dir}" \
    --target RacingGame_cna_samples RacingGameHarness_cna_samples RacingGamePhysicsProbe \
        RacingGameTrackProbe RacingGameDrivableSceneProbe \
        RacingGameSettingsProbe RacingGameScreenFlowProbe \
        RacingGameMenuScreensProbe RacingGameRaceReturnProbe \
        RacingGameDesktopRuntimeProbe RacingGameDeviceResetProbe \
    --parallel "${jobs}"

ln -sfn "${content_root}" "${build_dir}/Content"
ln -sfn "${content_root}" "${build_dir}/RACING_GAME_BUILD/Content"

"${build_dir}/RACING_GAME_BUILD/RacingGamePhysicsProbe" \
    "${evidence_dir}/physics-release.txt"
python3 "${harness_dir}/oracle/xna4/compare-physics-oracle.py" \
    "${physics_evidence}/fna.txt" "${evidence_dir}/physics-release.txt" \
    >"${evidence_dir}/physics-comparison-release.txt"
rg -n '^RESULT PASS$' "${evidence_dir}/physics-release.txt"
rg -n '^RACING_PHYSICS_ORACLE_COMPARISON=PASS records=753 ' \
    "${evidence_dir}/physics-comparison-release.txt"

"${build_dir}/RACING_GAME_BUILD/RacingGameTrackProbe" \
    "${evidence_dir}/track-release.txt"
python3 "${workspace_dir}/cna-samples/samples/RacingGame/scripts/compare-track-oracles.py" \
    "${track_evidence}/fna.txt" "${evidence_dir}/track-release.txt" \
    >"${evidence_dir}/track-comparison-release.txt"
rg -n '^RESULT PASS$' "${evidence_dir}/track-release.txt"
rg -n '^RACING_TRACK_ORACLE_COMPARISON=PASS records=79 ' \
    "${evidence_dir}/track-comparison-release.txt"

run_gl_probe() {
    local binary="$1"
    local expected="$2"
    local name="$3"
    shift 3
    xvfb-run -a \
        -s '-screen 0 1280x720x24 +extension GLX +render -noreset' \
        env -u WAYLAND_DISPLAY SDL_VIDEODRIVER=x11 \
            "${binary}" "$@" >"${evidence_dir}/${name}.log" 2>&1
    rg -n "^${expected}$" "${evidence_dir}/${name}.log"
}

run_gl_probe \
    "${build_dir}/RACING_GAME_BUILD/RacingGameDrivableSceneProbe" \
    '=== Racing Drivable Scene: PASS ===' drivable-release \
    "${content_root}" "${evidence_dir}/drivable-release.ppm"
run_gl_probe \
    "${build_dir}/RACING_GAME_BUILD/RacingGameScreenFlowProbe" \
    '=== Racing Screen Flow: PASS ===' screen-flow-release \
    "${content_root}" "${evidence_dir}/screen-flow-release.ppm"
run_gl_probe \
    "${build_dir}/RACING_GAME_BUILD/RacingGameMenuScreensProbe" \
    '=== Racing Menu Screens: PASS ===' menu-screens-release \
    "${content_root}" "${evidence_dir}/menu-screens-release.ppm"
run_gl_probe \
    "${build_dir}/RACING_GAME_BUILD/RacingGameDesktopRuntimeProbe" \
    '=== Racing Desktop Runtime: PASS ===' desktop-runtime-release \
    "${content_root}"
run_gl_probe \
    "${build_dir}/RACING_GAME_BUILD/RacingGameDeviceResetProbe" \
    '=== Racing Device Reset: PASS ===' device-reset-release \
    "${content_root}" "${evidence_dir}/device-reset-release.ppm"

xvfb-run -a \
    -s '-screen 0 1024x768x24 +extension GLX +render -noreset' \
    env HARNESS_BINARY="${build_dir}/RacingGameHarness_cna_samples" \
        HARNESS_CAPTURE="${evidence_dir}/harness-release.ppm" \
        HARNESS_LOG="${evidence_dir}/harness-release.log" \
        HARNESS_GLXINFO="${evidence_dir}/glxinfo-release.log" \
        HARNESS_CONTENT_ROOT="${content_root}" \
        "${script_dir}/run-milestone1-xvfb.sh"
if rg -n '^\[FAIL\]' "${evidence_dir}/harness-release.log"; then
    exit 1
fi
rg -n '^=== Racing Harness: [0-9]+/[0-9]+ PASS ===$' \
    "${evidence_dir}/harness-release.log"

xvfb-run -a \
    -s '-screen 0 1280x720x24 +extension GLX +render -noreset' \
    /usr/bin/time -v -o "${evidence_dir}/race-return-release.time" \
    env -u WAYLAND_DISPLAY SDL_VIDEODRIVER=x11 \
        "${build_dir}/RACING_GAME_BUILD/RacingGameRaceReturnProbe" \
        "${content_root}" "${evidence_dir}/race-return-release.ppm" \
        >"${evidence_dir}/race-return-release.log" 2>&1
rg -n '^=== Racing Race Return: PASS ===$' \
    "${evidence_dir}/race-return-release.log"

python3 "${workspace_dir}/cna-samples/samples/RacingGame/scripts/audit-release-assets.py" \
    "${artifact_root}" >"${evidence_dir}/asset-license-audit.txt"
rg -n '^RESULT PASS$' "${evidence_dir}/asset-license-audit.txt"
rg -n '^REDISTRIBUTION_STATUS=BLOCKED_MISSING_CANONICAL_LICENSE$' \
    "${evidence_dir}/asset-license-audit.txt"

cmake -LA -N "${build_dir}" >"${evidence_dir}/cmake-cache-release.txt"
git -C "${workspace_dir}/cna-samples" rev-parse HEAD \
    >"${evidence_dir}/cna-samples-start-head.txt"
git -C "${workspace_dir}/cnanext" rev-parse HEAD \
    >"${evidence_dir}/cna-head.txt"
git -C "${workspace_dir}/sharp-runtimenext" rev-parse HEAD \
    >"${evidence_dir}/sharp-runtime-head.txt"
sha256sum "${evidence_dir}"/*.ppm \
    >"${evidence_dir}/captures-sha256.txt"

echo "Racing Milestone 9 Release qualification passed; redistribution remains blocked pending a canonical license grant."
