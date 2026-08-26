# ShadowMapping — port notes

Upstream: `ShadowMappingSample_4_0` (SAMPLE-038). Ported whole — both shadow-map passes, the
floating-point render target, the compiled two-technique effect and every key binding. Nothing
is missing, stubbed or simplified, and no framework change was needed.

## Content

Two models, both through the sample's own pipeline extension (`CustomEffectPipeline`, like
SAMPLE-032 and SAMPLE-034), built for all three targets in Debug. Each carries **two** processor
parameters that the content project sets and that the build is wrong without:

```xml
<Compile Include="dude.fbx">
  <Processor>CustomEffectModelProcessor</Processor>
  <ProcessorParameters_CustomEffect>DrawModel.fx</ProcessorParameters_CustomEffect>
  <ProcessorParameters_Scale>0.75</ProcessorParameters_Scale>
</Compile>
```

`DrawModel.fx` and all fourteen textures are pulled in by that processor chain rather than listed
directly; the build produces sixteen XNBs from two source assets.

## What this sample exercises in CNA

More than any earlier sample in this campaign, and all of it worked first try:

- a **`SurfaceFormat.Single` render target** with `DepthFormat.Depth24`, 2048×2048;
- a **compiled effect with two techniques**, switched per draw by name
  (`CreateShadowMap` / `DrawWithShadowMap`);
- that render target **bound back as an effect texture parameter** while the second pass reads it;
- `BoundingFrustum.GetCorners` and `BoundingBox.CreateFromPoints` driving the light's
  orthographic projection every frame;
- `SpriteBatch` drawing the float render target to the screen.

## Comparison against the original

The camera and the character's rotation both move by `time * k` per frame, so neither is
reproducible from a key press. `CNA_ROTATE` pins the rotation in both engines
(`scripts/compare-frozen.sh`, `cna-diag/README.md`); the camera needs no hook, since its start-up
position is the one the R key resets to.

**The scene matches. Two differences remain, and neither is a defect.**

### The shadow-map preview: (R,1,1,1) versus (R,0,0,1) — a decision for the project owner

Where the shadow map holds 1.0, the original's 128×128 preview reads **(255, 255, 255)** and
CNA's reads **(255, 0, 0)**. Direct3D 9 expands a one-channel float format to `(R, 1, 1, 1)`;
OpenGL expands `GL_R32F` to `(R, 0, 0, 1)`.

**CNA is FNA-faithful here.** `FNA3D_Driver_OpenGL.c:378` maps `SurfaceFormat.Single` to
`GL_R32F` with `GL_RED`, and applies no swizzle anywhere — FNA on OpenGL shows the same red
square. So this is XNA-on-D3D9 versus the whole GL family, not a CNA bug.

It *is* fixable — `GL_TEXTURE_SWIZZLE_G/B/A = GL_ONE` is exactly the D3D9 expansion rule and is
core in GL ES 3.0 and desktop GL 3.3. **But WebGL 2 does not expose texture swizzle at all**, so
the fix would make the native and web targets of this campaign disagree with each other. That
trade is the owner's to make, not this sample's, so nothing was changed and the measurement is
recorded here instead.

It does not affect the shadow itself: `DrawModel.fx` reads the map's `.r`, which is identical
either way.

### Far-field texture filtering

Excluding the preview square, agreement is ~92 % of pixels within 8 levels — lower than earlier
samples because this frame is mostly a strongly minified checkerboard floor rather than flat
background. The residue is filtering, and it is distributed exactly as filtering would be:

| Band | Mean absolute difference | Within 8 levels |
|---|---|---|
| far third | 4.63 / 255 | 87.45 % |
| middle third | 4.58 / 255 | 86.03 % |
| **near third** | **1.95 / 255** | **94.95 %** |

Near the camera the floor's pixels are **exactly equal** — sampled at (255,380) and (255,430),
XNA and CNA both read (186, 98, 110) and (208, 208, 208). The difference appears only where the
checkerboard is minified and the two implementations pick different mip weights.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the floor
and character render, **the shadow is actually cast** (the second pass darkens the floor), the
shadow map is previewed, rotating the character moves the shadow, and the camera keys move the
view. Walking two steps forward fills the frame with floor and leaves almost no sky — the gate
requires the clear only on the frames that look at it.

## Deviations

None in behavior. Three C++ shapes worth naming:

- `spriteBatch->Begin(0, ...)` becomes `SpriteSortMode::Deferred`, and CNA takes the sampler,
  depth and rasterizer states by pointer, so the original's two nulls stay null.
- `GraphicsDevice.Textures[0] = null` becomes `getTexturesProperty()(0, nullptr)`; CNA's
  `TextureCollection` exposes assignment as a call operator, `operator[]` being read-only.
- `effect.Techniques[name]` returns a pointer in CNA, so it is passed to
  `setCurrentTechniqueProperty` directly rather than address-taken.
