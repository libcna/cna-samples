# BillboardSample — port notes

Upstream: `BillboardSample_4_0` (SAMPLE-039). **Status: ported and running, NOT yet verified
against the original.** The port builds, loads all content and renders the scene; a real
geometry difference in the billboards is open and characterised below.

This file replaces a version that recorded "custom `Billboard.fx` HLSL shader" as a blocker.
That is stale — compiled effects have worked since SAMPLE-032 — and this sample's effect loads
and runs.

## Content

One source asset, `landscape.x`, built by the sample's own `VegetationProcessor`
(`BillboardPipeline`) which emits the billboard geometry, points the material at `Billboard.fx`
and pulls in the three textures. Three things this sample was the first in the campaign to need:

- **The stock `XImporter`.** `landscape.x` is a DirectX `.x` model, so
  `Microsoft.Xna.Framework.Content.Pipeline.XImporter.dll` has to be declared in
  `PipelineAssemblies` beside the texture, effect and FBX importers.
- **The HiDef profile, end to end.** The sample declares `<XnaProfile>HiDef</XnaProfile>` and
  means it: `VegetationProcessor` emits one mesh part of 82 672 triangles and the Reach profile
  refuses anything past 65 535 primitives per draw call, so the Windows/Reach leg fails **by
  design** and `Content-hidef/` is the content that ships. The `RuntimeProfile` resource embedded
  in the executable must say `Windows.v4.0.HiDef` too — with the Reach resource every earlier
  sample used, the game links, starts, and dies in `LoadContent` with *"This file was compiled
  for the HiDef profile, and cannot be loaded into a Reach GraphicsDevice"*. The profile is
  enforced at **load** time, not at build time.
- **No `params` array in the pipeline runner.** The runner is compiled with Mono's `mcs` and runs
  on .NET Framework 4.0 inside the Wine prefix; for an empty `params` argument `mcs` emits a call
  to `System.Array.Empty<T>()`, which 4.0 does not have, and the content build dies with
  `MissingMethodException` before importing anything. The helper now takes explicit overloads.

## The open difference: billboard geometry

The terrain matches; the vegetation does not. Measured with the wind phase pinned in both engines
(`scripts/compare-frozen.sh`, `cna-diag/README.md`):

| Configuration | Pixels within 8 levels |
|---|---|
| as shipped | **74.9 %** |
| wind disabled in both | 75.0 % — wind is not the cause |
| alpha test disabled in both | 72.3 % — the alpha test is not the cause |
| all billboards forced to one size (8) | **87.0 %** |
| all billboards forced to size 40 | **0.0 %** — every pixel differs |

The sky is **exactly** identical and the terrain's horizon agrees to a pixel in every column
where terrain, not vegetation, forms it — so the camera, projection and ground path are right.
With the alpha test off, both engines draw solid quads and CNA's are visibly larger and
differently placed, which is what the size-40 row makes unmissable: this is geometry, not
filtering and not sub-pixel noise.

### What has been eliminated, with evidence

Every layer reachable from outside the shader was checked and is correct in CNA:

- **The vertex declaration**: `Position` Vector3 @0, `Normal` Vector3 @12, `TextureCoordinate0`
  Vector2 @24, `TextureCoordinate1` **Single** @32, stride 36 — exactly what `VegetationProcessor`
  writes and what `Billboard.fx`'s `VS_INPUT` expects.
- **The vertex data**: read back from the buffer, the first quad is four vertices at one position
  `(-100.81, 40.28, -86.02)` with one normal `(0.429, 0.897, 0.106)`, the four UV corners and a
  shared `Random` of −0.024. The next quad's is 0.592. That is the layout the shader wants.
- **The attribute binding**: the shader's inputs map to the right elements —
  `vs_v0`←POSITION0@0, `vs_v1`←NORMAL0@12, `vs_v2`←TEXCOORD0@24, `vs_v3`←TEXCOORD1@32.
- **The per-part effect parameters**: each of the three parts has its own `Effect` with its own
  `BillboardWidth`/`BillboardHeight` (5/5, 12/12, 5/5), and forcing CNA to a single size produces
  a frame different from the unforced one, so the per-part values do reach the shader.
- **`ModelMesh::Draw`** applies each part's own effect before that part's draw.
- **The generated GLSL** declares `vs_uniforms_vec4[19]` for registers c0–c18 with a contiguous
  1:1 mapping, and emits c19–c22 as inline constants — so FX-122's running-index hazard does not
  apply here.
- CNA is **deterministic**: two runs at the same pinned wind phase are byte-identical.

### Next step

The difference is inside the translated vertex shader's execution rather than in anything feeding
it. The next thing to measure is the two-pass compositing itself — the second pass runs with
`DepthStencilState.DepthRead` and `BlendState.NonPremultiplied` over 41 319 overlapping quads, so
a difference in depth-write or draw order would redistribute which billboard is visible without
moving any of them — and, separately, to read back the shader's own computed `rightVector` for a
known vertex.

## `WEBGL2`

Not yet built or gated; that follows once the native frame is understood.
