# SAMPLE-099 — ModelImporterSample_4_0 audit

**Status: complete.** The runtime sample is a faithful C++ port driven by the exact
XNA Game Studio 4.0 content output. The original design-time importer was recovered,
built and audited, but is not misrepresented as a CNA authoring-pipeline port.

## Source and behavior

The port retains the original `ObjImporterSample::ObjImporterGame` identity and its
complete runtime behavior:

- `Content.Load<Model>("Tank")` during `LoadContent`;
- base update before Escape/gamepad Back exit handling;
- a Cornflower Blue clear and time-based Y rotation using total seconds divided by
  five;
- the original `(0, 200, 350)` camera and `(0, 35, 0)` look-at point;
- absolute bone-transform copying before drawing every mesh;
- default lighting plus the original world, view and 45-degree perspective matrices;
- base draw after all model meshes have been submitted.

No raw-OBJ runtime loader, alternate model, sample-side parser or other framework
workaround was introduced.

## Authentic source recovery and content

The pre-existing normalized local snapshot lacked `Tank.obj`. The complete official
1.49 MB package was therefore recovered and preserved with SHA-256:

```text
ece748c7f465f04e3e620d7e2881743afa34989896cf6628f6d5108daa6ed8f1  ModelImporterSample_4_0.zip
06964f9ccb3b31498a4218de6c03a10794e8f7f5a3fce1363d99afc9ad7bbd4a  Tank.obj
```

A normalized `diff -qr --strip-trailing-cr` comparison proved that `Tank.obj` was
the only file absent from the local snapshot. The recovered OBJ has 10,910 positions,
14,312 texture coordinates, 10,910 normals, 12 groups, 12 material selections and
21,610 triangular faces.

The unchanged 596-line `ObjImporter.cs` was built as an XNA Content Pipeline
extension and processed the recovered OBJ, MTL and both TGA textures through the
original `ObjImporterSample.ObjImporter -> ModelProcessor` route. The committed
runtime assets are byte-identical to that authentic Windows/Reach output:

```text
11d7d7b5fd7b3bd59be6c1e83bb7f8c8e76f651912350479d26a4c7e374796a1  Content/Tank.xnb
71e35727ae856950f3c524b926246352a8a4ac17d138e5df5340cfe17fbb55c0  Content/engine_diff_tex_small_0.xnb
ee666d84c97951048c898af5aa1b4b2b06a060b8a47fe907873d75ee072d89dc  Content/turret_alt_diff_tex_small_0.xnb
```

The original documentation, image and license are retained at sample root and are
not loaded by the game.

## Design-time boundary

The original importer was reviewed in full. It parses OBJ positions, texture
coordinates, normals, groups, triangle faces, material selections and MTL libraries;
tracks source dependencies; reverses winding for XNA; emits mesh channels and
`BasicMaterialContent`; and resolves texture references while preserving source-line
identity in errors. It intentionally rejects unsupported face shapes and line input.

Under the owner-approved `SAMPLES-DEC-002` boundary, this sample qualifies the
runtime game with exact pregenerated XNB output. CNA does not currently provide an
XNA-compatible C++ authoring-time `ContentImporter`/`MeshBuilder` extension surface,
so the 596-line design-time assembly is audited and preserved as reference evidence,
not falsely labelled as ported runtime code.

## Original XNA reference

The unchanged importer, Windows/Reach content project and game compiled with the
XNA Game Studio 4.0 toolchain. The game executable has SHA-256:

```text
44351e3b0e266746f4ee4acec8b1d294a5d9da2ca6370e90adc066d693e43074  ObjImporterGame.exe
```

An isolated reference run loaded the generated model and textures, displayed the
fully textured tank, visibly rotated it between the two- and five-second captures,
and exited cleanly with Escape.

Reference source, build output, scripts, logs and captures are preserved under:

```text
/rv/tmp/samples/SAMPLE-099-ModelImporterSample_4_0/
```

## CNA qualification

- Debug OPENGLES3 build and real X11 run: passed; model and both external textures
  loaded, two- and five-second frames differed, and Escape exited cleanly.
- Clean Release OPENGLES3 build and real X11 run: passed with the same visible model,
  rotation and clean exit.
- Focused CNA XNB/model/texture/external-reference regression: 22/22 tests passed on
  a real OPENGLES3 context.
- WebGL2 Release build: passed.
- Real Chrome: the 800x480 canvas used WebGL 2.0, displayed the textured tank with
  more than 100 distinct captured colours, changed frame hash as the model rotated,
  and completed 600/600 `requestAnimationFrame` callbacks. There were no runtime
  exceptions, unhandled promise rejections, fatal console messages or relevant HTTP
  errors, and all HTML/JS/Wasm/data artifacts returned HTTP 200.

No CNA, Sharp Runtime, EasyGL or MetaGL source change was required by this sample.
All browser and native evidence, including reusable qualification scripts, remains
in the artifact directory above.
