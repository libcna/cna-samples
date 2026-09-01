# SAMPLE-130 — `MaterialsAndLights_ARCHIVE_2_0` audit and owner decision

## Status

Fresh audit complete enough to require an older-XNA product decision under
`SAMPLES-DEC-005`. This is a complete, independently runnable XNA 2.0 Shader Series game whose
central lesson depends on `EffectPool` and HLSL `shared` parameter propagation. No sample-local
parameter fan-out, profile rewrite, reduced scene or invented XNA 4 alias was introduced without
the owner's scope ruling.

## Complete product inventory

The upstream delivery contains **30 files / 6,453,876 bytes**:

- separate Windows and Xbox 360 XNA 2.0 solutions/projects;
- five C# files / 1,464 lines (`MaterialsAndLights`, `Material`, `PointLight`, the reusable
  `SampleArcBallCamera` and assembly metadata);
- three effects plus one include file / 376 HLSL lines;
- five FBX meshes, six authored textures and one Tahoma SpriteFont declaration;
- an HTML design/control document, icon, thumbnail and Microsoft Permissive License.

The 15 runtime content items are the five meshes, six textures, SpriteFont and three effects.
There is no shipped executable, XNB or raw compiled-effect blob.

## Audited behavior

The 853x480 game draws eight material/mesh combinations around a ring, a large tiled floor and
visible low-poly spheres for active lights. Eight materially different presets combine solid and
textured diffuse colour, specular power/intensity, UV repetition and optional diffuse/specular
maps. Point lights carry independent position, colour, range and exponential falloff; the material
shader computes Lambertian diffuse plus Phong specular lighting.

The game selects an eight-light Shader Model 3 path when available and a two-light Shader Model 2
fallback otherwise. It preserves these documented controls:

| Action | Keyboard | Gamepad |
|---|---|---|
| orbit/zoom camera | W/A/S/D | right stick/D-pad |
| rotate meshes | arrow keys | left stick/D-pad |
| rotate lights | Page Up/Down | triggers |
| add/remove lights | numpad +/- | shoulder buttons |
| cycle material assignment | Tab | X |
| regenerate light properties | Space | Y |
| exit | Escape/Alt+F4 | Back |

The complete 492-line camera is byte-identical to the XNA 4 VertexLighting, PerPixelLighting and
TexturesAndColors cameras after only namespace normalization, and to the future XNA 2
MultipassLighting camera on the same terms. Camera math and controls are therefore not a missing
runtime capability.

## The defining XNA 2 contract is absent from XNA 4

This sample does not merely happen to mention an old API. Its documentation and numbered examples
teach the following design:

1. load one base material effect;
2. clone it into eight material effects plus one floor effect;
3. keep material-specific values in each clone;
4. update seven HLSL `shared` declarations (`view`, `projection`, `cameraPosition`,
   `ambientLightColor`, `numLights` and the two `lights` arrays) only through the base effect;
5. rely on the common `EffectPool` to propagate those values to every clone before drawing.

The live XNA 4 reference assembly has `Effect.Clone()` with no device argument and no public
`Microsoft.Xna.Framework.Graphics.EffectPool` type. Authoritative FNA has no `EffectPool` API.
FNA's clone constructor calls `FNA3D_CloneEffect`; live CNA likewise clones the native runtime and
then copies mutable values into an independent parameter collection. The pinned MojoShader parser
discards the legacy parameter-flags word and `MOJOSHADER_cloneEffect` deep-copies each parameter
value. CNA's real regression contract explicitly verifies that mutating a clone does not reach its
source.

Consequently the unchanged algorithm would leave all nine cloned effects with stale/default
view, projection, camera, ambient, light-count and light-array values. Making the sample set each
of these values on every clone would produce a picture, but it would replace the core pool/shared
lesson and its state-setting model with sample-local fan-out. Adding `EffectPool` to the normal
Microsoft XNA namespace would instead add a removed XNA 2 API to CNA's XNA 4 public surface. Neither
choice is an autonomous faithful-port fix.

## Measured XNA 4 migration boundary

The five unchanged C# files were compiled diagnostically against the official XNA 4 assemblies.
They reach exactly **35** old-API errors, covering:

- `Effect.Clone(GraphicsDevice)`, `Effect.Begin/End`, `EffectPass.Begin/End`, `CommitChanges` and
  `SaveStateMode`;
- the XNA 2 vertex-stream/model-buffer binding surface and old `DrawIndexedPrimitives` arguments;
- `MinimumVertexShaderProfile`, `MinimumPixelShaderProfile`, `ShaderProfile` and
  `GraphicsDeviceCapabilities.PixelShaderVersion`;
- mutable `GraphicsDevice.RenderState` depth flags.

Most have ordinary XNA 4 mechanical mappings: `EffectPass.Apply`, mesh-part buffers, modern draw
arguments, `GraphicsProfile` and immutable `DepthStencilState`. Those mappings do not recover the
removed pool/shared behavior.

The unchanged content was also measured through Microsoft's XNA 4 pipeline:

- an optimized Windows/HiDef Release build produces **14/15** XNBs: all five models, all six
  textures, the SpriteFont and both material effects;
- `MaterialShader20.fx` compiles for Reach and HiDef; `MaterialShader30.fx` correctly requires
  HiDef because it uses Shader Model 3;
- a Debug pipeline build of the old SM2 source is additionally rejected at 132 arithmetic slots
  versus the ps_2_0 limit of 64, while the optimized Release build succeeds;
- the exact `PointLightMesh.fx` is rejected for both Reach and HiDef because XNA 4 no longer
  supports its `ps_1_1` profile.

Changing the point-light effect's `vs_1_1`/`ps_1_1` declarations to 2.0 would be a small,
behavior-preserving source migration—the shader only transforms a position and returns a uniform
colour—but it is still an XNA 2-to-XNA 4 edit to be made only after the product-scope choice.

## Current CNA evidence

The two exact material shaders compiled by the official processor parse in live CNA as 18
parameters, one technique, one pass and seven objects apiece. On a real offscreen OpenGL ES 3
context, MojoShader successfully compiles and binds the SM2 effect (10 vertex uniforms, 19 pixel
uniforms, one sampler) and the SM3 effect (10 vertex uniforms, 40 pixel uniforms, two samplers).
Thus the material shader language, reflected arrays/structures and GLES3 compiled-effect route are
not the blocker.

The 14 successful XNBs remain valid runtime content. CNA's native content compiler converts the
five models, six textures and SpriteFont to 12 validated CNBs; it truthfully leaves the two
`EffectReader` XNBs as runtime-only rather than inventing a CNB effect encoding. The normal CNA XNB
EffectReader is implemented and covered.

At CNA HEAD `7712534d3d22`, 54 focused OPENGLES3 Effect/EasyGL tests run: 53 pass and one unrelated
3D-volume-sampler contract is honestly skipped. The set includes clone independence, array and
structure reflection, state/sampler application, effect switching and real compiled draws. A
further 17/17 focused EffectReader, Model, Texture2D and SpriteFont XNB/content tests pass. No CNA or
Sharp Runtime source change was needed for this audit.

No native or WEBGL2 game is claimed: without an approved XNA 2 compatibility or modernization
contract, either build would validate newly invented behavior rather than the delivered product.

## Authentic reference boundary

The retained Wine compiler/pipeline and offline Win7 installation contain XNA Game Studio 4.0.
The archive requires XNA Game Studio 2.0 and ships no compiled product. No locally available XNA 2
toolchain or authentic executable/content output was found, so this audit does **not** claim an
original runtime capture. The supplied HTML is authoritative for behavior and controls, not a
substitute runtime result.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-130-MaterialsAndLights_ARCHIVE_2_0/`.

- `xna2-original/` is the complete byte-for-byte upstream snapshot;
- `evidence/file-inventory.txt`, `sha256sum.txt`, `image-metadata.tsv`, `model-metadata.tsv`,
  `csharp-lines.txt`, `hlsl-lines.txt`, `documentation.txt` and the empty `snapshot-diff.txt`
  cover every source input;
- `evidence/sample-camera-equivalence.tsv` proves the four namespace-only camera matches;
- `evidence/unchanged-source-vs-xna4.log` retains all 35 official compiler diagnostics;
- `evidence/xna4-effect-compile-status.tsv` and the six per-profile logs retain exact shader
  acceptance/rejection results;
- `evidence/xna4-content-debug.log`, `xna4-content-release.log` and
  `xna4-diagnostic/Content/` retain the Debug boundary and 14 Release XNBs;
- `evidence/xna4-effectpool-api-boundary.txt`, `live-clone-semantics.txt` and
  `legacy-api-and-effect-pool-scan.txt` retain the live API/source evidence;
- `evidence/cna-exact-effect-parse.log`, `cna-exact-effect-gl-bind.log`,
  `cna-content-transcode.log`, `cna-effect-tests.log` and `cna-content-tests.log` retain all CNA
  parser, real-GLES3 and focused regression results;
- `scripts/qualify.sh` reproduces the complete offline audit. Its only parallel converter step is
  explicitly capped at eight workers.

## Owner decision required

Choose one:

1. authorize a distinct legacy-XNA compatibility product, including an authentic XNA 2 reference
   route and deliberately `CNAEXT`-scoped `EffectPool`/shared semantics without contaminating the
   normal XNA 4 API;
2. authorize an explicit XNA 4 modernization: use the measured mechanical graphics mappings,
   profile-upgrade only the point-light colour effect, and replace pool propagation with a clearly
   documented per-material scene-parameter contract before full OPENGLES3/WEBGL2 comparison;
3. accept an evidence-backed historical-game archive/non-port boundary while retaining its full
   source, content and educational documentation.

Until that ruling, silently copying shared values to nine clones, dropping a shader path, adding a
reduced stock-lighting scene or calling an XNA 4 rewrite the authentic XNA 2 product would violate
the campaign's fidelity rules.
