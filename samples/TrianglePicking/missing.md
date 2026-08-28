# TrianglePicking — port notes

Upstream: `TrianglePickingSample_4_0` (SAMPLE-048). **Re-ported from scratch.** Ported whole — the
table and four models, the camera, the cursor, the per-triangle picking, the magenta wireframe of
the picked triangle and both lines of HUD text.

Artifact root: `/rv/tmp/samples/SAMPLE-048-TrianglePickingSample_4_0/`.

## The generated sidecar is gone

The 2026-07-09 pass could not read the sample's per-triangle data, so it **generated a
`TrianglePickingData.hpp` sidecar** holding the vertices as C++ source and filed the gap as
DEFERRED item #25. That sidecar is what the `plan.md` row asked to remove, and it is gone: the data
now comes from the `.xnb` the real pipeline wrote.

## The sample's own content pipeline extension

This is the second sample in the campaign with a `ContentProcessor` of its own (after SAMPLE-042's
`ShatterProcessor`). `TrianglePickingProcessor` derives from `ModelProcessor`, chains to
`base.Process`, walks the node tree and attaches to `Model.Tag` a `Dictionary<string, object>`
holding:

- `"Vertices"` — every world-space triangle vertex, three per triangle, as `Vector3[]`;
- `"BoundingSphere"` — `BoundingSphere.CreateFromPoints` over the same list.

`scripts/build-original.sh` compiles `TrianglePickingPipeline.dll` first and hands it to
`BuildContent` in `PipelineAssemblies`. Seven listed assets produce **10 `.xnb`**, because the FBX
materials pull `cat.tga` and the wood texture in on their own. Windows/Reach end to end, as the
`.csproj` declares.

## Two framework gaps, both found by decoding the real file

The `.xnb`'s type-reader table was read out of the built `Sphere.xnb` rather than guessed at:

```text
[1] ModelReader   [2] StringReader   [3] DictionaryReader`2[[String],[Object]]
[4] ArrayReader`1[[Vector3]]   [5] Vector3Reader   [6] BoundingSphereReader
[7] VertexBufferReader   [8] VertexDeclarationReader   [9] IndexBufferReader
[10] BasicEffectReader
```

**1. `ArrayReader<Vector3>` was never registered** (`cnanext 7b312232d`). `ArrayReader<T>` existed
as a template, but no instantiation of it was registered for any element type — the only collection
readers wired up were three `ListReader`s for `SpriteFont`. An `.xnb` naming
`ArrayReader\`1[[Vector3]]` therefore could not resolve its table, and because a table must resolve
**in full** before a single object is read, the whole model failed rather than just the array.

**2. `Model.Tag` could not carry a dictionary** (`cnanext 0a32dba89`). `ModelReader::ReadTag`
accepted only a value deserialising as `std::shared_ptr<System::Object>`;
`DictionaryReader<String, Object>` produces a `std::map<std::string, std::any>`, which is not one.
Searching the tree for a reader that *does* produce that shape found exactly one — **a test
fixture** — so `Model.Tag` had never carried real content at all. C++ has no `object`, so the
owner chose the shape: a `CNA::Content::ObjectDictionaryEXT` carrier, reached by `dynamic_cast` and
read with a typed `Get<T>`, each entry keeping the type its own reader produced. The port's
`RayIntersectsModel` reads it exactly as the C# reads `model.Tag`.

## Agreement with real XNA 4.0

The camera answers only to input, so it is already deterministic; the cursor is the one thing that
is not, and it decides which model is picked. `../../../cna-diag/` and `../../../xna4-diag/` add a
single `CNA_CURSOR="x,y"` hook to both engines.

| cursor | within 0 | within 8 | within 32 |
|---|---|---|---|
| on the P2Wedge | 76.0 % | **98.23 %** | **99.99 %** |
| on the Cats box | 76.0 % | **98.25 %** | **99.99 %** |
| on the sphere | 76.0 % | **98.23 %** | **99.99 %** |
| off every model | 76.0 % | **98.23 %** | **99.99 %** |

Without any hook at all — both engines simply left the pointer where their X server put it — the
frames still agree on 97.83 % within 8 and 99.64 % within 32, coverage matching to 67.522 % /
67.523 % and the centroids to two decimals.

**The picking itself is exact, and that is the stronger measurement.** Counting the white HUD text
and the magenta wireframe of the picked triangle in each leg:

| cursor | XNA white / magenta | CNA white / magenta |
|---|---|---|
| on the P2Wedge | 1048 / 0 | 1048 / 0 |
| on the Cats box | 973 / 326 | 973 / 326 |
| off every model | 401 / 0 | 401 / 0 |
| on the sphere | 985 / 27 | 985 / 28 |

The counts differ *between* legs, so the metric is not degenerate — and they match *between
engines* to the pixel in three legs and to one pixel in the fourth. The two engines pick the same
triangle of the same model. That exercises the whole chain at once: the `Vector3[]` and the
`BoundingSphere` out of the `Tag`, the ray transformed into object space by the inverted world
matrix, and the Möller-Trumbore test.

## Web

`WEBGL2` built under a real Emscripten toolchain and driven in real Google Chrome:

- the table is drawn and **textured** — the wood texture is not a content-project row, the
  `FbxImporter` resolves it from the material, so brown pixels pin that path;
- **`magentaPixels` is the gate this sample is pinned by**, calibrated against the native frames:
  326 with the cursor on the Cats box, 0 with it off every model. Chrome measured **326** and **0**
  — the same triangle, in the browser. That count is non-zero only if both framework fixes landed;
- the name follows the picking (white pixels 400 → 972) and the camera answers to the arrow keys;
- no runtime exceptions, no HTTP errors, no fatal console messages, `WEBGL2` renderer logged.

## Evidence

- `evidence/build-original.log` — the pipeline, including the sample's own processor.
- `evidence/frozen/<leg>/{xna,cna}/` — one directory per pinned cursor position.
- `evidence/cna-web-webgl2/browser-result.json` — the browser gate's own numbers.
