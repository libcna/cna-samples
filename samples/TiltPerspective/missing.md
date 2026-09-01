# SAMPLE-107 — TiltPerspective_4_0 parity audit

**Status:** the C++ port now matches the original XNA 4.0 sample. Native Debug and Release qualification passed. The WEBGL2 artifact builds with the correct WebGL 2 link contract; its required real-Chrome runtime gate remains pending because the approved ChatGPT browser-extension/native-host route is unavailable in this session.

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
