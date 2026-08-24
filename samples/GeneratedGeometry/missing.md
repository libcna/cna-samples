# SAMPLE-012 audit — GeneratedGeometrySample_4_0

No known behavioral or visual differences remain after the `SAMPLE-012` audit. There is no
unresolved CNA or sharp-runtime implementation gap for this sample.

## Reference and source audit

- The unchanged upstream snapshot is retained under
  `/rv/tmp/samples/SAMPLE-012-GeneratedGeometrySample_4_0/xna4-original`.
- `GeneratedGeometry.cs`, `Sky.cs`, `TerrainProcessor.cs`, `SkyProcessor.cs`, `SkyContent.cs`,
  both project files and the Content project were reviewed line by line. The Windows Reach project
  is the runnable XNA 4.0 reference.
- `scripts/build-original.sh` builds the unchanged custom processor assembly, invokes the real XNA
  4.0 Content Pipeline through the dedicated Wine prefix, compiles the x86 game assembly and stages
  the generated content. `scripts/capture-original.sh` runs that real XNA game through WineD3D.

The C++ translation preserves the original 800x480 presentation, rotating camera, projection,
terrain `BasicEffect` configuration, default lighting, warm specular term, camera-space fog,
far-plane skydome projection, depth-read sky draw, `WrapUClampV` sampler and Escape/Back exit path.
It also retains the original `#if WINDOWS_PHONE` constructor branch that selects a 333,333-tick
target frame interval and fullscreen mode. The Windows, Linux OPENGLES3 and browser reference
builds correctly leave that branch inactive.
Normal C++ value ownership, `std::optional`, `std::cos`/`std::sin`, CNA's property convention and
`CNAEXT GetTypeName()` are the only representation-level adaptations.

## Exact XNA content-pipeline output

The former port generated the terrain and sky meshes at runtime from loose BMP files. It also
loaded a non-original F1 help overlay and forced both models through `CullNone`. All of those
workarounds have been removed.

The audited port now consumes the exact products of the unchanged Microsoft processors:

| Asset | Original pipeline path | SHA-256 |
|---|---|---|
| `terrain.xnb` | `terrain.bmp` → `TerrainProcessor` → `ModelProcessor` | `8b8527739c18fc52b2ef41ce9a22d44f9b8c1bb3da261a26c9410d7f09602d12` |
| `rocks_0.xnb` | external terrain material → `TextureProcessor` | `e25b7875c42b23f9ba10c2cfbc80e11bf70e5bec9af8dfce46caba5d94264c57` |
| `sky.xnb` | `sky.bmp` → `SkyProcessor` → `ReflectiveWriter<SkyContent>` | `586d055a535d43b3d1f6ce40fd843848bf1fddfb1d04910cabb59042b1aec460` |

The three checked-in files are byte-identical to the retained XNA pipeline outputs. `terrain.xnb`
loads through CNA's stock `ModelReader`, including its external `rocks_0` texture. `sky.xnb` names
the original runtime type `GeneratedGeometry.Sky` and contains the processor-generated `Model` and
uncompressed `Texture2D`. C++ cannot instantiate the original CLR reflective runtime type, so the
sample registers the closed AOT `SkyReader` equivalent and reads those same two serialized fields
in their original order. This is the same typed content-reader boundary used by XNA, not geometry
generation or a content substitute.

On 2026-08-24 the owner explicitly accepted exact pregenerated XNB output as the faithful runtime
content boundary for samples. The unchanged processor sources and reproducible official-pipeline
build remain in the audit artifacts, while the game port consumes their exact products just as the
original runtime does. This decision does not claim that CNA implements the design-time
`ContentProcessor` authoring API and does not classify a standalone pipeline tool as ported.

Historical `help.png` is retained beside the sample's `CMakeLists.txt`, outside `Content`, and is
not loaded, copied or preloaded.

## No-workaround and framework review

The audited code has no runtime mesh generator, raw model helper, loose image substitute, direct
`SetData` replacement, handwritten shader, backend call, invented input, omitted branch, culling
override or help overlay. It uses only the XNA-facing CNA API and draws the official models with
their pipeline-created vertex/index buffers and `BasicEffect` instances.

The previous `missing.md` conclusions about ignored specular lighting, object-space fog and broken
winding described an obsolete handwritten EasyGL/sample path. Current `cnanext` executes the XNA
compiled `BasicEffect` on EasyGL, applies the stock XNB model data, renders both meshes with the
default `CullCounterClockwise`, and honors the custom sky sampler. The live result reproduces the
XNA terrain lighting, fog, skydome projection and culling without a framework change. No
sharp-runtime change was needed.

## Verification evidence

All source snapshots, generated files, builds, scripts, logs and captures are under
`/rv/tmp/samples/SAMPLE-012-GeneratedGeometrySample_4_0`:

- `xna4-build/bin/GeneratedGeometry.exe` is the real XNA 4.0 x86 Windows reference. It loads the
  three official pipeline outputs, renders the animated terrain/skydome scene and exits on Escape.
- `cna-native-opengles3/samples/GeneratedGeometry/GeneratedGeometry_cna_samples` reports EasyGL
  OpenGL ES 3.2 and `OPENGLES3`. An isolated eight-second stability run reached its timeout, and
  the capture run rendered the same 800x480 scene with default culling and exited on Escape.
- `cna-web-webgl2/samples/GeneratedGeometry/GeneratedGeometry_cna_samples.{html,data,js,wasm}` is
  the complete browser bundle. System Google Chrome fetched all four files with HTTP 200, reported
  WebGL 2.0 and `CNA: graphics renderer: WEBGL2`, loaded all three XNB assets, rendered the animated
  800x480 scene and produced no application, wasm or WebGL runtime error.

The first shared-desktop native capture was closed externally while several Codex agents were using
the same display. The retained validation and final captures use isolated Xvfb displays, preventing
unrelated windows or synthetic input from affecting the sample while still exercising the real
OPENGLES3 renderer and the system Chrome executable.
