# NormalMappingEffect — port notes

Upstream: `NormalMappingSample_4_0` (SAMPLE-034). Ported whole. Nothing is missing, stubbed or
simplified: every field, every key binding and both effect-parameter groups are the original's.

## Current requalification — 2026-09-20

The complete physical upstream directory is byte-identical to the retained
`/rv/tmp/samples/SAMPLE-034-NormalMappingSample_4_0/xna4-original/` snapshot.
Its unchanged Windows executable and official Debug content were rebuilt with
the sample's own `NormalMappingEffectPipeline` for Windows/Reach, Windows/HiDef
and Xbox 360/HiDef. The original runs under WineD3D on isolated Xvfb and exits
on Escape. All seven non-effect checked-in XNBs are byte-identical to the new
Windows output. Both `NormalMapping_0.xnb` versions are genuine 6764-byte
pipeline products: the fresh output varies by only 17 bytes of name-adjacent
padding at offsets 299–840, while bytes 900–6764, including the shader code,
match exactly. The checked-in official effect is retained. The historical
Windows 7 source-path difference described below is a separate comparison.

Fresh Release `OPENGLES3` and non-threaded `WEBGL2` builds use active
`libcna/cna` (`95b7e14a2`) and `libcna/sharp-runtime` (`cb8fd7f8`), compiled
effects and no more than four concurrent compiler jobs. The stripped retained
native product now has a `RUNPATH` into active `libcna/cna` instead of the
missing old `cnanext` checkout. Ordinary original and native captures exercise
moving/stopped light, zoom, reset and clean Escape exit. Isolated diagnostic
copies freeze only the light, never the shipped source. Whole-frame 800×480
XNA/native comparisons at 0.0, 1.5, 3.0 and 4.5 radians find 383948, 383938,
383960 and 383958 of 384000 pixels within eight levels, respectively
(99.984–99.990%). Channel MAE is 0.017–0.032/255 with no frame pixels excluded.
The current agreement is better than the older measurement below; the exact
cause of that improvement is not isolated.

Work, retained and byte-identical local-gallery WEBGL2 bundles pass the real
Google Chrome gate: WebGL2 800×480, visibly lit model, light motion and
Space pause/resume, X zoom, ArrowRight rotation, R reset, required HTTP 200s
and no relevant driver/runtime/promise errors. The original
`NormalMappingEffectSample.png` and `Game.ico` were restored outside `Content`.
The local gallery has 33 cards on 12/12/9 pages; no publication or prune was
requested. No game-code workaround, stub, intentional behavioral deviation,
new CNA fix or sharp-runtime change was needed. The current CNA already
contains the general FX-121 precision fix described below.

Reproduction commands, SHA-256 hashes, exact artifact paths, captured frames
and browser reports are in
`/rv/tmp/samples/SAMPLE-034-NormalMappingSample_4_0/evidence/requal-20260920/README.md`.
The current build trees are `work-native-opengles3-20260920/` and
`work-web-webgl2-20260920/` under that artifact root; canonical products are
`xna4-build/bin/NormalMappingEffect.exe`,
`cna-native-opengles3/samples/NormalMappingEffect/NormalMappingEffect_cna_samples`
and `cna-web-webgl2/samples/NormalMappingEffect/NormalMappingEffect_cna_samples.{html,js,wasm,data}`.

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
