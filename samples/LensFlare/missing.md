# LensFlare — port notes

Upstream: `LensFlareSample_4_0` (SAMPLE-041). Re-ported from scratch. The whole sample is here —
the terrain, the `DrawableGameComponent`, the occlusion query, the glow and all ten flare sprites,
and every key binding.

## What the previous port got wrong

An earlier pass (2026-07-09) shipped a header-only port built on a hand-converted
`terrain.model.json`, added an F1 help overlay the original does not have, and recorded three
findings in this file. Two of them do not survive contact with the official content pipeline, and
the third was fixed in `cnanext` since:

| Old claim | What is actually true |
|---|---|
| "near-plane clipping renders the terrain as a thin line" — filed as a shared framework bug against a second asset | An artifact of `tools/fbx_ascii2model.py`. Through the real pipeline the terrain renders correctly: right shape, right scale, lit, fogged. |
| "`ground.png` is unused; the `.model.json` schema has no texture field" | The FBX material pulls `ground.png` in by itself. The content build emits **six** XNBs from the five listed assets, and the terrain is textured. |
| "EasyGL never applies `BlendState.ColorWriteChannels` (no `glColorMask`)" — an opaque quad over the sun | Fixed in EasyGL since. The occlusion query polygon is invisible, as it must be; the `WEBGL2` gate now pins that with a measurement. |

The lesson is the one this campaign keeps relearning: a bypassed content pipeline manufactures
framework bugs that are not there. All three findings came from the bypass or from a version of
the renderer two months old.

## Content

Five listed assets, all through stock importers and processors. `terrain.fbx` carries a processor
**parameter** the content project sets and the build is wrong without:

```xml
<Compile Include="terrain.fbx">
  <Processor>ModelProcessor</Processor>
  <ProcessorParameters_RotationX>-90</ProcessorParameters_RotationX>
</Compile>
```

That is the Z-up to Y-up correction the previous port tried to reproduce by hand in a converter.
`ground.png` is not listed at all — the FBX material references it — so six XNBs come out of five
assets. The sample declares HiDef, and the executable's embedded `RuntimeProfile` resource says
so; the Reach content leg also builds here, since nothing in this sample needs Shader Model 3.

## The real finding: `OcclusionQuery.PixelCount` is a boolean on OpenGL ES

XNA's `PixelCount` is a **tally of the fragments that passed**, which is what Direct3D 9 returns.
`LensFlareComponent.UpdateOcclusion` divides it by the query rectangle's area to get a coverage
ratio, and fades the whole effect by that ratio.

**OpenGL ES 3.0 and WebGL 2 have no query target that produces a tally.** Their core occlusion
target is `GL_ANY_SAMPLES_PASSED`, whose result is 0 or 1 however much geometry was covered. So
`occlusionAlpha` settles at `1/10000` and the glow and flares are drawn at an alpha of 0.0001 —
invisible. The terrain, the fog, the lighting and the camera are all correct; only the effect the
sample is named after is missing.

Measured rather than inferred, in this order:

1. `occlusionAlpha` reads **0.0001** every frame — exactly `1/queryArea`.
2. Disabling the depth test entirely does not change it, so it is not the depth comparison.
3. Drawing the query quad **visibly** (its `ColorWriteChannels` lifted) shows a 100x100 red
   rectangle covering **9788** pixels, centred exactly on the projected sun. The geometry, the
   orthographic projection and the rasterization are all right — the query is the only thing
   answering wrongly.
4. Feeding the component the ratio those 9788 pixels represent, and changing nothing else, makes
   the CNA frame agree with the original to **99.74 %** of pixels within 8 levels, mean absolute
   difference **0.58/255**, median **0**, and no signed bias in any channel. The glow and all ten
   flares land in the same places, at the same sizes, in the same colours.

So the port is right and the renderer cannot answer the question. `cnanext` now does the best the
API allows:

- `EasyGLOcclusionQueryRenderer` asks the driver for `GL_SAMPLES_PASSED` on its first query and
  keeps it if GL accepts the enum, falling back to the boolean otherwise. The asymmetry is real
  and driver-side: **the same Mesa 25.0.7** accepts it under a desktop OpenGL 4.5 context and
  reports 4096 fragments for a fully covered 64x64 viewport, and refuses it under the OpenGL ES
  3.2 context the `OPENGLES3` profile creates. That measurement lives in
  `cnanext/spikes/occlusion-count-spike/`, so the precise arm is proven rather than assumed.
- `OcclusionQuery::isPixelCountPreciseEXT()` (CNAEXT) lets a game ask which of the two it is
  holding before dividing by an area. It forwards a defaulted
  `IOcclusionQueryRenderer::PixelCountIsPreciseEXT()`, so no other backend had to change.
- `docs/occlusionquery-support.md` no longer calls EasyGL "fully correct". Both existing EasyGL
  occlusion tests assert `PixelCount() > 0` and `PixelCount() <= 0` — **a boolean passes both**,
  which is why this went unnoticed. The new
  `OcclusionQueryPixelCountPrecisionTests.cpp` requires the value and the precision claim to agree,
  and was confirmed to fail when the claim is falsified.

This is a genuine limit of OpenGL ES rather than a CNA shortcut, and FNA has it too: its OpenGL
backend uses the same boolean target on ES. Nothing in the sample was changed to work around it.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the terrain
renders against the clear, each of the four camera keys moves the view, `R` restores the start
view (both by hash and by sky fraction), and that the **occlusion query polygon stays invisible** —
the regression pin for `ColorWriteChannels`.

That last check counts **near-black** pixels rather than bright ones, deliberately: an opaque
query quad would add about 10000 of them, while terrain shadows produce 345-662 whether the glow
is drawn or not. A bright-pixel check would have passed today and then failed the day this
renderer gains a precise occlusion count and starts drawing the glow — pinning the limitation in
place instead of the contract.

## Deviations

None in behavior. The previous port's F1 help overlay is **gone**: the original has no
`SpriteBatch` in its game class, no font and no on-screen text, and this campaign ports what the
original does. Four C++ shapes worth naming:

- `Components.Add(lensFlare)` takes a borrowed pointer, so the game owns the component through a
  `std::unique_ptr` and hands over `.get()`.
- `static readonly BlendState ColorWriteDisable = new BlendState { ... }` becomes a
  function-local static configured once, since CNA's `BlendState` is set through
  `setColorWriteChannelsProperty`.
- XNA's `ContentManager` returns the same `Texture2D` for a repeated asset name, and four of the
  ten flares share `flare1`. CNA's `Load<T>` returns by value, so the port keeps an explicit
  name-keyed cache and the flares point into it.
- `foreach (BasicEffect effect in mesh.Effects)` becomes an explicit `static_cast<BasicEffect*>`,
  as CNA's `ModelEffectCollection` yields `Effect*`.
