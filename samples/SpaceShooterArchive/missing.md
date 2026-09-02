# SAMPLE-146 — `SpaceShooter_ARCHIVE_3_0` audit and owner decision

## Status

Fresh audit complete enough to require an owner product decision under `SAMPLES-DEC-002` and
`SAMPLES-DEC-005`. This is a complete XNA 3.0 game whose defining renderer was removed during the
official XNA 4.0 transition. No partial shooter, particle substitution, hidden legacy-state shim or
unapproved XNA4 rewrite was added.

## Complete delivered product

The full licensed delivery contains **83 files / 28,706,714 bytes**. Its solution tree has:

- 30 runtime C# files / 4,670 lines;
- five custom-processor C# files / 304 lines;
- eleven effects / 1,076 HLSL lines;
- 667 lines of project/content-project XML;
- two Windows/Xbox solutions, documentation and the Ms-PL.

The 6,717-line measured source surface implements the complete Wing Commander-style tutorial game:
player keyboard/gamepad flight and shooting, a simple evasive enemy, bolt collision and damage,
win/lose state, chase camera, HUD, three sound effects, bloom, planet/atmosphere/sun/sky rendering,
a moving star field and eleven fire/smoke/spark particle-system configurations. This is not an
incomplete starter, effect-only archive or asset pack.

## Exact XNA 3 to XNA 4 boundary

The unchanged five-file custom processor assembly compiles against Microsoft's local XNA 4.0
pipeline with zero errors. The unchanged runtime compiler reaches the first two removed semantic
types, `ResolveTexture2D` and `RenderState`, then stops before member binding can enumerate the rest.
The retained whole-source scan measures, among other sites:

- two `ResolveTexture2D` and one `ResolveBackBuffer` references;
- three indexed `SetRenderTarget(0, ...)` calls;
- 48 `RenderState`, seven old vertex-stream-slot and five device-owned
  `VertexDeclaration` references;
- 17 point-sprite/point-size references;
- the removed Effect/pass `Begin`/`End` protocol and `SaveStateMode`.

These are not CNA omissions from the XNA 4 API. Microsoft's local XNA 4 `Particle3DSample` is the
authoritative migration precedent: it expands one XNA3 point vertex into four corner vertices,
adds six triangle indices per particle, replaces `PointList` with indexed `TriangleList`, and
replaces hardware `PSIZE`/point-sprite texture coordinates with explicit quad expansion and
vs_2_0/ps_2_0 texture coordinates. The local XNA 4 `BloomSample` likewise renders the scene into a
`RenderTarget2D` instead of resolving the back buffer and uses the XNA4 immutable state/SpriteBatch
contract. Both comparisons are retained explicitly; this is a defining renderer rewrite, not a
mechanical C#-to-C++ translation.

## Authentic content evidence

The exact 29-row XNA3 content project first fails unchanged under XNA4 at an effect function named
the now-reserved `PixelShader`. An isolated diagnostic copy, never the original snapshot, applies
only the measured compiler migrations:

- rename the three bloom pixel-shader functions and the particle vertex-shader function;
- raise four `ps_1_1` entries to `ps_2_0`;
- remove the obsolete `POINTSIZE` pass state.

Microsoft's official XNA4 Windows/HiDef Release pipeline then builds all 29 authored rows and their
dependencies into **38 XNBs**: three SoundEffects, eleven Effects, one SpriteFont, three custom-
material Models and twenty Texture2Ds. All four nontrivial custom processor types run successfully,
including alpha-texture conversion, height-to-normal conversion and custom ship material/model
processing.

Current CNA converts and structurally validates all **24 generic products** (three sounds, one font
and twenty textures). It truthfully refuses eleven raw `EffectReader` roots because CNB does not
claim a compiled-effect schema, and refuses the three custom-material models rather than erase
their `EffectMaterialReader` graphs. This is a native-CNB representation boundary, not evidence
that the authentic XNBs failed to build.

The compiled bytecode extracted from all eleven XNA4-built Effect XNBs parses through the pinned
MojoShader: 23 shader objects and 20,336 bytes of Shader Model bytecode. Seven complete effects
(eight passes) bind on a real offscreen GLES3 context. The three bloom effects are intentionally
pixel-only and use SpriteBatch's stock-vertex inheritance, a contract covered by the focused CNA
renderer suite. The retained legacy particle effect parses, but its real GLES3 link fails precisely
because fragment input `io_5_0` (the hardware point-sprite TEXCOORD) has no preceding vertex-stage
output. CNA separately supports point primitives; this failure is specifically the old compiled
Effect point-sprite linkage that Microsoft's XNA4 quad migration eliminated.

Focused qualification passes:

- **90/90** XNB pipeline, Effect/EffectMaterial/Model, SoundEffect, SpriteFont and Texture2D tests;
- **32/33** EasyGL compiled-effect tests, with the one known unrelated volume-sampler precision
  case honestly skipped;
- all 24 generated CNBs through `cna_tool_cnb_info`;
- all seven full-pair sample effects through a real GLES3 context.

No CNA or Sharp Runtime source change was justified: changing the XNA4 public API to expose removed
XNA3 state/resolve types would violate the framework contract, while silently replacing the
particle renderer inside the sample would choose a product modernization without owner approval.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-146-SpaceShooter_ARCHIVE_3_0/`.

- `xna3-original/` is the complete byte-for-byte 83-file upstream snapshot;
- `evidence/source-inventory.tsv`, `original.sha256`, `source-shape.tsv` and the empty
  `snapshot-diff.txt` cover the full input;
- `evidence/unchanged-*-vs-xna4.log` and `unchanged-source-vs-xna4-result.tsv` retain the exact
  processor/runtime compiler result;
- `evidence/xna3-api-migration-sites.txt` and `xna3-api-migration-counts.tsv` retain every measured
  legacy site, while `xna4-authoritative-migration.txt` records the local official precedents;
- `xna4-diagnostic/content-source/` and `evidence/xna4-content-migration.diff` isolate every content
  edit; `xna4-content-build-migrated.log` retains the successful 38-output official build;
- `xna4-diagnostic/effects/` retains all eleven extracted bytecodes and their manifest;
- `evidence/mojoshader-effect-probe.log`, `mojoshader-gles3-full-pair.log` and
  `mojoshader-particle-diagnostic.log` retain parse, real-context bind and exact particle failure
  evidence;
- `cna-diagnostic/`, `cna-content-transcode.log`, `cna-cnb-info.log`, both focused test logs and
  `qualification.log` retain the CNA boundary and passing gates;
- `scripts/compile-unchanged-against-xna4.sh`, `build-xna4-content-diagnostic.sh`,
  `extract-effect-xnb.py` and `qualify.sh` reproduce the offline evidence. Every parallel command
  is capped at eight workers, and every graphics test uses an explicit virtual or offscreen display.

There is no native-game or browser-game claim because no representation choice has been authorized.
Inventing a reduced endpoint merely to produce those gates would not qualify this game.

## Owner decision required

Choose one:

1. authorize an isolated authentic XNA3 compatibility product, including the old point-sprite,
   effect/state and resolve semantics plus exact custom-material content behavior;
2. authorize a complete XNA4 modernization following Microsoft's quad-particle and render-target
   bloom precedents, then port that complete result to C++ CNA and qualify native/WEBGL2 behavior;
3. retain the measured source/content evidence as a historical game non-port.

The owner-approved pregenerated-XNB boundary can preserve the exact custom pipeline output for a
runtime product. Requiring CNB-only packaging would additionally need an explicit compiled-effect
and custom-material model schema decision; it must not be solved by dropping effects or materials.
