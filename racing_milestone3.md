# Racing Milestone 3 — compiled Effect integration proof

## Result

Milestone 3 is complete on 2026-09-02. CNA loads and executes the representative
normal/specular and multi-pass post effects from the authentic XNA Game Studio 4.0
content build. No Racing-only effect dispatcher, byte parser, shader rewrite or
asset substitution is present.

The final public harness is 97/97 PASS in both Debug and ASan/UBSan modes. Its four
effect readbacks are bit-exact with the FNA/OpenGL behavior oracle. The normal-map
and changed-clone images are also bit-exact with the unchanged XNA 4/D3D9 oracle.

## Frozen revisions and environment

- CNA: `756096626cfb537e32ac8ccc6b0743a23c57ddfd`
- cna-samples starting revision: `148e4525f047fb283228c4bbeab5a3e5c2da529d`
- sharp-runtime: `9cc96cd57cde394940cc24d58743edf9bf63d3fb`
- FNA: `1358793096d556388be798d24dc3728b2abb10ff`
- FNA3D: `32401479a3ab5bd6b2e7f786e87bf4166aa03b0f`
- modern Racing behavior oracle: `d8092633e4e43e014ff168d8e913a9373538b851`
- CNA renderer: `OPENGL33`, HiDef, Mesa llvmpipe OpenGL 4.5
- authentic runtime: Windows 7 32-bit, XNA Framework/Game 4.0.0.0

The Win7 VM remained offline (`nic1=none`, `nic2=none`) and was returned to the
`saved` state after qualification. The oracle was compiled by the VM's .NET 4
MSBuild and XNA Game Studio targets as x86/HiDef. The clean rebuild completed with
zero errors and its interactive scheduled run returned result code zero; the
one-shot task was deleted afterward.

## Canonical inputs

The CNA and XNA runs use only these authentic products:

- `evidence/xna4-authentic-build/Debug/Content/Shaders/NormalMapping.xnb`
- `evidence/xna4-authentic-build/Debug/Content/Shaders/PostScreenShadowBlur.xnb`

Their implementation authority remains the original `.fx` source below
`xna4-original/RacingGameWindows1/RacingGame/RacingGameContent/Shaders/` and the
original `ShadowMapBlur.cs`/`VBScreenHelper.cs` flow. The runtime vertex is the
original 44-byte `Position@0 + TextureCoordinate@12 + Normal@20 + Tangent@32`
layout. It is deliberately separate from the processor-authored 56-byte model
layout proved in Milestone 2.

The FNA comparison uses the same C# oracle source compiled against the authoritative
local FNA checkout. Current FNA cannot construct the authentic XNB's full Effect
graph because its unpatched MojoShader stops on a legacy `ps_1_1 TEXCRD` instruction
in an unused technique. For the FNA renderer-only run, and only for that run, the
modern repository's original FNA `.efb` products are loaded as the permitted
behavior oracle. They are not copied into the port, loaded by CNA or treated as
canonical content.

## XNA 4 oracle

The reproducible project is
`samples/RacingGameHarness/oracle/xna4/RacingEffectOracle.csproj`. It loads both
authentic XNBs through `ContentManager` and records:

- all 14 `NormalMapping` techniques in authored order;
- `Specular20` with its single `P0` pass;
- `diffuseTexture`, `normalTexture` and the runtime material parameter surface;
- authored `shininess=16` before mutation;
- equal clone/original parameters producing identical pixels;
- clone parameter mutation not changing original parameter storage;
- a complete 32x32 normal-mapped quad and all 1024 pixels changing on clone
  mutation;
- `ScreenAdvancedBlur20` with exactly
  `AdvancedBlurHorizontal,AdvancedBlurVertical`;
- real 32x32 render-target execution and readback for both passes.

Frozen XNA metrics:

```text
NORMAL center=84,42,21,255 lit=1024 cloneChanged=1024
BLUR sourceLit=16 horizontalLit=64 verticalLit=220
     horizontalRow=16 horizontalColumn=4 verticalRow=16 verticalColumn=16
RESULT PASS
```

## CNA and FNA/OpenGL evidence

The CNA harness uses the same 32x32 input textures, matrices, material values,
runtime vertex bytes and fullscreen quad. It drives the authentic compiled effects
through public `ContentManager`, `Effect`, `EffectParameter`, `EffectPass`,
`VertexBuffer`, `Texture2D`, `RenderTarget2D` and `GraphicsDevice` APIs.

Frozen OpenGL metrics from both CNA and FNA:

```text
NORMAL center=84,42,21,255 lit=1024 cloneChanged=1024
BLUR sourceLit=16 horizontalLit=75 verticalLit=232
     horizontalRow=15 horizontalColumn=5 verticalRow=16 verticalColumn=16
RESULT PASS
```

The D3D9 and OpenGL blur footprints differ slightly because of backend sampling and
pixel-center conventions. The actual images remain close, with normalized RMSE
`0.030538` for the horizontal pass and `0.018422` for the vertical pass. More
importantly, CNA and authoritative FNA/OpenGL are byte-for-byte identical for every
readback, so this is not a CNA-only rendering divergence.

Key SHA-256 values:

| Readback | XNA 4 | FNA/OpenGL | CNA/OpenGL33 |
|---|---|---|---|
| normal | `5170b23b90d70e844a2ffb24be5100e22f49fb1e7ae77e7b567b9358e55fd919` | same | same |
| changed clone | `8714dadebd97bc97720b5bc039a0c723c767926f233ea31807087855db95caf5` | same | same |
| horizontal blur | `0bc59f0e4e0c8d89741da446b7ab5b43b07885adb499b6a00233d7891542c273` | `b9a2f6ef969a38a159dba15761b44cb8d0bb763c90cbdc5adf1d40935ab62468` | same as FNA |
| vertical blur | `5d46951b8cc15517f949b8ed2bae965d52421f73d90b8f40d91229d1c5c601ee` | `554a9d7222a07af98dd4c77cda4a1191922606f880b0012da1fc1ec3141c7629` | same as FNA |

The committed `compare-effect-oracles.py` enforces the exact and bounded comparisons
and reports:

```text
RACING_EFFECT_ORACLE_COMPARISON=PASS fna_cna_exact=4
xna_cna_normal_exact=2 xna_gl_horizontal_rmse=0.030538
xna_gl_vertical_rmse=0.018422
```

## Qualification

`scripts/qualify-milestone1.sh` remains the permanent harness entry point but now
also supplies the authentic Content root and Milestone 3 effect evidence path. It
uses at most eight build jobs and the pinned Racing FNA3D/MojoShader checkout.

- Debug OPENGL33 + real synthetic X11 keyboard/mouse input: 97/97 PASS.
- ASan/UBSan with leak detection disabled: 97/97 PASS, no finding.
- LSan classification run: 97/97 PASS; 100,956 bytes in 449 allocations are wholly
  rooted in external Mesa `libGLX_mesa`, matching the existing classification.
- XNA/FNA/CNA effect comparator: PASS.
- `git diff --check`: PASS before commit.

Evidence roots:

- `evidence/xna4-authentic-effect-oracle/`
- `evidence/fna-authentic-effect-oracle/`
- `evidence/cna-opengl33/milestone3/`
- `evidence/cna-opengl33/milestone1/` for the final full harness logs

## Engine disposition and exit

No new CNA or sharp-runtime change was required during this milestone. The current
CNA compiled-Effect runtime, including the already committed general parser fixes at
`756096626`, supports both representative effects through the normal API. The
Milestone 3 exit condition is therefore satisfied: no Racing-specific effect path
is needed.

Milestone 4 may now begin with the original raw track/combi/landscape readers and a
representative static scene.
