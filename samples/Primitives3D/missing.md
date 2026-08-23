# SAMPLE-002 audit — Primitives3DSample_4_0

**Status:** complete. No known missing behavior, workaround, sample-local substitute, owner
decision, or acceptance gate remains for this sample.

## Reference and evidence

- Upstream source: `/rv/tmp/XNAGameStudio/Samples/Primitives3DSample_4_0`.
- Preserved exact source tree and the built XNA reference:
  `/rv/tmp/samples/SAMPLE-002-Primitives3DSample_4_0/xna4-original/`.
- Native CNA build: artifact root plus
  `cna-native-opengles3/build/samples/Primitives3D/Primitives3D_cna_samples`.
- Web CNA build: artifact root plus `cna-web-webgl2/build/samples/Primitives3D/`.
- Captures and runtime logs: `/rv/tmp/samples/SAMPLE-002-Primitives3DSample_4_0/evidence/`.

The unmodified C# game and primitive sources were compiled on Linux against the installed real
XNA 4.0 assemblies. The local installation does not include the XNA content compiler, so the
`hudFont.spritefont` source was built into a compatible XNB from the exact locally archived
Segoe UI Mono TTF, size 10, and the source character range 32--126. The resulting executable was
then run by the real XNA 4.0 runtime under Wine with WineD3D; the runtime accepted the XNB and
rendered the reference captured in `primitives3d-xna-original-wined3d.png`.

## Line-by-line comparison result

The Windows XNA 4.0 branch is retained:

- the five primitives are created in the original order (cube, sphere, cylinder, torus,
  teapot), with the same defaults and construction formulae;
- the custom vertex remains exactly position `Vector3` at byte 0 plus normal `Vector3` at byte
  12, stride 24; no color, texture coordinate, raw mesh, or dummy UV was introduced;
- `BasicEffect.EnableDefaultLighting()`, matrices, rotation rates, depth/blend/rasterizer state,
  tint cycle, and indexed triangle drawing match the original;
- keyboard, gamepad, mouse regions, edge-triggered input, Escape/Back exit, color/primitive
  cycling, and wireframe toggle retain the original branches;
- the exact three HUD strings, line breaks, `(48, 48)` position, white color, and original font
  source are present; the invented F1/help overlay and `Content/help.png` were removed;
- `ArgumentOutOfRangeException` and `IDisposable` behavior replace the port's former generic
  exceptions and omissions.

A mechanical data comparison additionally confirmed all 127 teapot control points and all 160
patch indices are numerically identical to the C# source. `Primitives3D.htm` is byte-identical to
the upstream file. The bypass scan found no `RawMesh`, `RawModel`, `SetDataRaw`,
`VertexPositionNormalTexture`, dummy data, placeholder, F1 overlay, or backend helper in the
sample. `CNAEXT GetTypeName()` is the one required C++ runtime extension.

One representation-level C++ deviation is intentional: the sample vertex exposes the original
declaration getter but does not inherit CNA's currently polymorphic `IVertexType`, because that
would insert a vptr ahead of the fields and destroy the original 24-byte GPU layout. The explicit
XNA `VertexBuffer(GraphicsDevice, VertexDeclaration, ...)` path preserves the source declaration
and behavior without modifying the vertex data.

## CNA defects found and fixed

The old port uploaded `VertexPositionColor`, discarded every source normal, and rendered a flat
tint. Its previous recommendation to invent a zero UV and use
`VertexPositionNormalTexture` was a forbidden workaround. SAMPLE-002 instead fixed CNA:

1. `VertexBuffer::SetData<T>` now uploads a trivially-copyable application-defined XNA vertex
   type according to the buffer's declaration.
2. EasyGL stock-effect selection now recognizes the exact position+normal declaration. It no
   longer mistakes this 24-byte layout for `VertexPositionColorTexture`, and it uses the real
   BasicEffect lit shader without requiring a texture coordinate.
3. A byte-exact generic VertexBuffer test and a real OPENGLES3 pixel test cover the new path;
   the latter proves a forward normal receives the directional contribution and a reversed
   normal does not.

No sharp-runtimenext change was required. The existing `System::ArgumentOutOfRangeException` and
`System::IDisposable` contracts were sufficient.

## Content and build fixes

The shared font generator formerly emitted an obsolete `.font.json` descriptor and an
unpremultiplied RGBA atlas. Current CNA loads canonical SpriteFont `.cnj`, while XNA AlphaBlend
expects premultiplied atlas pixels. `tools/make_font.py` now emits the current CNJ envelope and a
premultiplied atlas; the regenerated `hudfont` has the original 95 characters and no invented
default character. This removed the heavier fringe that appeared in the first CNA capture.

Native content deployment also formerly ran only after a relink. The shared CMake helper now
synchronizes `Content` whenever a native sample target is built, so an asset-only edit cannot
leave a stale runnable artifact.

## Verification

- XNA 4.0 original: real runtime rendered the rotating, lit red cube and exact HUD at 800x480;
  capture and log preserved under `evidence/`.
- CNA native: Release build against `../cnanext` and `../sharp-runtimenext`, renderer reported
  `OPENGLES3` / EasyGL OpenGL ES 3.2; visual output matches the reference. An isolated input run
  captured A changing cube to sphere, B changing red to green, Y changing solid to wireframe, and
  Escape exiting with status 0.
- CNA web: Release Emscripten build against the same sibling checkouts, renderer restricted to
  `WEBGL2`; all `.html`, `.js`, `.wasm`, and `.data` outputs were generated, served over HTTP with
  status 200, and the JS package map contains both `Content/hudfont.cnj` and
  `Content/hudfont.png`. Google Chrome 151 reported Chromium WebGL 2.0 and
  `CNA: graphics renderer: WEBGL2`, rendered the same lit scene and HUD, and produced no
  application console error. Captures prove A changed cube to sphere, B changed red to green, and
  Y changed solid to wireframe. After Escape, an additional A produced a byte-identical screenshot,
  proving that the web game loop had stopped.
- CNA tests: generic VertexBuffer upload test passed; the EasyGL Position+Normal lighting pixel
  test passed both lit and reversed-normal assertions; affected CNA targets built successfully.
