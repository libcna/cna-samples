# SAMPLE-002 audit — Primitives3DSample_4_0

**Status:** reopened on 2026-09-02. The game/code audit remains useful, but the content gate is not
complete: the port ships `hudfont.cnj` plus a loose PNG instead of the original pipeline-built
`hudFont.xnb`.

## Reopened content-fidelity defect

The original content project compiles `hudFont.spritefont` through the Microsoft XNA 4.0
SpriteFont pipeline and the game loads the identifier `hudfont` as `SpriteFont`. The current CNA
port preserves the load call but replaces the pipeline product with `Content/hudfont.cnj` and
`Content/hudfont.png`. That is a loose font-sidecar substitute forbidden by `rules.md` now that CNA
loads authentic SpriteFont XNBs.

The retained reference was also not a qualifying official-pipeline build: the historical audit
states that `build-tools/make_spritefont_xnb.py` synthesized a compatible XNB because the local
content compiler was believed unavailable. The prepared offline Win7/XNA Game Studio path has
since disproved that premise. Completion requires building the unchanged `hudFont.spritefont`
with the real XNA 4.0 Content Pipeline, checking in that exact XNB, removing the CNJ/PNG substitute,
and repeating the native and real-browser gates. Until then SAMPLE-002 must not be marked complete.

## Reference and evidence

- Upstream source: `/rv/tmp/XNAGameStudio/Samples/Primitives3DSample_4_0`.
- Preserved exact source tree and the built XNA reference:
  `/rv/tmp/samples/SAMPLE-002-Primitives3DSample_4_0/xna4-original/`.
- Native CNA build: artifact root plus
  `cna-native-opengles3/build/samples/Primitives3D/Primitives3D_cna_samples`.
- Web CNA build: artifact root plus `cna-web-webgl2/build/samples/Primitives3D/`.
- Captures and runtime logs: `/rv/tmp/samples/SAMPLE-002-Primitives3DSample_4_0/evidence/`.

The unmodified C# game and primitive sources were compiled on Linux against the installed real
XNA 4.0 assemblies. The historical run used a script-synthesized compatible XNB and therefore
remains behavioral evidence only, not proof of authentic content provenance. The resulting
executable ran under the real XNA 4.0 runtime with WineD3D and rendered the reference captured in
`primitives3d-xna-original-wined3d.png`.

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
  source are present; the invented F1/help overlay was removed, while its historical `help.png`
  remains beside `CMakeLists.txt`, outside `Content`, and is not loaded;
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

## Historical content work (superseded by the reopened defect)

The shared font generator formerly emitted an obsolete `.font.json` descriptor and an
unpremultiplied RGBA atlas. It was changed to emit a CNJ envelope and premultiplied atlas with the
original 95 characters. That improved the historical visual comparison, but it is still a
sample-local replacement for the original SpriteFont XNB and is not accepted under the current
content policy.

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
