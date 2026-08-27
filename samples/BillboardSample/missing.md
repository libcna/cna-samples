# BillboardSample — port notes

Upstream: `BillboardSample_4_0` (SAMPLE-039). Ported whole — the landscape model, the custom
compiled billboard effect, the two-pass opaque/fringe rendering, the wind animation and every key
binding. Nothing is missing, stubbed or simplified, and **no framework change was needed**: this
is the first sample in a while that CNA rendered correctly on the first run.

## Content

One source asset, `landscape.x`, built by `VegetationProcessor` from the sample's own pipeline
extension. It is the first `.x` model in this campaign, so `XImporter.dll` joins the stock
importers in `PipelineAssemblies`; CNA itself never sees the `.x` file, only the XNB the official
pipeline produces. The processor pulls in `Billboard.fx`, the terrain texture and the two
billboard textures, so five XNBs come out of one listed asset.

**This sample must be built HiDef.** The processor writes the whole landscape as one mesh part:

```
XNA Framework Reach profile supports a maximum of 65535 primitives per draw call, but this
ModelMeshPart contains 82668 triangles.
```

That is a content-build failure, and it has a second half at run time: the original refused its
own content with *"compiled for the HiDef profile, cannot be loaded into a Reach GraphicsDevice"*
until the `Microsoft.Xna.Framework.RuntimeProfile` resource embedded in the executable was set to
`Windows.v4.0.HiDef` (`scripts/Microsoft.Xna.Framework.RuntimeProfile.txt`). The profile is
enforced twice — once by `BuildContent`, once by the loader reading that resource — and both had
to be satisfied.

## The processor is not deterministic — the two engines must be handed the same XNB

`VegetationProcessor` scatters ~41 300 grass billboards and 77 trees over the terrain using an
unseeded random source. **Every content build produces a different landscape.** The XNBs are the
same size and load identically; only the contents differ.

This cost most of the investigation. The two engines were being fed XNBs from two different
pipeline runs, so they were rendering *different worlds*, and every measurement taken against
them was noise:

- 74.9 % whole-frame agreement, blamed in turn on wind, on the alpha test, on the depth test and
  on billboard sizing — each "explained" by a plausible mechanism that then failed to hold up;
- an apparent defect where XNA's trees were tall and slender and CNA's short and fat, which reads
  exactly like the shader's `squishFactor` going wrong. Both engines were in fact computing it
  correctly, from different `Random` values;
- a part-isolation diagnostic that reported 99.95 % agreement while the two frames plainly showed
  different objects, because `mesh.Effects` does not enumerate the parts in the same order in the
  two engines and an index hid a different part in each. Selecting by the part's own
  `BillboardWidth` instead is order-independent and gave the honest answer.

The tell was in the model, not the picture: the parts held 165 296 / 300 / 4 vertices in XNA and
165 276 / 308 / 16 in CNA — same total, different split. **After copying the original's own
`landscape.xnb` and `Billboard_0.xnb` into the port, the part table matched exactly**, which also
clears CNA's XNB model reader: given the same bytes it produces the same parts, offsets and
vertices.

The XNBs shipped with this port are therefore the ones the original executable runs, byte for
byte, and must stay that way.

**`scripts/build-original.sh` re-randomizes them.** It builds the content with `RebuildAll`, so
every run of it produces a fresh landscape — which happened once more after the comparison was
finished and silently replaced the original's content with a fourth world. The five pinned XNBs
are the ones in `samples/BillboardSample/Content/`; after any rebuild of the original they must be
copied over `xna4-build/bin/Content/`, `xna4-build/bin-diag/Content/` and
`xna4-build/Content-hidef/`. The script now says so in its header, and the four content hashes to
check are `landscape a86a7ca5`, `Billboard_0 1ff72c09`, `grass_0 bb3365a8`, `tree_0 6bc91a54`,
`cat_0 49a10bc6`.

With the same world and the wind left running, the two engines' start frames agree to 87.27 %;
the remaining gap is the wind phase, which is what the pinned comparison below removes.

## Comparison against the original

The wind is the only self-animating quantity, driven by `TotalGameTime`. Its phase at capture
depends on start-up latency, which is not the same under Wine as it is natively, so it is pinned
in both engines by a `CNA_WIND` hook (`scripts/compare-frozen.sh`, `cna-diag/README.md`). The
hook existed only on the XNA side for most of this investigation, which is why the numbers below
moved so much when it was added to both.

With the phase pinned, **the frames agree to 95.61 %** of pixels within 8 levels, and both
engines are now bit-exact between their own runs (XNA 100 %, CNA 100 %). Three phases were
captured rather than one, so the figure is not a lucky moment of the animation:
`CNA_WIND` 0 → 95.61 %, 1.5 → 95.58 %, 3 → 95.63 % (`evidence/frozen/`).

| Measurement | Value |
|---|---|
| pixels within 8 levels | **95.61 %** |
| mean absolute difference | 1.83 / 255 |
| median absolute difference | **0** |
| mean signed difference (CNA − XNA) | −0.06 / −0.10 / −0.19 per channel |
| differing pixels lying on an edge | **96.5 %** (edges are 20.7 % of the frame) |

The residue is the one-pixel silhouette of alpha-tested foliage, and it is distributed exactly as
that would be: the sky and the tree line are **0.00 %** different, the distant trees 1.26 %, and
the dense grass nearest the camera 12.4 %. Two candidate causes were tested and rejected rather
than assumed:

- **Not the alpha test.** Lowering `AlphaTestThreshold` in both engines makes agreement *worse*
  (0.50 → 84.99 %, 0.05 → 78.10 %), which is the opposite of a knife-edge threshold.
- **Not the wind.** With `WindAmount` forced to zero in both, agreement is 95.54 % — unchanged.
  So it is not `sin()` precision in the vertex shader either.

The animation *rate* matches: over the same 1.5 s the frame changes by 11.0 % of pixels in XNA
and 14.1 % in CNA, and the sizes, positions and lighting of the billboards are identical.

## What this sample exercises in CNA

- a `.x` model through the official pipeline, with a **custom `Effect` per mesh part**;
- **82 668 triangles in a single `ModelMeshPart`**, requiring the HiDef profile end to end;
- per-part effect parameters set from `mesh.Effects` between two draws of the same mesh;
- `clip()` in a compiled pixel shader — the alpha test that makes a billboard a cutout;
- two passes over the same geometry with `BlendState.Opaque`/`DepthStencilState.Default` and then
  `BlendState.NonPremultiplied`/`DepthStencilState.DepthRead`, sharing one depth buffer.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the landscape
renders against the clear, the **billboards are alpha-tested cutouts rather than opaque quads**,
the **wind animates with no input at all**, the camera keys move the view and R restores it.

The cutout check counts sky pixels appearing *below* the topmost vegetation pixel of their
column — pixels that exist only because `clip()` punched holes in the quads; opaque rectangles
would score about zero. It is calibrated against both engines' own native frames, which score 169
(XNA 4.0) and 197 (CNA OPENGLES3) on the same measurement.

Its first version scored an identical 47 076 on all five frames, which is what a degenerate
measurement looks like: the captured clip starts on a non-sky row, so the "vegetation edge" was
found at row 0 in every column and the count was really measuring the sky. A number that does not
move when the picture does is not a contract — it now ignores any leading non-sky border and
scores 178–970 across the five frames.

## Deviations

None in behavior. Three C++ shapes worth naming:

- `graphics.PreferredBackBufferWidth`-style setup and `mesh.Draw()` are unchanged, but CNA takes
  the render states by value through `setBlendStateProperty` / `setDepthStencilStateProperty` /
  `setRasterizerStateProperty` rather than by property assignment.
- `GraphicsDevice.SamplerStates[0] = SamplerState.LinearClamp` maps directly onto
  `getSamplerStatesProperty()[0] = …`; unlike `TextureCollection`, CNA's `SamplerStateCollection`
  exposes an assignable `operator[]`.
- `effect.Parameters["Name"].SetValue(v)` returns a pointer in CNA, so the calls chain through
  `->` rather than `.`.
