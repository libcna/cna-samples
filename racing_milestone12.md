# Racing Game Milestone 12 — Web qualification

## Status

In progress as of 2026-09-04. The shared Racing C++ port now builds as a threaded
Emscripten/WebGL2 application, progressively loads losslessly staged authentic
XNA 4.0 content, completes an Advanced race and reloads its highscore/replay in
a new real Chrome process. Production memory, audible-cue and browser-compatibility
gates remain open. WebGL context-loss recovery and the
resize/fullscreen/background-resume lifecycle are also qualified.

## Implementation boundary

The web target uses the shared `RacingGameCore` and `RacingGameApplication`
libraries. CMake selects `WEBGL2`, Emscripten Asyncify, shared-memory support and
a preloaded virtual filesystem. A sample-owned responsive shell keeps the 5:3
canvas and download progress inside the viewport, caps the windowed presentation at the
source-faithful 800x480 size, still scales down for smaller displays and provides
an explicit `Fullscreen` button. The Web game starts windowed rather than applying
the original persisted desktop fullscreen preference during the audio-unlock click;
native platforms retain the original setting behavior. A platform-boundary
`ContentDelivery` provider
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

Emscripten now selects the same `MobileInput` composition as Android, reusing CNA
`TouchPanel`, the platform-independent role mapper and the existing overlay while
leaving accelerometer steering disabled. Keyboard, mouse and gamepad input remain
composed through the original desktop provider; there is no browser physics path.

## Build and browser evidence

- Emscripten Release build with CNA `902d6e516`, `WEBGL2`, pthreads, Asyncify,
  exception catching, memory growth and progressive authentic content: PASS.
- Outputs are a 36,646,329-byte bootstrap, 72,915,638-byte Models package,
  178,814,172-byte Landscape package and 771,704-byte Textures/post-process
  package, plus a 9,956,821-byte Wasm module and 310,577-byte JavaScript loader.
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
- The baseline canvas and WebGL2 drawing buffer remained 800x480 and the page
  was `crossOriginIsolated`. There were zero uncaught exceptions, promise
  rejections, HTTP failures or WebGL errors; that baseline did not inject a
  context-loss event.
- A subsequent production Release run exercised the real
  `WEBGL_lose_context` extension during progressive loading, in the main menu
  and during an Advanced race. All three stages reported an exact
  `lost,restored` pair, resumed 98, 112 and 100 animation frames respectively,
  and produced non-empty restored captures (1,662,998, 1,039,789 and 887,278
  bytes). The final run completed 1,175 frames with an 800x480 drawing buffer,
  running 48 kHz WebAudio, ready IDBFS and zero JavaScript, HTTP or WebGL
  errors. The restored menu and race captures were also inspected visually.
- A normal X11 Chrome session through ANGLE/Vulkan accepted the trusted start
  gesture, reported SDL's WebAudio context as `running` at 48 kHz and retained
  its playback node. This closes audio activation and graph construction, but
  not the audible XACT cue-listening gate.
- IDBFS was first verified across an original Options-screen write and a separate
  cached reload of `AllPlayers/RacingGameSettings.xml`. A subsequent diagnostic
  target used the production shell, content packages, storage provider, screen
  stack and physics to complete the Advanced race without teleporting or skipping
  gameplay. The 589.562-second host integration run reached original Game Over,
  submitted 372.600 seconds, installed the faster 243.200-second lap result,
  persisted a 1,159-byte settings/highscore XML and a 77,880-byte, 1,216-matrix
  `Player1/TrackAdvanced.Replay`, then unwound Track and Car selection to MainMenu.
- Chrome was then terminated and a new process/profile attachment loaded the same
  IDBFS database. In 20.247 seconds it recovered the 243.200-second highscore and
  prior replay, entered an Advanced race and exposed 2,442 replay matrices through
  the original preserved replay behavior. Both runs ended PASS with zero uncaught
  JavaScript exceptions, promise rejections or HTTP failures. These are local-host
  deterministic integration timings, not production load or frame-rate claims.
- The restarted race exported a 1,152,015-byte CNA back-buffer PPM at 800x480. It
  was converted and inspected: the Advanced track, car, HUD and loaded 4:03.20
  highscore are complete and sharp. The CNA back-buffer capture is retained as the
  authoritative visual artifact because Chrome's compositor screenshot path showed
  an ANGLE/Vulkan mailbox artifact after the long accelerated run while the game
  itself reported no WebGL error.
- Retained final evidence is under
  `evidence/cna-web-game-settings-write-final/` and
  `evidence/cna-web-x11-game-settings-reload/`: PNG capture points, console
  logs and structured result/audio/storage/cache records.
- Complete-race and process-restart evidence is retained under
  `evidence/cna-web-race-persistence-write-final2/` and
  `evidence/cna-web-race-persistence-reload-final2/`; the latter includes the
  visually inspected CNA back-buffer PPM/PNG.
- The context-loss qualification is retained under
  `evidence/cna-web-context-loss-release-final3/`, including restored loading,
  menu and race captures plus the structured timing/error record.
- A clean Chrome profile with DevTools touch emulation used a real DOM touch to
  choose Start Race and then held steering plus throttle as two independent
  contacts. The production game recorded the complete one/two-finger transition
  sequence, displayed its touch overlay and reached 244 MPH over 1,020 frames;
  WebAudio, IDBFS and all content groups remained ready with zero JavaScript,
  HTTP or WebGL errors. This closes the shared Web touch integration, but not
  physical mobile-browser ergonomics. Evidence is retained under
  `evidence/cna-web-touch-release/`.
- A clean Chrome lifecycle run resized the active game through 844x390 landscape
  and 390x844 portrait mobile viewports, left the initial fullscreen state,
  re-entered fullscreen through Emscripten's production request path and returned
  to the exact 800x480 back buffer. A DevTools freeze/active transition produced
  `visibility:hidden,visibility:visible`, resumed at least 12 animation frames and
  retained a valid 48 kHz WebAudio graph, ready IDBFS and complete content with no
  JavaScript, HTTP or WebGL errors. The landscape, fullscreen and resumed captures
  were visually valid. Portrait survives lifecycle changes but is stretched by
  `NativeBackBuffer`; it is not a qualified gameplay orientation. Evidence is
  retained under `evidence/cna-web-lifecycle-release-final4/`.
- A follow-up to the user-reported fullscreen-exit crop measured the exact failure:
  the browser had restored an 800x480 canvas while SDL's cached 1853x1112 size left
  both the WebGL viewport and scissor at 1853x1112. CNA `956d5c34b` now samples the
  actual canvas backing store only on the fullscreen-to-windowed transition and
  retains it until SDL catches up, avoiding a per-frame pthread/DOM round trip.
  The focused Release harness deterministically grew an 800x480 canvas to
  2048x1200 fullscreen and restored it to 800x480; canvas, drawing buffer, WebGL
  viewport and scissor all ended at exactly 800x480. The windowed surface remained
  800x480, the complete main menu and all three Track Selection buttons were
  visually inspected, and the run reported zero JavaScript, HTTP or WebGL errors.
  Evidence is retained under `evidence/cna-web-fullscreen-exit-final/`.
- The owner-requested production path now starts in an 800x480 window with a visible
  sample-shell Fullscreen control even from a clean profile whose original default
  setting requests fullscreen. Two focused WebGL2 runs used that real control,
  navigated through Car Selection to Track Selection while fullscreen, and showed
  all three track cards plus Select/Back without clipping. After Escape, canvas,
  drawing buffer, viewport and scissor all returned to 800x480. CNA `34c5a9d4a`
  additionally synchronizes SDL's cached logical window dimensions with the restored
  canvas CSS size, because SDL uses those dimensions for browser pointer scaling.
  A real mouse click at normalized canvas position (0.90, 0.92) then activated the
  lower-right Back button and returned to the visually inspected Car Selection
  screen. The final run completed 733 browser frames with running 48 kHz WebAudio,
  ready IDBFS/content and zero JavaScript exceptions, HTTP failures or WebGL errors.
  Evidence is retained under
  `evidence/cna-web-fullscreen-navigation-windowed-start/` and
  `evidence/cna-web-fullscreen-pointer-after-esc/`.
- Software SwiftShader also initialized the full game and executed the same
  renderer without GL errors, but the race was too slow for the interactive
  flow harness. It is fallback integration evidence, not performance support.

Use `samples/RacingGame/scripts/build-web.sh` to reproduce the bundle. Serve the
build directory through `serve-threaded-wasm.py`; the COOP/COEP headers are
required by the pthread ABI. `chrome-web-smoke.mjs` drives an already opened
Chrome DevTools endpoint and stores its evidence in the directory argument.
Set `CNA_TEST_CONTEXT_LOSS=1` to inject and require all three context-loss
recovery stages. Set `CNA_TEST_LIFECYCLE=1` to require the resize, production
fullscreen and background-resume sequence. Set `CNA_TEST_FULLSCREEN_EXIT=1` for
the focused fullscreen-to-windowed canvas, viewport and scissor regression. Set
`CNA_TEST_FULLSCREEN_NAVIGATION=1` to keep the production fullscreen active through
Track Selection, exit it there and require the lower-right mouse navigation probe. Set
`CNA_TEST_RACE_PERSISTENCE=write` for the full deterministic race and
`CNA_TEST_RACE_PERSISTENCE=reload` after restarting Chrome to qualify the saved
highscore/replay. `CNA_ATTACH_RUNNING=1` attaches the harness to an already running
DevTools endpoint without launching another browser.

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
- keyboard, mouse, touch and gamepad qualification across desktop and mobile
  Chrome plus another supported browser family (desktop Chrome emulated-touch
  integration is complete);
- release asset-rights and hosting/cache-header decisions.

Until those pass, the correct claim is “real-browser WebGL2 baseline works,” not
“Web supported.”
