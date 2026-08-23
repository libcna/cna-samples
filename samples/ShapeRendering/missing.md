# SAMPLE-008 audit — ShapeRenderingSample_4_0

No known behavioral or visual differences remain after the `SAMPLE-008` audit. There is no
unresolved CNA or sharp-runtime implementation gap for this sample.

## Reference and source audit

- Authoritative source: the unchanged local XNA 4.0 `ShapeRenderingSample_4_0` snapshot retained
  in `/rv/tmp/samples/SAMPLE-008-ShapeRenderingSample_4_0/xna4-original`.
- `Program.cs`, `ShapeRenderingSampleGame.cs`, `DebugShapeRenderer.cs` and all three original
  Windows/Xbox/Phone project files were reviewed against the C++ port. The Windows project is the
  selected runnable reference.
- The original Debug x86 executable was compiled directly against the real local XNA 4.0
  assemblies, with the XNA HiDef runtime-profile resource embedded, and run through the dedicated
  Wine/WineD3D prefix.
- The sample has no gameplay Content project or runtime asset. `Background.png`, the icon and the
  thumbnail are project metadata, not content loaded by the game.

The port retains the original `ShapeRenderingSampleGame` name, class split, unused `SpriteBatch`,
CornflowerBlue clear, rotating camera, projection values and draw order. It renders the original
yellow `BoundingBox`, green `BoundingFrustum`, red `BoundingSphere`, purple triangle and brown line.
The Back-button and player-one Escape branches also match the C# source.

The old port-only F1 timer, input branch, texture load and help-overlay draw were removed. The
historical `help.png` is preserved beside `CMakeLists.txt`, outside `Content`, and is not loaded,
copied or preloaded.

## DebugShapeRenderer fidelity

The complete renderer was compared line by line. The C++ translation now preserves:

- all no-lifetime and explicit-lifetime overloads;
- the initial 64-vertex batch, shared eight-corner buffer and 30-step unit sphere;
- the exact one-time initialization guard and `InvalidOperationException` message;
- cache selection, active/cached transitions, lifetime expiration and ascending cache sort;
- vertex order for every line shape and the 65,535-line Reach-profile batching limit;
- `BasicEffect` vertex-color, texture, diffuse-color, world, view, projection and pass behavior.

The XNA project defines `DEBUG` only for Debug configurations and applies
`[Conditional("DEBUG")]` to every public renderer method. C++ has no call-site-eliding attribute,
so the sample CMake target defines the collision-free `SHAPE_RENDERING_SAMPLE_DEBUG` only for its
Debug configuration and the original game call sites are conditionally compiled. This preserves
the source behavior: Debug renders the shapes, while Release omits the debug-renderer calls. A raw
`DEBUG` preprocessor macro cannot be used because it would replace CNA's existing
`LogLevel::DEBUG` enum token.

Normal C++ ownership, reference and property-call syntax plus `CNAEXT GetTypeName()` are the only
representation-level adaptations. The bootstrap include uses CNA's current
`CNA/Platform/Entrypoint.hpp`; no renderer or sample-specific framework helper is used.

## No-workaround review

The audited sample contains no `RawMesh`, `RawModel`, direct content-substitute `SetData`, sidecar
asset, handwritten shader, backend call, omitted shape, simplified branch, invented input or help
overlay. The root `help.png` is historical data only. No change to `cnanext` or
`sharp-runtimenext` was required: the public XNA-shaped `BasicEffect`, bounding-volume,
`DrawUserPrimitives`, viewport and input APIs already supported the faithful translation.

## Verification evidence

All generated source snapshots, builds, scripts, logs and captures are under
`/rv/tmp/samples/SAMPLE-008-ShapeRenderingSample_4_0`:

- `xna4-build/bin/ShapeRenderingSample.exe` is the Debug x86 XNA 4.0 reference executable.
  `evidence/xna-original/shape-rendering-xna-original.png` is its real 800x480 Wine/WineD3D
  capture. The live native-Wayland original also displayed and exited normally.
- `cna-native-opengles3/samples/ShapeRendering/ShapeRendering_cna_samples` is the Debug native
  reference build. It reports EasyGL OpenGL ES 3.2 and `OPENGLES3`.
  `evidence/cna-native-opengles3/shape-rendering-native.png` is the deterministic 800x480 capture;
  synthetic Escape exited with status 0. A separate unforced live GNOME Wayland run displayed on
  the real desktop and exited with status 0 after Escape.
- `cna-web-webgl2/samples/ShapeRendering/ShapeRendering_cna_samples.{html,js,wasm}` is the complete
  browser bundle. System Google Chrome loaded all three files with HTTP 200, reported Chromium
  WebGL 2.0 and `CNA: graphics renderer: WEBGL2`, rendered the scene, and produced no application,
  wasm or WebGL runtime error. The capture is
  `evidence/cna-web-webgl2/shape-rendering-webgl2.png`.

The original and native 800x480 captures have the same background, geometry and colors. The
separately started original, native and browser processes are captured at different points on the
time-driven camera orbit, so their projected positions are intentionally not pixel-identical.
