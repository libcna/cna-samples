# Racing Game Milestone 12 — Web qualification

## Status

In progress as of 2026-09-03. The unchanged Racing C++ port now builds as a
threaded Emscripten/WebGL2 application, loads the complete authentic XNA 4.0
Content tree and reaches a driven race in a real Chrome session. The production
delivery, browser lifecycle and compatibility exit gates remain open.

## Implementation boundary

The web target uses the shared `RacingGameCore` and `RacingGameApplication`
libraries. Platform selection is confined to CMake: `WEBGL2`, Emscripten
Asyncify, shared-memory support, WebGL 2 and a preloaded virtual filesystem. No
web-only gameplay, model, effect or asset substitution exists.

Browser catch-up exposed a source-level input lifetime limitation. Racing captures
input in `Update`, while its source-faithful screen implementations consume
`JustPressed` values from `Render`. Several fixed updates before one browser draw
could therefore overwrite a transition. For the owner-requested Web qualification,
`ControlFrame` now retains only one-shot controls and typed text until the next
rendered frame; held axes and buttons still use the latest update. This intentional
portable adjustment is recorded in `samples/RacingGame/diff.md` and has a focused
native regression check.

## Build and browser evidence

- Emscripten Release build with CNA `c9d8bfd85`, `WEBGL2`, pthreads, Asyncify,
  exception catching, memory growth and the complete authentic Content tree:
  PASS.
- Output sizes: 363,340,561-byte data package, 9,864,160-byte Wasm module and
  328,294-byte JavaScript loader. The packager warning correctly identifies the
  initial 346 MiB asset bundle as too large for a production exit claim.
- The data package uses Emscripten's versioned IndexedDB preload cache. A clean
  profile performs the initial local transfer; the subsequent qualified reload
  made no `.data` network request.
- Chrome headless on the host AMD Radeon 780M through ANGLE/Vulkan completed the
  scripted `Splash -> Main menu -> Car selection -> Track selection -> Advanced
  race` path, then supplied throttle and left steering. The final run completed
  919 browser animation frames in 26.464 seconds.
- The canvas and WebGL2 drawing buffer remained 800x480 and the page was
  `crossOriginIsolated`. There were zero uncaught exceptions, promise
  rejections, HTTP failures, WebGL errors or context lost/restored events.
- Retained evidence is under
  `evidence/cna-web-webgl2-vulkan-latch/`: seven PNG capture points, console and
  structured result/exception records.
- Software SwiftShader also initialized the full game and executed the same
  renderer without GL errors, but the race was too slow for the interactive
  flow harness. It is fallback integration evidence, not performance support.

Use `samples/RacingGame/scripts/build-web.sh` to reproduce the bundle. Serve the
build directory through `serve-threaded-wasm.py`; the COOP/COEP headers are
required by the pthread ABI. `chrome-web-smoke.mjs` drives an already opened
Chrome DevTools endpoint and stores its evidence in the directory argument.

## Content and quality decision

The first web proof deliberately keeps all 348 MiB of authenticated XNA output:
approximately 259 MiB of textures and 85 MiB of audio. The current IndexedDB
cache fixes repeat transfer, not first-load size or peak memory. Lower-resolution
assets are therefore not silently substituted.

A production web tier may remove products proven unreachable from the complete
screen/race/audio matrix or generate measured, reproducible texture/audio
derivatives. It must retain the canonical Content tree as the fidelity reference
and report visual, audio, load and residency deltas. Progressive groups are still
required before Milestone 12 can close.

## Open exit gates

- production-reasonable progressive first load and measured peak Wasm/GPU memory;
- browser audio unlock and audible XACT music, engine, collision and UI checks;
- explicit IDBFS synchronization for settings, highscores and replay data;
- WebGL context-loss/restoration while loading, in menus and during a race;
- keyboard, mouse, touch and gamepad qualification across desktop and mobile
  Chrome plus another supported browser family;
- browser resize/fullscreen/background-resume behavior and a complete race;
- release asset-rights and hosting/cache-header decisions.

Until those pass, the correct claim is “real-browser WebGL2 baseline works,” not
“Web supported.”
