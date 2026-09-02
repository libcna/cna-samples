# Racing Game Milestone 2 — authentic XNA pipeline/XNB proof

## Status

Milestone 2 is in progress. The former Win7 filesystem block, authentic content
build, public CNA load path and processor vertex-layout proof are complete. The
unchanged-XNA structural comparison and a meaningful draw of all four proof models
remain before the milestone can close.

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

On `CNA_GRAPHICS_RENDERER=OPENGL33`, CNA `0eb5fc151` and sharp-runtimenext
`9cc96cd5`, the harness reports **64/64 PASS**. The measured model graph is:

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

## General CNA repairs

CNA commit `0eb5fc151` fixes reusable Effect Framework behavior exposed by these
authentic XNBs:

- accept repeated object records and bounded XNA type-1 auxiliary records;
- preserve safe object/value bounds throughout the pinned MojoShader parser;
- translate legacy Shader Model 1 `TEXCRD` for desktop GLSL and GLSL ES;
- normalize XNA render-state flag bits before converting to MojoShader's enum;
- reject identifiers outside ordinary states `0..102` and shader pseudo-states
  `146/147` before a C/C++ enum object can hold an invalid value.

The committed 17,404-byte authentic ShadowMap EffectReader payload has SHA-256
`0014e2286cdd67c96f7ab24c7bf311c533e1b12461d1cb9a5453a068c8842d5d`.
Qualification passed the direct Basic/Conformance/Racing MojoShader probe, focused
Debug and ASan/UBSan Effect tests, a deterministic 2,000-iteration mutation campaign
with seed `0xffffffffffffffff`, and this 64/64 real-GL harness run.

The broad `EffectTest.*` process still reproduces a pre-existing order-sensitive
segfault when a second GL `GraphicsDevice` is constructed in the same test process.
The two affected regressions pass independently; that unrelated lifecycle issue was
not folded into this Effect/content change.

## Remaining exit work

1. Run a small inspector under the unchanged XNA executable/runtime and retain the
   same bones, transforms, names, parent relationships, part order, bounds, material
   technique suffixes, texture formats and mip counts.
2. Compare the XNA dump with CNA rather than assuming successful loading implies
   exact reflection parity.
3. Draw `Car`, `Windmill`, `AlphaDeadTree` and `Cube` through their authentic
   model/effect/material graph and assert meaningful pixels/transforms.
4. Close Milestone 2 only when those comparisons pass without patching XNBs or
   adding a sample-local renderer/content workaround.
