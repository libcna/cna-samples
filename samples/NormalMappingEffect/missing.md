# NormalMappingEffect — port notes

Upstream: `NormalMappingSample_4_0` (SAMPLE-034). Ported whole. Nothing is missing, stubbed or
simplified: every field, every key binding and both effect-parameter groups are the original's.

## Content

Built by the official XNA 4.0 Content Pipeline from the **unmodified** original content project,
for all three of the solution's targets (Windows/Reach, Windows/HiDef, Xbox 360/HiDef). The
sample ships its own pipeline extension — `NormalMappingEffectPipeline` — whose
`NormalMappingModelProcessor` chain generates tangents and binormals, points each material at
`NormalMapping.fx`, and converts every normal map to `NormalizedByte4`. That assembly is compiled
first and handed to `BuildContent` in `PipelineAssemblies`; `scripts/build-original.sh` does all
of it.

**`BuildConfiguration` had to be measured, not assumed.** This sample builds in **Debug** and
fails in Release: the optimizer folds `pow(rDotV, SpecularPower)`, whose base can be zero,
through `log(0)` into an infinity literal and `fxc` refuses it with `error X4579`. That is the
**opposite** of SAMPLE-032 and SAMPLE-033, where Debug overran `ps_2_0`'s 64-instruction limit
and Release was the configuration that worked. Neither is a default — check the shipped shader
against the configuration before doubting the toolchain.

Seven of the eight XNBs are **byte-identical** to a build the project owner made on Windows 7
with Visual Studio 2010 and the real XNA Game Studio 4.0 — the model and all four textures, which
is the whole custom-processor chain. The eighth, the compiled effect, differs by exactly 16
bytes: the source path a Debug shader build embeds
(`C:\Users\…\NormalMappingSample_4_0\…\NormalMapping.fx`, 114 characters, against this build's
98-character `Z:\rv\tmp\…`). The shader bytecode itself is identical.

## Framework defect found and fixed in `cnanext`

**Compiled-effect fragment shaders were translated at GLSL ES's `mediump` default**
(`plans/plan_fx.md` FX-121). A Direct3D 9 shader computes in full 32-bit float unless an
instruction opts into partial precision; `mediump` guarantees only fp16 range. This sample's
pixel shader normalizes an interpolated **world-space** light vector, so the first `dot(v, v)`
reaches ~10^6 on the sample's own scale (light radius 800, camera distance 1500), overflows,
and `normalize` returns the zero vector — taking the diffuse **and** the specular term to
exactly zero.

What that looked like is the part worth keeping: the model still drew, the camera still
responded, `AmbientLightColor` still worked, and the frame read as a dim but plausible render.
It was `AmbientLightColor * diffuseTexture` and nothing else, and a full rotation of the light
changed **not one pixel**. Every layer above the shader measured correct first — the parameter
reached the runtime boundary with the right value, reached the vertex register file, and read
back from GL as `vs_uniforms_vec4[12]` correctly; writing that register by hand still changed
nothing, which is what moved the search into the fragment shader itself.

Fixed at the source, as a second MojoShader patch
(`cmake/patches/mojoshader-6333f74-fragment-precision.patch`) rather than by merging an unrelated
fix into the existing parser-robustness one; the apply script now takes a list.

## Comparison against the original

`scripts/compare-frozen.sh` pins the orbiting light — the sample's only animated quantity, since
it starts at the camera the R key resets to — in both engines and captures the same four angles.

| Light angle | Exactly equal | Within 8 levels | Mean absolute difference |
|-------------|---------------|-----------------|--------------------------|
| 0.0 rad     | 81.9 %        | 95.50 %         | 1.28 / 255               |
| 1.5 rad     | 81.6 %        | 95.42 %         | 1.34 / 255               |
| 3.0 rad     | 83.5 %        | 98.17 %         | 0.76 / 255               |
| 4.5 rad     | 83.6 %        | 98.07 %         | 0.78 / 255               |

Frame mean brightness agrees with the original to **0.02 of a level out of 255** at every angle,
so the residue carries no bias: it is a fine speckle on the model's own surface, where a normal
map texel lands on a different side of a quantization boundary under two different texture
filters. The silhouette, the specular highlights and their positions all match.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the scene
renders, the light animates, Space stops it, X zooms, R restores the camera, the document title
is the original's `Normal Mapping`, and — as the regression pin for FX-121 — that peak model
luminance clears 80: the defect capped it at 31 against a lit frame's 147.

## Deviations

None in behavior. The three C++ shapes the port uses are the campaign's usual ones: `std::optional<Model>`
for a reference-type field that is null until `LoadContent`, `getXProperty()`/`setXProperty()` for
C# properties, and `Game::Update`/`Game::Draw` for `base.Update`/`base.Draw`.
