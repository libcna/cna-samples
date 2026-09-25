# HeightmapCollision — port notes

Upstream: `HeightmapCollisionSample_4_0` (SAMPLE-049). **Re-ported from scratch.** Ported whole — the
terrain the content pipeline generates from a bitmap, the rolling sphere, the heightmap collision,
the chase camera and the fog.

Artifact root: `/rv/tmp/samples/SAMPLE-049-HeightmapCollisionSample_4_0/`.

## What the 2026-07-09 pass did, and what the row asked for

That pass **built the terrain at run time** in C++ and **constructed `HeightMapInfo` directly**,
because it could not run the sample's `ContentProcessor` or read a `Model.Tag`. The `plan.md` row
asked to verify the terrain generation, the texture/model path and the collision values; all three
now come from the pipeline, and all three are measured below.

Its other three findings were the familiar ones and none survives: the "flat, fully-saturated white
sphere" (an artefact of the hand-built model, not the renderer), the `.model.json` 16-bit index
limit (irrelevant once the real `.xnb` is used — this terrain has 131 072 triangles), and
`Clear(Color)` not clearing depth (fixed in Task 928).

## The sample's own content pipeline extension

Third sample in the campaign with a `ContentProcessor` of its own, and the first with a
`ContentTypeWriter` too. `TerrainProcessor` is a `ContentProcessor<Texture2DContent, ModelContent>`:
it converts `terrain.bmp` to a float bitmap, builds a grid mesh whose vertex heights are the pixels,
converts that to a `ModelContent` through the stock `ModelProcessor`, and attaches a
`HeightMapInfoContent` to `Model.Tag`. `scripts/build-original.sh` compiles
`HeightmapCollisionPipeline.dll` and hands it to `BuildContent` in `PipelineAssemblies`.

**Two listed assets produce four `.xnb`.** The processor's own default `TerrainTextureFilename` is
`"rocks.bmp"`, which it resolves beside the input — so the terrain texture arrives without ever
being a row in the content project, and `pawball.tga` comes in the same way from the sphere's FBX
material.

**HiDef is not optional here, and that is measured.** The Windows project declares HiDef; building
the same content for Reach **fails**, in the pipeline's own words:

> XNA Framework Reach profile supports a maximum of 65535 primitives per draw call, but this
> ModelMeshPart contains 131072 triangles.

## Reading it back: a game-supplied ContentTypeReader

The writer records `HeightmapCollision.HeightMapInfoReader` as the runtime reader, and XNA finds
that class by reflecting over the game assembly. C++ has no reflection, so the port registers the
reader under that name — one line, and the only line the original does not have. See
[`diff.md`](diff.md). `ContentTypeReaderManager::AddTypeCreator` already existed; nothing was added
to CNA for this sample.

`HeightMapInfo` derives from `System::Object` so `Model::Tag` can carry it, and
`terrain.Tag as HeightMapInfo` becomes a `dynamic_cast` — a checked cast that yields null on a
mismatch, which is what `as` does. The game still throws the original's own message if it is null.

## Historical agreement with real XNA 4.0 (2026-09-09)

The sphere moves only on input, so the scene is deterministic; `../../../cna-diag/` and
`../../../xna4-diag/` add `CNA_SPHERE`, `CNA_FACING` and `CNA_PROBE` to both engines.

| sphere pinned at | within 0 | within 8 | within 32 | after 4 px blur |
|---|---|---|---|---|
| the centre | 68.5 % | **99.97 %** | **100.00 %** | **100.00 %** |
| east (600, −300) | — | **99.99 %** | **100.00 %** | **100.00 %** |
| north-west (−900, 900) | — | **99.98 %** | **100.00 %** | **100.00 %** |
| south (0, 1200) | — | **99.98 %** | **100.00 %** | **100.00 %** |

With no hook at all the start frames agree on **99.97 %** within 8 and **100.00 %** within 32, mean
absolute difference **0.148 of 255**, coverage 100.000 % on both sides and identical centroids. All
eight frame hashes across the four legs are distinct, so the hook is doing something and the metric
is not degenerate.

**The collision values themselves are exact, which is what the row asked.** `CNA_PROBE` prints
`IsOnHeightmap` and `GetHeight` over an 81-point grid spanning the terrain, in both engines:

- `IsOnHeightmap` agrees at **all 81 points**;
- `GetHeight` agrees at **all 81 points, bit-identical as `float32`, 0 ULP**.

That last number needed care: compared as decimal text the two logs differ by up to 3 × 10⁻⁵,
because C#'s `"R"` and C's `%.9g` print the same float with different digit counts. At a height of
about −620 one float ULP is 6.1 × 10⁻⁵, so the printed difference is **less than one representable
step** — parsing both back to `float32` shows they are the same value. Comparing the strings would
have reported a difference that does not exist.

## Historical WebGL2 result (2026-08-28)

`WEBGL2` built under a real Emscripten toolchain and driven in real Google Chrome:

- **a rendered frame at all is already the strongest check here**, because the game throws the
  original's own `InvalidOperationException` if the terrain model has no `HeightMapInfo` on its
  `Tag` — so a picture proves the processor, the writer, the `.xnb` and the game-registered reader
  all worked end to end;
- the numbers are calibrated against the native frame: sphere 4426 px and rock 69.38 % natively,
  **4425 px and 69.69 %** in Chrome;
- nothing moves without input (hash-identical), driving forward moves the sphere — which is the
  collision path running every frame — and turning swings the camera;
- **one gate threshold was corrected rather than left to pass by luck.** Requiring black fog in
  *every* frame failed the driven-forward leg at 0.12 %, because there the camera sits at the foot
  of a hill that fills the frame. That is a correct picture, so the fog check now applies to the
  three frames that look at the horizon and says so.

## Evidence

- `evidence/build-original.log` — the pipeline, including the Reach refusal quoted above.
- `evidence/frozen/<leg>/{xna,cna}/` — one directory per pinned sphere position, and `probe/`
  carries both engines' collision-value logs.
- `evidence/cna-web-webgl2/browser-result.json` — the browser gate's own numbers.

## Current-head requalification (2026-09-25)

The retained `xna4-original/` matches all **26 physical upstream files** at their original
paths and SHA-256 values. The unchanged Windows and Xbox projects both select HiDef. The
sample-owned `TerrainProcessor` and `HeightMapInfoWriter` compiled and ran in the official
Windows/HiDef XNA pipeline; the unchanged game built and ran under Wine with
`/home/robertvokac/.wine-cna-xna40` and `WINEDLLOVERRIDES=d3d9=b`. All four freshly built XNBs
match checked-in content and the native product byte for byte. The port now also retains the
original license, game icon, sample screenshot and the three small illustrations the retained
HTML references.

The old port omitted one **project setting**: XNA embeds `<XnaProfile>HiDef</XnaProfile>` from
both `.csproj` files, but the C++ executable used CNA's Reach default. A fresh native build
therefore loaded the terrain and immediately threw "Thirty-two-bit index buffers are not
supported by the Reach graphics profile." `src/Properties/AssemblyInfo.cpp` now records HiDef
through CNA's existing general `ProjectGraphicsProfileEXT` mechanism, just as SAMPLE-040 does.
This is a translation of project metadata, not a game workaround; `diff.md` records it. No
change to CNA or sharp-runtime was needed. The game and heightmap reader logic remain unchanged.

Fresh native OPENGLES3 and real XNA default frames match for **99.97% of pixels within eight
levels**, 100.00% within 32 and 100.00% after a 4px blur. The current frozen diagnostic pair,
built in an isolated source shadow without swapping checked-in files, gives 99.97% at the
centre, 99.98% east, 99.97% north-west and 99.98% south. All **81** checked grid positions
agree on `IsOnHeightmap` and bit-identical `float32` `GetHeight` values. Ordinary original and
native runs respond to forward and turn keys; held Escape closes each game window cleanly.

The fresh four-file WebGL2 bundle runs in system Google Chrome from static HTTP with threads
disabled. It renders the textured terrain, sphere and horizon fog, remains still without input,
moves the sphere on Up and turns the chase camera on Left. The current browser gate measures
4,445 sphere pixels against 4,442 native, no runtime exception, HTTP failure or fatal console
message. The identical gallery copy passes the same gate; page 4 includes the 48th card,
detail page, screenshot and thumbnail. The web page's tightened gate now bounds sphere and
terrain measurements to the current native reference rather than the old broad thresholds.

Current reproduction is in `scripts/build-original.sh`, `scripts/build-current.sh`,
`scripts/capture-original.sh`, `scripts/capture-cna-native.sh`, `scripts/compare-frozen.sh`
and `scripts/capture-web.sh`. Logs and captures are under
`evidence/requal-20260925/` and adjacent `evidence/requal-20260925-*.log` files.
