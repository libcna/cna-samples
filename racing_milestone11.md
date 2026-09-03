# Racing Game Milestone 11 — Android qualification and controls

## Status

In progress as of 2026-09-03. The Android application shell, mobile input and
overlay are implemented and both target ABIs build offline, but the milestone is
not complete until a touch-only race and the device matrix pass on representative
physical hardware.

## Fidelity boundary

The port still loads the unchanged XNA Game Studio 4.0 XNB, XGS, XSB and XWB
products and runs one shared gameplay, screen, audio and rendering implementation.
No Android model, effect, physics, menu or asset replacement exists.

The existing desktop `Input.cpp` is unchanged. `MobileInput` first captures that
provider, then merges CNA `TouchPanel` contacts and optional CNA `Accelerometer`
steering into the game-owned logical `ControlFrame`. It does not synthesize a
connected gamepad. Menu touches reuse the original pointer/click fields; race
touches supply zero-defaulted steering, throttle, brake, handbrake and camera
values to the same `CarPhysics` formulas as the original device branches.

Android preprocessor selection is restricted to the application/provider boundary:
default provider construction, landscape orientation, and `DisplayInfo` safe-area
polling. The multi-touch mapper, per-finger ownership, handed layout, tilt filter,
logical state and overlay renderer are portable C++ and are testable on the host.
The overlay uses a game-owned one-pixel texture plus the authentic game font; it is
drawn after the scene post-process rather than changing the original effects.

## Build and package evidence

- Android API/compile SDK 35, minimum API 24, NDK 29.0.14206865 and CMake 4.1.2.
- SDL3 activity and native shared `libmain.so`, explicit `OPENGLES3`, landscape,
  no Internet permission, and optional touchscreen/gamepad/accelerometer features.
- During a real `TouchPanel` contact, the mobile provider suppresses SDL's
  duplicate touch-generated mouse-driving snapshot and clears its smoothing
  history. It also suppresses the first race frame and ignores contacts inherited
  from GO until release. Direct post-entry touch remains live, while genuine mouse
  input is otherwise unaffected.
- Android enables the optional accelerometer preference by default. Every race
  entry and lifecycle resume starts a fresh sensor session whose first valid
  landscape sample becomes neutral; leaving the race stops the sensor. The Android
  default reverses the platform axis, raises sensitivity to 1.5x and uses a faster
  filter response. A live steering-pad finger overrides tilt and remains the
  sensor fallback.
- Offline Gradle `assembleDebug`: 37 tasks, `BUILD SUCCESSFUL` for `x86_64` and
  `arm64-v8a`.
- A separate `benchmark` variant uses optimized native release flags while
  retaining only debug-key signing for local installation. Performance claims
  must use this variant rather than the assertion-enabled debug APK.
- Offline Gradle `assembleBenchmark`: 50 tasks, `BUILD SUCCESSFUL` for
  `arm64-v8a`; the native build uses `-O2 -DNDEBUG`, the v2 APK signature
  verifies with one local debug-key signer, and the APK is 374,667,401 bytes.
- The benchmark APK was installed and launched on a Redmi `24040RN64Y` running
  Android API 35. Android sensor service reports the game process registered the
  physical `BMA510 ACCELEROMETER` successfully and retains an active connection.
- On that device, disabling post-processing and high detail made the debug APK a
  subjectively smooth race at the original in-game counter's measured 15 FPS. The
  latest benchmark run subsequently presented 23.5--25.2 buffers/s in Android's
  one-second `BufferQueueProducer` samples during an active race. These are useful
  baselines, not yet a controlled engine/GPU frame-time or thermal result.
- x86_64 debug APK: 399,633,305 bytes.
- arm64-v8a debug APK: 400,267,154 bytes; it is a local qualification artifact,
  not a release-size result.

The API-35 x86_64 AVD has a 2400x1080 SDL surface and loads the packaged SDL3 and
Racing `libmain.so`. The rebuilt APK creates a live game session, opens its Android
audio stream and has no application exception or SELinux denial in the clean run.
The game requests an 800x480 logical backbuffer and CNA presents it on the
2400x1080 surface; focused tests pin independent X/Y touch and safe-area transforms
for that exact differing-aspect case. This headless emulator has no KVM and uses
SwiftShader, so its extreme startup time and unrelated Google-process ANRs are not
performance evidence for the game and cannot substitute for physical-device
qualification.
The retained `evidence/cna-android-x86_64/loading-landscape.png` frame proves the
packaged build progressed past XACT construction into authentic landscape loading;
it is integration evidence, not a frame-time or visual-quality measurement.

## Native rendering and control evidence

- `RacingGameMobileControlsProbe`: 49/49 PASS, including independent window-to-
  backbuffer safe-area scaling, stable multi-touch roles, containment/mirroring,
  analog values, focus reset, tilt filtering, injected CNA `TouchPanel` state and
  suppression of duplicate touch-generated mouse steering without disabling a
  genuine mouse after release. Race-entry cases cover a held GO contact, a GO
  release delta and a new post-entry steering contact.
- `RacingGameMobileOverlayProbe`: 5/5 PASS over 240 updates; analog touch throttle
  drives the authentic car physics and every overlay control renders inside the
  safe area. Physical-device ergonomics, including its translucent overlap with
  the corner HUD at some aspect ratios, remains an explicit exit gate.
- `RacingGameMenuScreensProbe`: all 10 screen/storage/cursor checks PASS.
- `RacingGameScreenFlowProbe`: all 16 input-lifetime, loading, splash, main menu,
  car/track selection, race, XACT and capture checks PASS.
- `RacingGameDrivableSceneProbe`: 23/23 PASS over 420 updates/draws, all 12 car and
  ghost parts, 53 models, 1,252 objects, all five post-process passes, shadows,
  replay and HUD.

Retained OPENGLES3 captures are below
`evidence/cna-opengles3/`: `mobile-overlay-final-xact.ppm`, `menu-final.ppm`,
`screen-flow-final-xact.ppm` and `drivable-final-xact.ppm`.

The Linux blur/clipping report exposed general EasyGL defects rather than a port
fallback. CNA commits keep GLES base-vertex draws correct, report render-target
capabilities truthfully, translate legacy compiled-effect texture inputs, preserve
the XNA render-target fallback and bind every compiled render-target sampler to its
own texture unit. The last defect caused later quarter-resolution post-process
inputs to replace earlier samplers; its focused regression and the complete Racing
screen/driving probes pass after CNA commit `d5f8adae8`.

Android qualification also exposed that CNA's non-streaming XACT constructors
were using host `std::ifstream` reads despite FNA using title-content bytes. They
now use the platform filesystem contract, which preserves desktop case-insensitive
lookup and routes packaged Android XGS/XSB/XWB files through AssetManager. The
focused `AudioEngine`, `SoundBank` and `WaveBank` suites pass 117/117 after that
general framework correction.

The 800x480-on-2400x1080 emulator case exposed a second general EasyGL issue:
window input conversion used the Y scale for both axes. EasyGL now derives the
inverse of its actual presentation rectangle for NativeBackBuffer, Stretch,
fixed-height, Letterbox and Overscan modes; all seven focused surface-state tests
pass. Racing separately maps the window-client safe area into its logical
backbuffer, as required by the documented `DisplayInfo` coordinate contract.

## Asset and quality decision

The canonical Content tree is 348 MiB. The largest individual products are two
game-music XNBs (28.6 and 24.8 MiB), the 18.8 MiB XACT wave bank, four 16.8 MiB
landscape/car textures, and an 11.6 MiB menu-music XNB. Those measurements explain
the current roughly 400 MiB debug APK; they do not justify silently reducing source
quality.

Keep the authenticated XNA build products as the canonical set. Before creating a
mobile tier, identify unused packaged products and duplicates, measure peak CPU/GPU
residency, startup, frame time and thermals, and use renderer-capability choices such
as reduced intermediate render-target dimensions where fidelity measurements allow
it. Any lower-resolution or recompressed mobile derivative must be reproducible,
named as a platform tier and compared with the canonical output; it must not replace
the reference XNBs.

## Open exit gates

- complete a race using touch only on representative physical Android devices;
- evaluate control ergonomics and persist handedness, size, opacity, sensitivity
  and tilt preferences;
- measure real GPU formats, frame time, peak memory, load time and thermal behavior;
- test pause/background/resume, surface/context restoration and process death;
- listen to music, engine/gears, collisions and UI cues through the Android XACT
  backend;
- verify settings, highscores/replay storage and Bluetooth-gamepad coexistence;
- decide and qualify a measured mobile quality tier only if needed.

Until those gates pass, the correct claim is “Android implementation and emulator
integration in progress,” not “Android supported.”
