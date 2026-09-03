# Racing Game Milestone 12 — Web qualification

## Status

In progress as of 2026-09-04. The shared Racing C++ port now builds as a threaded
Emscripten/WebGL2 application, progressively loads losslessly staged authentic
XNA 4.0 content and reaches a driven race in a real Chrome session. Production
memory, audible-cue, complete save-data, browser-lifecycle and compatibility
gates remain open.

## Implementation boundary

The web target uses the shared `RacingGameCore` and `RacingGameApplication`
libraries. CMake selects `WEBGL2`, Emscripten Asyncify, shared-memory support and
a preloaded virtual filesystem. A sample-owned responsive shell keeps the 5:3
canvas and download progress inside the viewport. A platform-boundary `ContentDelivery` provider
loads three additional file packages at the existing Models, Landscape and
Textures loading phases. The manager only waits for each requested phase; no
web-only gameplay, model, effect or asset substitution exists.

The shell mounts `/save` as an auto-persisting Emscripten IDBFS filesystem and
completes its initial synchronization before exposing a `Start race` button.
That trusted user gesture calls the unchanged program entry point, satisfying
browser autoplay policy before SDL creates the XACT audio graph. A small
platform-boundary `PersistentStorage` provider directs the standard CNA storage
path through `XDG_DATA_HOME=/save` on Web and is a no-op elsewhere. The original
Options flow and `GameSettings::Save()` therefore write their unchanged XML;
no browser save implementation exists in gameplay code.

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
  exception catching, memory growth and progressive authentic content: PASS.
- Outputs are a 36,646,329-byte bootstrap, 72,915,638-byte Models package,
  178,814,172-byte Landscape package and 771,704-byte Textures/post-process
  package, plus a 9,869,313-byte Wasm module and 309,785-byte JavaScript loader.
- Each data package uses Emscripten's versioned IndexedDB preload cache with a
  stable deployed filename. A new Chrome profile reported `fromCache:false` for
  all four packages; the immediate qualified reload reported `fromCache:true`
  for all four and made no `.data` request.
- Chrome headless on the host AMD Radeon 780M through ANGLE/Vulkan completed the
  scripted `Splash -> Main menu -> Car selection -> Track selection -> Advanced
  race` path, then supplied throttle and left steering. The clean final run made
  the 800x480 canvas available in 2.957 seconds, finished all content groups in
  12.657 seconds and completed 807 browser animation frames. The cached run made
  the canvas available in 90 ms and content ready in 8.079 seconds. These are
  local-host integration timings, not production network claims.
- The canvas and WebGL2 drawing buffer remained 800x480 and the page was
  `crossOriginIsolated`. There were zero uncaught exceptions, promise
  rejections, HTTP failures, WebGL errors or context lost/restored events.
- A normal X11 Chrome session through ANGLE/Vulkan accepted the trusted start
  gesture, reported SDL's WebAudio context as `running` at 48 kHz and retained
  its playback node. This closes audio activation and graph construction, but
  not the audible XACT cue-listening gate.
- IDBFS was verified across a clean write and a separate cached reload. The
  original Options screen produced
  `AllPlayers/RacingGameSettings.xml`; an independently synchronized probe and
  the XML were both present after reload. Highscore and replay end-to-end flows
  remain to be exercised.
- Retained final evidence is under
  `evidence/cna-web-game-settings-write-final/` and
  `evidence/cna-web-x11-game-settings-reload/`: PNG capture points, console
  logs and structured result/audio/storage/cache records.
- Software SwiftShader also initialized the full game and executed the same
  renderer without GL errors, but the race was too slow for the interactive
  flow harness. It is fallback integration evidence, not performance support.

Use `samples/RacingGame/scripts/build-web.sh` to reproduce the bundle. Serve the
build directory through `serve-threaded-wasm.py`; the COOP/COEP headers are
required by the pthread ABI. `chrome-web-smoke.mjs` drives an already opened
Chrome DevTools endpoint and stores its evidence in the directory argument.

## Content and quality decision

The canonical tree remains 356 files and 363,340,561 bytes. Packaging reuses 326
runtime files byte-for-byte (289,147,825 bytes) and adds three six-byte readiness
markers. It excludes only 30 products totaling 74,192,736 bytes: XACT source-wave
intermediates already compiled into the authentic XWB and two Xbox-only screens
whose existing platform selection uses the Windows variants. No texture, model,
shader or compiled audio bank is recompressed or reduced. `manifest.json` records
every source file, group, byte count and exclusion reason.

The 35 MiB bootstrap makes the loading UI available without first transferring
the 170 MiB Landscape group. IndexedDB removes repeated data transfer, but the
289 MiB first-use payload and unmeasured peak Wasm/GPU residency are not yet a
production exit. Lower-resolution assets are therefore not silently substituted.

A later production tier may generate measured, reproducible texture/audio
derivatives if hosted-network and residency measurements require them. It must
retain the canonical Content tree as the fidelity reference and report visual,
audio, load and residency deltas.

## Open exit gates

- hosted-network load budget and measured peak Wasm/GPU memory for the current
  progressive packages;
- audible XACT music, engine, collision and UI checks;
- highscore and replay save/load qualification through IDBFS, including
  background/resume and process-restart behavior;
- WebGL context-loss/restoration while loading, in menus and during a race;
- keyboard, mouse, touch and gamepad qualification across desktop and mobile
  Chrome plus another supported browser family;
- browser resize/fullscreen/background-resume behavior and a complete race;
- release asset-rights and hosting/cache-header decisions.

Until those pass, the correct claim is “real-browser WebGL2 baseline works,” not
“Web supported.”
