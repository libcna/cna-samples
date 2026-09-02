# SAMPLE-134 — `MultipassLighting_ARCHIVE_2_0` audit and owner decision

## Status

Fresh audit complete enough to require an older-XNA product decision under `SAMPLES-DEC-005`.
This is a complete, independently runnable XNA 2 Shader Series application whose scene depends on
the removed `EffectPool`/HLSL `shared` contract. No per-clone parameter fan-out, profile rewrite,
reduced lighting demonstration or invented XNA4 alias was introduced.

## Complete product inventory

The upstream delivery contains 30 files / 6,451,155 bytes:

- separate Windows and Xbox 360 XNA 2 solutions/projects;
- five C# files / 1,640 lines: the main game, `Material`, `PointLight`, the reusable
  `SampleArcBallCamera` and assembly metadata;
- three effects plus one shared include / 330 HLSL lines;
- five FBX meshes, six textures and one Tahoma SpriteFont declaration;
- an HTML design/control document, icon, thumbnail and Microsoft Permissive License.

The 15 runtime content items are five meshes, six textures, one SpriteFont and three effects. No
compiled executable, XNB or effect binary is shipped.

Twelve source assets (all meshes, textures and the font declaration) are byte-identical to
SAMPLE-130, and the complete 492-line camera becomes byte-identical to SAMPLE-130 plus three XNA4
lighting samples after namespace normalization. This does not make the product a duplicate:
`MultipassLighting`, `Material`, `PointLight`, the material shaders, include and marker shader all
contain distinct multipass behavior. The documentation names it Shader Series 5 and explicitly
builds on the earlier materials-and-lights lesson.

## Audited behavior

The 853×480 game arranges eight mesh/material combinations around a ring, draws a large tiled floor
and renders a small coloured low-poly sphere at every active point light. Eight material presets
combine solid or textured diffuse colour, optional specular maps, specular power/intensity and UV
repetition. Lights have independent colour, position, range and exponential falloff and move on
two rotating axes.

The scene begins with 30 lights and permits 1–300. On Shader Model 3, the material shader owns a
twelve-element light array, starts with eight lights per draw and allows 1–12. The Shader Model 2
fallback processes exactly one light per draw. Every geometry batch first draws one opaque ambient
pass with depth writes enabled. Subsequent point-light passes disable depth writes, use
`LessEqual`, and add source and destination colours with `One`/`One`; this preserves occlusion while
summing arbitrarily many light contributions. The floor repeats the same algorithm.

| Action | Keyboard | Gamepad |
|---|---|---|
| orbit camera | W/A/S/D | right stick |
| zoom camera | Z/X | A/B |
| rotate meshes | Arrow keys | left stick |
| rotate lights | Page Up/Down | triggers |
| add/remove active lights | Numpad +/− | right/left shoulder |
| increase lights per draw | P | D-pad Up |
| cycle material assignments | Tab | X |
| regenerate random light properties | Space | Y |
| exit | Escape/Alt+F4 | Back |

There is no audio, networking, persistence or second runnable product.

## Defining XNA2 EffectPool dependency

The code loads one base lighting effect and clones it into eight material effects plus one floor
effect. Each clone retains independent material textures/colour/specular values and its own light
array. Five declarations in `Includes.inc` are instead marked `shared`: `view`, `projection`,
`cameraPosition`, `ambientLightColor` and `numLightsPerPass`.

The game updates those five values only through the base effect. That base is never used for a
geometry draw; XNA2's common EffectPool propagates the shared values to all nine clones. This is
observable, required logic rather than an unused language modifier: without it, clones keep stale
projection/view/camera/ambient/count data and cannot reproduce the scene.

The retained local XNA2 documentation identifies public `EffectPool` as the resource-sharing class
and `Effect.EffectPool` as the pool of shared parameters. The live XNA4 assembly has only
`Effect.Clone()` and no public `Microsoft.Xna.Framework.Graphics.EffectPool`; authoritative FNA has
no such XNA4 API. Live CNA follows XNA4/FNA: its documented clone has an independent parameter and
technique graph, the renderer runtime is cloned, and tests require mutations not to cross between
source and clone. MojoShader also deep-copies parameter values and does not retain the legacy flags
as an XNA4 pool contract.

Changing the sample to set five parameters on nine clones every frame could produce a picture, but
it would silently replace the delivered state-sharing algorithm. Adding an unmarked XNA2
`EffectPool` to CNA's normal Microsoft XNA 4 namespace would be the opposite error. Either a
deliberately isolated legacy compatibility route or an explicit modernization needs the owner's
scope decision.

## Measured XNA4 migration boundary

The five unchanged C# sources compiled diagnostically against official XNA4 assemblies and reached
46 old-API errors. They cover `Clone(GraphicsDevice)`, old Effect/Pass Begin/End and
`CommitChanges`, `SaveStateMode`, mutable `RenderState`, the old vertex-stream/model-buffer and
indexed-draw surface, minimum shader profiles and `GraphicsDeviceCapabilities`. These have ordinary
XNA4 mechanical mappings, but none restores shared-pool propagation.

Every effect was also compiled exactly and independently through Microsoft's XNA4 processor:

- `MaterialShader20` succeeds for Reach and HiDef;
- `MaterialShader30` succeeds for HiDef and is truthfully rejected by Reach for Shader Model 3;
- exact `PointLightMesh` is rejected for both profiles solely because XNA4 no longer accepts its
  `ps_1_1` declaration.

An optimized Windows/HiDef Release pipeline build therefore produces 14/15 exact XNBs: every model,
texture, font and both material shaders. The unoptimized Debug build additionally stops at 73
arithmetic slots versus the `ps_2_0` limit of 64, while the exact same shader succeeds optimized in
Release. Upgrading the marker shader's `vs_1_1`/`ps_1_1` declarations to 2.0 is a small
behavior-preserving migration—it only transforms position and returns a uniform colour—but it is
still an XNA2 source change to perform only after choosing the target product.

## Current CNA evidence

The exact compiled SM2 and SM3 material effects each expose 18 parameters, one technique, two
passes and nine objects. MojoShader parses their twelve shader objects / 28,988 bytes of Shader
Model bytecode. On a real offscreen OpenGL ES 3 context, its GLES3 adapter compiles and binds both
ambient and light passes, including the SM3 light-array path with 55 pixel uniforms. Thus the
compiled-effect, array/structure reflection, sampler and GLES3 shader paths are not the blocker.

Live `cna-content` converts the five models, six textures and SpriteFont into twelve validated CNBs.
It truthfully leaves the two EffectReader XNBs for runtime loading rather than inventing a CNB
effect encoding. At CNA HEAD `5347b52eae13`, 54 focused OPENGLES3 Effect/EasyGL tests run: 53 pass
and the established unrelated 3D-volume-sampler case skips. A further 17/17 EffectReader,
Model/Texture2D/SpriteFont and XNB/CNB tests pass. No CNA or Sharp Runtime source change was needed.

No native or WEBGL2 game is claimed: before an approved legacy or modernization contract, such a
build would validate agent-invented sharing behavior rather than this delivered XNA2 product.

## Authentic reference boundary

The retained Wine compiler/pipeline and offline Win7 installation contain XNA Game Studio 4.0.
This archive requires XNA Game Studio 2.0 and ships no compiled product. No local authentic XNA2
runtime/content toolchain was found, so the audit does not claim an original execution or visual
parity. The complete HTML behavior/control document and thumbnail are retained as source evidence,
not substituted for a runtime capture.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-134-MultipassLighting_ARCHIVE_2_0/`.

- `xna2-original/` is the complete byte-for-byte upstream snapshot;
- `evidence/file-inventory.txt`, `sha256sum.txt`, metadata/line inventories,
  `documentation.txt` and the empty `snapshot-diff.txt` cover every input;
- `evidence/sample130-shared-assets.tsv`, `sample130-tree-diff.txt` and
  `sample-camera-equivalence.tsv` distinguish exact reuse from the new product behavior;
- `evidence/unchanged-source-vs-xna4.log` retains all 46 compiler diagnostics;
- `evidence/xna4-effect-compile-status.tsv`, the per-profile logs, Debug/Release pipeline logs and
  `xna4-diagnostic/Content/` retain exact effect and 14-XNB results;
- `evidence/xna2-effectpool-documentation.txt`, `xna4-effectpool-api-boundary.txt`,
  `live-clone-semantics.txt` and `legacy-api-and-effect-pool-scan.txt` retain the API/source basis;
- `cna-diagnostic/` retains all twelve validated CNBs, while the CNA parse, real-GLES3 bind,
  conversion and 53+17 passing-test logs are under `evidence/`;
- `scripts/qualify.sh` reproduces the complete offline audit under isolated Xvfb. Its only parallel
  converter step is explicitly capped at eight workers; no host-display browser is opened.

## Owner decision required

Choose one:

1. authorize a distinct legacy-XNA compatibility product, including an authentic XNA2 reference
   route and deliberately `CNAEXT`-scoped EffectPool/shared semantics without contaminating CNA's
   normal XNA4 API;
2. authorize an explicit complete XNA4 modernization: apply the 46 measured mechanical API
   mappings, upgrade only the point-marker profile, replace pool propagation with a documented
   per-material scene-parameter contract, and then qualify the full 1–300 light behavior in native
   OPENGLES3 and real-browser WEBGL2;
3. accept an evidence-backed historical educational-product non-port boundary while retaining its
   complete source, assets, exact XNA4 content evidence and documentation.

This decision may share an implementation strategy with SAMPLE-130, but SAMPLE-134 remains a
separate runnable product with a different algorithm and shaders. Until the ruling, silently
copying shared values to nine clones, dropping the SM2/SM3 branch, capping the lights, using stock
lighting or calling a reduced scene the port would violate the campaign's fidelity rules.
