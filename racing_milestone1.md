# Racing Game Milestone 1 — OPENGL33 harness

> **Post-close source correction (2026-09-02):** the 31-check harness result remains
> valid. Its old GLB/sidecar Milestone 2 handoff is withdrawn; Milestone 2 now uses
> original `.X` sources, authentic XNA 4 pipeline XNBs and `ContentManager` only.
> See `plan_racing.md`.

> **Post-close Milestone 2 extension (2026-09-02):** the same permanent harness now
> accepts `--content-root` and adds authentic XNB model/effect/layout checks. Its
> current 64/64 result belongs to [`racing_milestone2.md`](racing_milestone2.md);
> it does not rewrite this frozen 31-check Milestone 1 result.

## Result

Milestone 1 closed on 2026-09-02. `samples/RacingGameHarness` is a permanent,
standalone diagnostic target; it is deliberately not a partial Racing game and
contains no translated gameplay or content loader. The pinned `OPENGL33` path
passed **31/31** behavioral checks in both Debug and ASan/UBSan runs on an isolated
X11 display.

Artifact evidence is retained under:

```text
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/evidence/cna-opengl33/milestone1
```

## Frozen inputs

| Component | Revision/configuration |
|---|---|
| modern Racing source | `d8092633e4e43e014ff168d8e913a9373538b851` |
| CNA | `51d61ef42d1105d97387feeba11eae91a2f3e2e9` |
| sharp-runtimenext | `9cc96cd57cde394940cc24d58743edf9bf63d3fb` |
| renderer | `CNA_GRAPHICS_RENDERER=OPENGL33` |
| compiled Effects | `CNA_EASYGL_COMPILED_EFFECTS=ON` |
| compiler/build | GCC 14.2.0, C++23, Ninja, at most 8 parallel jobs |
| sanitizer build | `-fsanitize=address,undefined -fno-omit-frame-pointer` |
| runtime | Mesa 25.0.7 llvmpipe, OpenGL 4.5 core, isolated Xvfb/X11 |

`cmake-cache-debug.txt`, `cmake-cache-asan.txt`, the four revision files and the
three `glxinfo-*.log` files preserve the machine-readable configuration.

## Behavioral evidence

Both `harness-debug.log` and `harness-asan.log` record all 31 checks passing:

- exactly one initialize/load/unload/create/dispose lifecycle and paired
  reset/resetting events;
- three completed Update and Draw/present cycles;
- 256x144 startup, 320x180 resize, NativeBackBuffer viewport tracking and a
  fullscreen/windowed round trip;
- real X11 keyboard `R`, mouse button and mouse motion delivered through CNA's
  public input snapshots to the Game window; the unavailable gamepad is logged as
  disconnected rather than simulated;
- deterministic clear and backbuffer readback;
- `RenderTarget2D` bind, clear, unbind and CPU readback;
- independent upload/readback of all six Color `TextureCube` faces;
- byte-exact upload/readback of the original Racing 44-byte
  Position/TextureCoordinate/Normal/Tangent layout;
- a real indexed GPU draw whose output proves UV, normal and tangent attributes
  were consumed at offsets 12, 20 and 32;
- complete 320x180 PPM capture.

The diagnostic centre pixel is `(128,191,64,255)`. Debug, LeakSanitizer and
ASan/UBSan captures are byte-identical at the PPM level:

```text
c20ef5d5f6cf73eb9b1ba733a47f2ea57ce5b5470bab597063b3fe79b5f60136
```

The image contains the expected dark border and green-to-yellow interpolated
rectangle. `capture-sha256.txt` retains all PPM/PNG hashes.

## Sanitizer classification

The ASan/UBSan control run used `ASAN_OPTIONS=detect_leaks=0:halt_on_error=1` and
`UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`. It exits successfully with
31/31 checks and no AddressSanitizer or undefined-behavior diagnostic.

A separate `detect_leaks=1` run also reaches 31/31, then reports exactly 100,956
bytes in 449 process-exit allocations. Every non-interceptor allocation frame is
inside external `libGLX_mesa.so`; there is no CNA, Sharp Runtime or harness
allocation frame. This exactly matches CNA's previously documented Mesa/Xvfb
baseline, so `qualify-milestone1.sh` preserves and classifies that log before
running the clean ASan/UBSan control. It does not silently suppress an unknown
leak.

## Environment observations

SDL logs a mode-switch timeout under Xvfb because that server has no window
manager; SDL reverts the virtual mode. CNA's manager and presentation state still
complete the fullscreen/windowed round trip, restore the requested dimensions and
raise paired reset events. This is retained in the logs rather than hidden and is
not evidence for physical-display fullscreen behavior.

The host's `/tmp/.X11-unix` ownership was invalid during this session, so ordinary
`xvfb-run` could not allocate a Unix-domain display. Qualification used a dedicated
Xvfb reachable only for the test session through `RACING_XVFB_DISPLAY=localhost:177`.
The runner explicitly sets `SDL_VIDEODRIVER=x11` and unsets `WAYLAND_DISPLAY`, so no
window or browser touched the real desktop. On a normally configured host the same
script creates and tears down its own Xvfb automatically.

## Reproduction

```bash
CNA_BUILD_JOBS=8 samples/RacingGameHarness/scripts/qualify-milestone1.sh
```

If an already isolated Xvfb is intentionally managed by the caller:

```bash
RACING_XVFB_DISPLAY=localhost:177 CNA_BUILD_JOBS=8 \
  samples/RacingGameHarness/scripts/qualify-milestone1.sh
```

The script configures and builds both variants, drives only the named harness
window, injects real X11 input, validates all pass markers, classifies LeakSanitizer,
converts captures and records revisions/checksums. No external binding-specific or
gameplay workaround is involved.

## Handoff to Milestone 2

The reference lifecycle/resource/input platform is stable. Milestone 2 now owns
only the bounded authentic XNA 4 pipeline and XNB proofs for `Car`, `Windmill`,
`AlphaDeadTree`, `Cube` and their referenced content. The old sidecar/malformed-GLB
handoff is void. It must not expand into gameplay translation or use modern assets
while the snapshot-protected Win7 repair gate recorded in `plan_racing.md` remains
open.
