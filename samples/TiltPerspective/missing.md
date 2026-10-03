# SAMPLE-107 — TiltPerspective_4_0 parity audit

## Qualification — 2026-10-03 (`✅`)

Qualified on CNA `next fc64a4be3339` and Sharp Runtime `next db86514c5bb8`, with no CNA or Sharp
Runtime change. The owner-requested mouse-to-touch opt-in (`diff.md`) is the only `CNAEXT` line and
the only difference from the original. Artifact root:
`/rv/tmp/samples/SAMPLE-107-TiltPerspective_4_0/`; evidence: `evidence/qualification-20261003/`.

**What the sample is.** A Windows Phone 7 Reach 3D demo, 480×800, fullscreen, 30 Hz. 25
multisampled spheres roll in a stone-textured box (`stone4.xnb`). The accelerometer's "down"
vector drives both the ball physics and an off-centre perspective, so the box appears to lie
behind the screen. Holding a touch records the current tilt as the level reference. The GamePad
Back button exits. On the Phone emulator, which has no sensor, `AccelerometerHelper` substitutes a
circular roll (φ = π/8). Because the original updates the helper explicitly in `Update` and again as
a component, the roll advances twice per frame; the port keeps both calls. CNA reports
`DeviceType::Emulator` on desktop and in the browser
(`modules/devices/tests/Microsoft/Devices/EnvironmentTests.cpp`), so the port runs the original's
own emulator branch, not an invented one. The upstream directory has 21 files; the eight top-level C#
units have 1,302 lines and the port has 1,048 lines of C++. `xna4-original/` (renamed from the
historical `original/`) is byte-identical to the physical upstream directory.

**Measurable behavior.** The original's projection maps the box opening exactly onto the screen, so
the back wall appears as a ~660 px tall rectangle offset by `-eye.xy·400/(eye.z+400)`. Without
input, the emulator roll moves the eye on a circle and the vertical offset swings by about ±136 px.
While a touch is held, `ComputeEyeVector()` rotates the eye by the reference pitch taken from the same
frame's tilt, which removes the eye's vertical component, so the offset must stay near 0. After
release, the reference stays at its last value. `scripts/analyze-tilt-frames.py` finds the back
wall's edges in every captured frame: the lit back wall is brighter than the side bands, and its
edge is a straight row-wide step.

**Builds.** `scripts/build-cna-native.sh` and `scripts/build-cna-web.sh` configure the canonical
static Release trees `cna-native-opengles3/` and `cna-web-webgl2/` with
`CNA_SAMPLES_ONLY=TiltPerspective`, nonthreaded web and shared ccache. Native took 2:06 and web
1:58, both exit 0. They produced 43 compiler warnings, all inherited from CNA content code, draco and
system headers; none comes from the sample. The old `cna-web-webgl2/` pointed at a dead source path,
which justified the fresh configure. Products:

| Product | SHA-256 |
|---|---|
| native `TiltPerspective_cna_samples` (unstripped, 43,549,968 B) | `12d776ef5f220603ce93cef6b4e26a78c5e3c6cd7a65c94faaca3e46b0a86b1f` |
| web `.html` | `3c25146aab44b21a050b1d193d8b183f0c34b7ad273e53cef26db5c1b999d44a` |
| web `.js` | `b60f5661d2613a1650b4945abff3ad105c4e9eb881966a33993b66c3ff141fb0` |
| web `.wasm` | `f0ee2ceb63bd62bd7e42fad500645ed5e6b36239fc2d56ac79255c1af15721e0` |
| web `.data` = `Content/stone4.xnb` | `3e5943546ea499de1532b82661f037206a8e094f6652fe5ff1b3b1072e50d44a` |

**Native OPENGLES3 gate** (`scripts/capture-cna-native-gate.sh`; owned 480×800 Xvfb, Mesa
software GL; `evidence/qualification-20261003/native/`). It captured 26 frames per phase, held the
left mouse button through xdotool, and closed the window with a `WM_DELETE_WINDOW` client message
(`scripts/send-wm-delete.c`), as a window manager's close button does:

| Phase | Back-wall vertical offset |
|---|---|
| no input | −136 … +134 px (range 270) |
| left mouse held | −6.5 … +6.5 px (range 13) |
| after release | −9 … +304 px (shifted reference) |

The window close gave exit code 0. The bare Xvfb has no window manager, so SDL logs its usual
fullscreen mode-switch timeout; this is an environment message, as in earlier Phone samples.

**WEBGL2 gate** (`scripts/capture-cna-web-gate.sh` + `chrome-tilt-gate.mjs`). A **visible** system
Google Chrome 152.0.7977.82 ran on an owned private Xvfb over plain HTTP and was driven over CDP:

| Phase | Canonical bundle (`web/`) | Exact gallery copy (`web-gallery-copy/`) |
|---|---|---|
| no input | −136 … +136 px | −136 … +137 px |
| left mouse held | −11 … +6 px | −6 … +6 px |
| after release | −319 … +6 px | −313 … +2.5 px |
| real CDP touch held | −5 … +6.5 px | −8 … +7 px |

Both runs reported WebGL 2, a 480×800 canvas, 600 rAF callbacks, no cross-origin isolation, and no
exception, rejection or HTTP error. Both bundles hash identically.

**Not exercised, recorded.** The original exits only on GamePad Back; CNA maps no desktop key to it
(only Android's system Back, `Sdl3Platform.cpp:606`). No physical gamepad is attached. A virtual
uinput pad would be visible to every other session's games on this machine and was deliberately not
created. The Back → `Exit()` line is therefore verified by source review, and shutdown was exercised
by the native window close. The unchanged original cannot run here (no Windows Phone SDK/host, and
it requires `Microsoft.Devices.Sensors`), so behavior is compared with the source, the documentation
and the official content, as in the 2026-09 audit.

**No-workaround review.** The only `CNAEXT` is the owner-approved touch opt-in. The four `SetData`
calls translate the original `DebugDraw`/`GeometricPrimitive` calls directly. No keyboard, Escape,
F1, help, renderer or loader path remains.

**Gallery.** `../samples.libcna.com` gets `TiltPerspective.html`, the byte-identical bundle under
`TiltPerspective/`, the running-game shot `assets/img/tilt-perspective.png` (frame `web/unheld/06.png`),
a new `page-8.html` (sample 85 of 85) with pagination on every page, and the Next link from
Performance Utility. Every local link resolves. Full-page captures are in
`evidence/qualification-20261003/gallery-pages/`.

**Artifacts before the prune.** `cna-native-opengles3-release/` (old shared-library tree,
148 MiB), `cna-web-webgl2-current/` (redundant current tree, 97 MiB), `chrome-profile-107/` (60 MiB
leftover profile) and `cna-native-opengles3/send-wm-delete`. The `tools/prune-completed-sample.sh
SAMPLE-107-TiltPerspective_4_0` dry run on 2026-10-03 estimated 1.3 GB → 183.5 MB.

**Pushed and pruned on the owner's instruction ("yes push and prune", 2026-10-03).** `cna-samples`
`0e8c8c9` and gallery `c24e74c` were pushed. The guarded `--apply` reduced the root from
1,368,650,517 to 175,876,354 bytes and wrote `MANIFEST.md`. Of 421 retained files, 417 are
byte-identical to their pre-prune hashes. The three changed files are the intentionally stripped
native executables: the canonical one is now 39,904,032 B,
`04657ce1bf2abc06c32f6cf3ba7313e36316d9e5783544f77afb865cef3d2099`. The stripped product has no
missing library and passes the full native gate again (±136 px unheld, −8 … +6.5 px held, exit 0).
The second dry run removes nothing. `cna-native-opengles3-release/` and `cna-web-webgl2-current/`
keep only their historical products, and `win7-export/` is kept. After GitHub Pages deployed
`c24e74c`, all four bundle files served by `https://samples.libcna.com/TiltPerspective/` hashed
identically to the qualified bundle, and the detail page, `page-8.html` and the shot return 200
(`deployed-gallery-check.txt`). Evidence: `evidence/prune-closure-20261003/`.

## Current-head analysis — 2026-10-03 (before qualification)

Before qualification, the existing WEBGL2 tree rebuilt at the same heads in 4:39
(`scripts/build-current-head-web-20261003.sh`). A visible-Chrome smoke run with the older
`chrome-tilt.mjs` (`scripts/smoke-current-head-web-20261003.sh`) rendered the moving scene without
errors (`evidence/current-head-analysis-20261003/`). The old extension/native-host requirement was
found obsolete.

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

    /rv/tmp/samples/SAMPLE-107-TiltPerspective_4_0/xna4-original/   (renamed from original/ on 2026-10-03)

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

## Qualification (2026-09, historical)

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

## Remaining item (2026-09; resolved by the 2026-10-03 qualification)

The port has no known source, content, CNA, or Sharp Runtime gap. Only the repository-mandated real-Chrome WEBGL2 runtime gate is outstanding because its approved control integration is unavailable. Until that infrastructure is restored, the plan status remains ready/in progress rather than fully complete.
