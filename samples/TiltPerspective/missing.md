# SAMPLE-107 — TiltPerspective_4_0 parity audit

## Current-head analysis — 2026-10-03

**Status unchanged: `🛠`.** This is an analysis, not a qualification. No sample, CNA or Sharp
Runtime source changed.

**What the sample is.** A Windows Phone 7 Reach 3D demo, 480×800, fullscreen, 30 Hz. 25
multisampled spheres roll in a stone-textured box (`stone4.xnb`). The accelerometer's "down"
vector drives both the ball physics and a parallax camera, so the box appears to tilt in
perspective with the phone. Holding a touch recalibrates the reference "down". The GamePad Back
button exits. On the Phone emulator, which has no sensor, `AccelerometerHelper` replaces the reading
with a slow circular roll (φ = π/8, 1 rad/s). Its upstream directory has 21 files; its eight top-level C# units have 1,302 lines,
and the port has 1,048 lines of C++.

**Port state.** The 2026-09 line-by-line audit stands. The only `CNAEXT` line is the
owner-requested `TouchPanel::setMouseTouchEmulationEnabledEXT(true)` (`diff.md`). The four
`SetData` calls are direct translations of the original's `DebugDraw`/`GeometricPrimitive` calls.
No keyboard, Escape, F1, help or renderer path remains. CNA reports `DeviceType::Emulator` on
desktop and in the browser (`modules/devices/tests/Microsoft/Devices/EnvironmentTests.cpp`), so the
port takes the original's own emulator branch rather than an invented one. `Guide.IsScreenSaverEnabled`
does not require gamer services at the current CNA head. The retained `original/` snapshot is
byte-identical to the physical upstream directory.

**Current-head evidence** (`/rv/tmp/samples/SAMPLE-107-TiltPerspective_4_0/evidence/current-head-analysis-20261003/`):

- `scripts/build-current-head-web-20261003.sh` rebuilt the existing nonthreaded static Release
  WEBGL2 tree against CNA `next fc64a4be3` and Sharp Runtime `next db86514c`: 4:39, exit 0
  (`web-build.log`).
- `scripts/smoke-current-head-web-20261003.sh` ran it in a **visible** system Google Chrome 152 on an
  owned private Xvfb over plain HTTP, driven over CDP by the retained `chrome-tilt.mjs` with a mouse
  press. Result: WebGL 2, a 480×800 canvas, 600 rAF callbacks, no exception, rejection or HTTP error,
  not cross-origin isolated. The frame after 600 rAF shows a different perspective and ball
  positions than the first frame (`web-smoke-mouse/`). This is a smoke run, not the gate: the driver
  does not yet measure motion, recalibration or exit.

**What remains for `✅`.** The old requirement for a browser extension/native host is obsolete; the
current gate is the system Chrome launched from the terminal (`rules.md`, the precedent of
SAMPLE-102).

1. **Native product.** The existing tree `cna-native-opengles3-release/` is a *shared-library*
   build (`libcna.so`) in a non-canonical directory. Configure the static Release OPENGLES3 product
   in `cna-native-opengles3/` with `CNA_SAMPLES_ONLY=TiltPerspective`, as SAMPLE-102's
   `build-cna-native.sh` does. Run it on a private Xvfb: rendering, emulator motion, recalibration
   with the left mouse button held, Escape as Back and a clean exit code.
2. **Browser gate.** Extend the driver to measure that frames change over time, that holding the
   mouse and real touch changes the perspective against an unheld run, and that Back/Escape exits
   cleanly, with no runtime errors.
3. **Artifact layout.** Rename `original/` to the canonical `xna4-original/`. Decide between the
   stale `cna-web-webgl2/` (91 MiB) and `cna-web-webgl2-current/` and record the choice in the
   manifest. Remove the leftover `chrome-profile-107/` (60 MiB).
4. **Gallery.** Add a detail page, a card, page navigation and the byte-verified Release bundle in
   `../samples.libcna.com`, with a screenshot of the running game.
5. Update `plan.md` and this file, commit, and offer the prune dry run.

The original cannot be executed: there is no Windows Phone SDK or host, and its source requires
`Microsoft.Devices.Sensors`. The comparison basis therefore remains the source, the official
content and the documentation, as in the 2026-09 audit. No CNA or Sharp Runtime change is
expected. **Estimate to complete: about 2.5–4 hours.**

## Owner-requested desktop mouse input and rebuild — 2026-09-27

The original Windows Phone sample recalibrates only while `TouchPanel::GetState()` has an active
touch. At the owner's request the constructor now enables CNA's off-by-default mouse-to-touch
extension with one `CNAEXT` line; see `diff.md`. No accelerometer, simulation or sample-local
input logic changed. Release OPENGLES3 and nonthreaded Release WEBGL2 were rebuilt on active CNA
`fd16e1e52` and Sharp Runtime `9e58c955`, using new current-head artifact trees. Native rendered
the box and balls, accepted a held left mouse button and exited cleanly. Supplementary system
Chrome smoke runs on the WEBGL2 bundle exercised mouse and real touch, rendered the scene,
completed 600 RAF callbacks and reported WebGL 2 with no runtime or HTTP errors. Their captures
and results are under `evidence/mouse-optin-*`. The previously required approved browser
extension/native-host interaction route has not been restored, so its qualification gate and
`🛠` plan status remain pending; the smoke runs are not substituted for that gate.

**Historical status before the 2026-09-27 input addition:** the C++ port matched the original XNA
4.0 sample. Native Debug and Release qualification passed. The WEBGL2 artifact built with the
correct WebGL 2 link contract; its required approved browser-extension/native-host gate was
pending. The current rebuild and supplementary browser smoke are recorded above.

## Source and behavior audit

The authoritative source snapshot is stored at:

    /rv/tmp/samples/SAMPLE-107-TiltPerspective_4_0/original/

All runtime source units were compared against the original C# sample:

- TiltPerspectiveSample.cs
- AccelerometerHelper.cs
- BallSimulation.cs
- DebugDraw.cs
- GeometricPrimitive.cs
- RandomUtil.cs
- SpherePrimitive.cs
- VertexPositionNormal.cs
- Program.cs

The port preserves the original ParallaxSample type, 480x800 fullscreen configuration, 30 Hz timestep, Guide screen-saver setting, 25-ball simulation, touch-only recalibration, GamePad Back exit, multisampling state, procedural position/normal vertex layout, real accelerometer route, and exact time-driven emulator fallback.

The previous keyboard tilt, mouse recalibration, Escape/F1 handling, help overlay, dummy texture coordinates, windowed-mode substitution, and other desktop conveniences were removed. They were not present in the XNA sample and are not acceptable porting workarounds.

No CNA or Sharp Runtime change was needed. Live CNA already provides the required accelerometer API, touch API, Guide property, fullscreen property, custom vertex declarations/buffers, multisample rasterizer state, and XNA-compatible framework services.

## Authentic XNA content

Content/stone4.xnb was built offline in the owner's Windows 7 VM using the installed XNA 4.0 content pipeline. The VM network adapters remained disconnected. The temporary VirtualBox shared folder was removed and the VM was shut down after export.

    file:    Content/stone4.xnb
    size:    262331 bytes
    SHA-256: 3e5943546ea499de1532b82661f037206a8e094f6652fe5ff1b3b1072e50d44a
    header:  XNBm, version 5

The content-only MSBuild invocation succeeded. The full original Windows Phone solution cannot reach C# compilation on this installation because its XNA installation lacks the Windows Phone project extension referenced by Microsoft.Xna.GameStudio.targets. This does not affect the authentic TextureProcessor output required by the CNA port.

Evidence and reproducible build inputs are stored under:

    /rv/tmp/samples/SAMPLE-107-TiltPerspective_4_0/evidence/
    /rv/tmp/samples/SAMPLE-107-TiltPerspective_4_0/scripts/
    /rv/tmp/samples/SAMPLE-107-TiltPerspective_4_0/win7-export/

The original root-level help.png is retained as source-package material but is not copied into Content or loaded at runtime, matching the original project.

## Qualification

- OPENGLES3 Debug target: final incremental build passed after the source audit cleanup.
- OPENGLES3 Debug runtime: authentic XNB loaded and the sample rendered successfully on an isolated 480x800 Xvfb display. The screenshot confirms the box, lighting, shadows, and 25 simulated spheres.
- OPENGLES3 Release target: clean configure and build passed with the final source.
- OPENGLES3 Release runtime: started, loaded content, and continued through the virtual-X11 fullscreen mode-switch timeout; no sample/runtime failure was observed.
- WEBGL2 Release target: clean Emscripten configure and build passed.
- WEBGL2 final link contract: -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2.
- WEBGL2 browser runtime: pending the approved real-Chrome browser-extension/native-host integration. No standalone Playwright/CDP or visible host-display Chrome substitute was used.

Native visual evidence:

    /rv/tmp/samples/SAMPLE-107-TiltPerspective_4_0/evidence/cna-opengles3-debug.png

For safe local X11 qualification, WAYLAND_DISPLAY must be removed and SDL must be forced to X11 so the sample cannot select the real Wayland session:

    env -u WAYLAND_DISPLAY DISPLAY=:89 SDL_VIDEODRIVER=x11 .../TiltPerspective_cna_samples

## Remaining item

The port has no known source, content, CNA, or Sharp Runtime gap. Only the repository-mandated real-Chrome WEBGL2 runtime gate is outstanding because its approved control integration is unavailable. Until that infrastructure is restored, the plan status remains ready/in progress rather than fully complete.
