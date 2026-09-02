# Racing Game Milestone 2 — authentic XNA pipeline/XNB proof

## Status

Milestone 2 is complete as of 2026-09-02. The former Win7 filesystem block,
authentic content build, public CNA load path, processor vertex-layout proof,
unchanged-XNA structural comparison and meaningful four-model draw all pass.

No GLB, glTF, modern-repository material, modern FNA effect or handwritten content
decoder participated. The input is the original XNA 4 sample and its real Microsoft
XNA Game Studio 4.0 pipeline output.

## Authentic Windows build

The original powered-off `win7` VM was protected with snapshot
`pre-ntfs-repair-2026-09-02`. Work happened in the temporary linked clone
`cna-win7-racing-repair`; `chkdsk` repaired the cloned filesystem without modifying
the protected original disk.

Inside the offline Windows 7 32-bit guest, Visual Studio/XNA content at
`C:\Users\vboxuser\Desktop\XNAGameStudio` built the unchanged
`RacingGameWindows1\RacingGame\RacingGame\RacingGame.csproj` as `Debug|x86`.
The successful full-game rebuild log records Microsoft Build Engine 4.0, XNA Game
Studio 4.0, three original HLSL `pow` warnings, zero errors and 44.75 seconds.
The earlier direct `RacingGameContent.contentproj /t:Rebuild` experiment failed
because `TargetPlatform` was absent; it is retained as negative evidence and was
superseded by the correct game-project build.

Evidence root:

```text
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/evidence/xna4-authentic-build
```

The shared-folder export contains 358 files and 347.20 MiB:

- 339 XNB files;
- all 57 `Content/Models/*.xnb` products from the original `.X` inputs and
  `RacingGameModelProcessor`;
- all ten original compiled effect XNBs;
- 244 texture XNBs;
- original XACT `.xgs`, `.xsb` and `.xwb` products;
- track/combi/height raw files, `RacingGame.exe` and PDB;
- `SHA256SUMS-debug.txt`, a 358-entry hash manifest;
- successful `racinggame-debug.log` and `robocopy-debug.log`, plus the retained
  failed direct-content experiment `msbuild-debug.log`.

## CNA public load proof

`samples/RacingGameHarness` accepts the authentic exported `Content` directory via
`--content-root` and uses only `ContentManager::Load<Model>` for these original IDs:

```text
Models/Car
Models/Windmill
Models/AlphaDeadTree
Models/Cube
```

On `CNA_GRAPHICS_RENDERER=OPENGL33`, CNA `8cab5f32a` and sharp-runtimenext
`9cc96cd5`, the pre-draw harness reports **64/64 PASS**. The measured model graph is:

| Asset | Bones | Meshes | Effect kind | Vertex layout |
|---|---:|---:|---|---|
| Car | 19 | 6 | `EffectMaterial` | processor tangent frame, 56 bytes |
| Windmill | 7 | 2 | `EffectMaterial` | processor tangent frame, 56 bytes |
| AlphaDeadTree | 7 | 2 | `EffectMaterial` | processor tangent frame, 56 bytes |
| Cube | 2 | 1 | `BasicEffect` | position/normal, 24 bytes |

Every part has non-null vertex/index/effect resources, positive vertex/primitive
counts and finite bounds. The processor layout is exactly Position `Vector3` at 0,
Normal `Vector3` at 12, TextureCoordinate `Vector2` at 24, Tangent `Vector3` at 32
and Binormal `Vector3` at 44. This remains separate from the original game's
runtime-generated 44-byte `TangentVertex` layout: Position at 0, texture coordinate
at 12, normal at 20 and tangent at 32. The harness tests both independently.

## Unchanged-XNA semantic comparison

An inspector built into the unchanged XNA game project emits the same ordered
semantic report as the CNA harness. The committed
`scripts/compare-model-reports.py` comparison is strict for record order, bone and
mesh names, parent indices, part ranges, vertex layouts, effect type/technique and
parameter reflection, texture dimensions, mip counts and surface formats. It allows
only normal floating-point formatting differences for matrices/bounds and normalizes
the representational difference between a nullable .NET name and CNA's non-null C++
string.

The authentic XNA report and the real OPENGL33 CNA run agree on all 531 records:

```text
RACING_MODEL_REPORT_COMPARISON=PASS records=531 MODEL=4 BONE=35 MESH=11 PART=17 ELEMENT=82 PARAM=320 TEXTURE2D=30 TEXTURECUBE=31 TEXTURE=1
```

The comparison includes the processor-authored 56-byte tangent frames, all 320
effect parameter records and the original DXT formats for all 30 Texture2D and 31
TextureCube bindings. No XNB was modified and no asset was converted.

## General CNA repairs

CNA commit `0eb5fc151` hardens reusable Effect Framework behavior exposed by these
authentic XNBs:

- accept repeated object records and bounded XNA type-1 auxiliary records;
- preserve safe object/value bounds throughout the pinned MojoShader parser;
- translate legacy Shader Model 1 `TEXCRD` for desktop GLSL and GLSL ES;
- reject structurally invalid render-state records before applying a pass.

The first version incorrectly masked every render-state identifier with `~0xA0`,
which changed the legitimate XNA 4 shader pseudo-states `146/147` into `18/19`.
CNA commit `756096626` removes that invalid normalization, names the legacy
`PixelShaderConstant`/`SetSampler` records `160/178` in MojoShader's enum and accepts
exactly ordinary states `0..102` plus `146`, `147`, `160` and `178`. Every other
identifier is rejected before conversion to the C/C++ enum. Its regression applies
an authentic Racing ShadowMap pass and would previously have failed as unsupported
`FogStart`/`FogEnd`.

The committed 17,404-byte authentic ShadowMap EffectReader payload has SHA-256
`0014e2286cdd67c96f7ab24c7bf311c533e1b12461d1cb9a5453a068c8842d5d`.
Qualification passed the direct Basic/Conformance/Racing MojoShader probe, a
deterministic 2,000-iteration mutation campaign with seed `0xffffffffffffffff`, and
all 24 `EffectTest.*` tests in both Debug and ASan/UBSan configurations on a real
Mesa OpenGL 4.5 core context. The pinned patch stack also passes clean apply and
reverse/idempotence checks.

## Meaningful four-model draw

The harness draws all 17 parts from the four public `ContentManager` model results.
It uses the original `objectMatrix`, bone transforms, processor-authored mesh suffix
technique selection and authentic effect/material parameters. It neither decodes nor
modifies an XNB and does not use a renderer helper. The observed technique bindings
include `ReflectionSpecular20` for car glass, the expected
`SpecularWithReflection20`/`Specular20` car-part split,
`DiffuseSpecular20` for Windmill and AlphaDeadTree, and `BasicEffect` for Cube.

The complete isolated-Xvfb run against CNA `756096626` reports **69/69 PASS** with
three completed update/draw/present cycles. Each 160x90 proof viewport differs from
the clear colour and has non-flat luminance:

| Asset | Changed pixels | Luminance range |
|---|---:|---:|
| `Models/Car` | 605 | 483 |
| `Models/Windmill` | 79 | 422 |
| `Models/AlphaDeadTree` | 416 | 209 |
| `Models/Cube` | 613 | 308 |

Evidence is retained under `evidence/cna-opengl33/milestone2/`:

- `authentic-four-model-draw.log` — complete 69/69 run and all 17 submissions;
- `authentic-four-model-draw.ppm` — exact 320x180 readback, SHA-256
  `884d8089f8f4862476aeff3d7629b0721161641e303b2b610086bc94314151bd`;
- `authentic-four-model-draw.png` — review copy, SHA-256
  `99a9913c879e7ed39f4a62cdf9dfcc13409270b2cce11c7ab4888130c0f6697f`.

## Exit and next milestone

The four proof assets render with their authentic transforms and material
assignments, so Milestone 2's exit gate is satisfied. Milestone 3 may now qualify one
representative normal/specular Effect XNB and one multi-pass post Effect XNB before
gameplay translation begins.
