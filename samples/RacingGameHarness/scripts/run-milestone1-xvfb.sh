#!/usr/bin/env bash
set -euo pipefail

: "${HARNESS_BINARY:?HARNESS_BINARY is required}"
: "${HARNESS_CAPTURE:?HARNESS_CAPTURE is required}"
: "${HARNESS_LOG:?HARNESS_LOG is required}"
: "${HARNESS_GLXINFO:?HARNESS_GLXINFO is required}"

glxinfo -B >"${HARNESS_GLXINFO}" 2>&1

export SDL_VIDEODRIVER=x11
unset WAYLAND_DISPLAY
cd "$(dirname "${HARNESS_BINARY}")"

harness_args=(--capture="${HARNESS_CAPTURE}" --require-input)
if [[ -n "${HARNESS_CONTENT_ROOT:-}" ]]; then
    harness_args+=(--content-root="${HARNESS_CONTENT_ROOT}")
fi
if [[ -n "${HARNESS_EFFECT_EVIDENCE:-}" ]]; then
    harness_args+=(--effect-evidence="${HARNESS_EFFECT_EVIDENCE}")
fi
if [[ -n "${HARNESS_STATIC_SCENE_CAPTURE:-}" ]]; then
    harness_args+=(--static-scene-capture="${HARNESS_STATIC_SCENE_CAPTURE}")
fi

"${HARNESS_BINARY}" "${harness_args[@]}" \
    >"${HARNESS_LOG}" 2>&1 &
harness_pid=$!

cleanup() {
    if kill -0 "${harness_pid}" 2>/dev/null; then
        kill "${harness_pid}" 2>/dev/null || true
    fi
}
trap cleanup EXIT

window_id=""
for _ in $(seq 1 200); do
    window_id="$(xdotool search --name '^Racing CNA Qualification Harness$' 2>/dev/null \
        | sed -n '1p' || true)"
    if [[ -n "${window_id}" ]]; then
        break
    fi
    if ! kill -0 "${harness_pid}" 2>/dev/null; then
        wait "${harness_pid}"
        exit $?
    fi
    sleep 0.05
done

if [[ -z "${window_id}" ]]; then
    echo "Racing harness window did not appear" >>"${HARNESS_LOG}"
    exit 1
fi

xdotool windowfocus --sync "${window_id}"
xdotool mousemove --window "${window_id}" 48 32
xdotool keydown r
xdotool mousedown 1
for step in $(seq 1 40); do
    if ! kill -0 "${harness_pid}" 2>/dev/null; then
        break
    fi
    if (( step % 2 == 0 )); then
        xdotool mousemove --window "${window_id}" 201 113 2>/dev/null || true
    else
        xdotool mousemove --window "${window_id}" 48 32 2>/dev/null || true
    fi
    sleep 0.25
done
if kill -0 "${harness_pid}" 2>/dev/null; then
    xdotool keyup r 2>/dev/null || true
    xdotool mouseup 1 2>/dev/null || true
fi

wait "${harness_pid}"
trap - EXIT
