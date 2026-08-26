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

**The scene matches.** One framework defect was found and fixed; one difference remains and is
not one.

### Framework defect found and fixed in `cnanext`: one- and two-channel formats did not expand

Direct3D 9 expands a texture's missing channels when a shader samples it — a one-channel format
arrives as `(R, 1, 1, 1)`, a two-channel one as `(R, G, 1, 1)`. OpenGL expands the same storage
to `(R, 0, 0, 1)` and `(R, G, 0, 1)`. So where the shadow map holds 1.0, the original's 128×128
preview read **(255, 255, 255)** and CNA's read **(255, 0, 0)** — a red square instead of a white
one, on a sample whose whole subject is that map.

CNA was FNA-faithful here (`FNA3D_Driver_OpenGL.c:378` maps `SurfaceFormat.Single` to
`GL_R32F`/`GL_RED` and swizzles nowhere), but this campaign's oracle is XNA, so CNA now matches
XNA.

**Why the fix is not `GL_TEXTURE_SWIZZLE`.** That parameter is exactly D3D9's rule and is core in
GL ES 3.0 and desktop GL 3.3, and it would have covered every sampling path at once. **WebGL 2
does not have it** — measured in a real browser rather than assumed: the constant is absent from
the context object and `texParameteri(TEXTURE_SWIZZLE_G, ONE)` raises `INVALID_ENUM`. Since this
campaign ships a native and a `WEBGL2` build of every sample, a swizzle-based fix would have made
the two disagree. The expansion is applied in the sprite fragment shader instead, from the bound
texture's own `SurfaceFormat`, where every profile does it identically.

Two things that fix cost, both worth keeping:

- **A uniform location belongs to the program it came from.** Caching the two locations from the
  sprite program broke `WeightedBlendedTransparencyTest`: a SpriteBatch drawn with a custom
  `ShaderEffect` runs that effect's OWN program, and handing it a foreign location is
  `GL_INVALID_OPERATION`, not a silent no-op — it surfaced as *"native GL errors were pending
  before MRT setup"* two passes later. The locations are now looked up per flush on the program
  actually in use, which also gives the right semantics: a custom effect samples the texture
  itself and has no such uniform, so nothing is written and its own sampling is left alone.
- **The expansion had to be proven identity for everything else.** SAMPLE-036's ten frames are
  **byte-identical** across the change, and a second test asserts a `Color` texture drawn the
  same way is untouched — a shader that expanded unconditionally would turn its green pixel
  white.

Both tests are in `modules/graphics/tests/.../SingleChannelExpansionTests.cpp`, and the first was
confirmed to fail with the fix reverted.

The boundary this leaves: a **custom** effect that samples a one- or two-channel texture still
sees GL's expansion, since CNA does not author that shader. `DrawModel.fx` reads the map's `.r`,
which is identical either way, so this sample is unaffected by it.

### Far-field texture filtering

Whole-frame agreement is ~92 % of pixels within 8 levels — lower than earlier samples because
this frame is mostly a strongly minified checkerboard floor rather than flat background. The
residue is filtering, and it is distributed exactly as filtering would be:

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
shadow map is previewed, **that preview is white and not red** (the channel-expansion regression
pin), rotating the character moves the shadow, and the camera keys move the view. Walking two
steps forward fills the frame with floor and leaves almost no sky — the gate requires the clear
only on the frames that look at it.

The white-preview check exists because of a mistake worth recording: the channel expansion was
first verified natively only, and the WEBGL2 bundle shipped from a build made *before* the fix,
so the square was still red in the browser while the native frame was already white. The gate
checked that the preview was *present*, not what colour it was, so nothing caught it. **A
framework fix is not verified until both targets are rebuilt and re-captured**, and this sample's
gate now measures the pixel that distinguishes them.

That also settles the open question the earlier version of this file left: WebGL 2 has no
`GL_TEXTURE_SWIZZLE`, but the shader-side expansion reaches it fine — measured, not assumed.

## Deviations

None in behavior. Three C++ shapes worth naming:

- `spriteBatch->Begin(0, ...)` becomes `SpriteSortMode::Deferred`, and CNA takes the sampler,
  depth and rasterizer states by pointer, so the original's two nulls stay null.
- `GraphicsDevice.Textures[0] = null` becomes `getTexturesProperty()(0, nullptr)`; CNA's
  `TextureCollection` exposes assignment as a call operator, `operator[]` being read-only.
- `effect.Techniques[name]` returns a pointer in CNA, so it is passed to
  `setCurrentTechniqueProperty` directly rather than address-taken.
