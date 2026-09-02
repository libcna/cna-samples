# SAMPLE-147 — `SpriteBatchShader_ARCHIVE_2_0` audit and owner decision

## Status

Fresh audit complete enough to require an owner scope decision under `SAMPLES-DEC-005`. This is a
three-file pre-XNA4 educational shader archive, not a game, sample project, authoring tool or
runtime library. No profile-upgraded shader, CNA stock-effect alias or invented viewer was added.

## Classification and complete inventory

The entire upstream directory contains **three files / 66,433 bytes**:

- `SpriteBatchShader/SpriteBatch.fx`: 3,709 bytes / 135 source lines;
- `SpriteBatchShader/Readme.htm`: 16,804 bytes;
- `SpriteBatchShader/Microsoft Permissive License.rtf`: 45,920 bytes.

There is no solution, project, C#/C++ game source, compiled Effect/XNB, executable, entry point,
texture, scene, input path or runtime UI. The readme explicitly calls the HLSL educational source
for the SpriteBatch shader built into the contemporary XNA Framework and presents it only as a
starting point for custom shaders. Although the directory name says `ARCHIVE_2_0`, its own help
metadata includes `ProjType=XNA_30` and the document is dated 2008; the honest classification is
therefore pre-XNA4 rather than a claimed exact XNA2 release identity.

The exact shader SHA-256 is:

```text
99e546810e873f943c45efe6750c9d60bf482bcfcc023cdf28e518c814b19223
```

Its single technique/pass compiles one `vs_1_1` and one `ps_1_1` program. Four explicit inputs
provide viewport size, texture size, matrix and sampler state. The Windows vertex branch receives
four CPU-expanded corners and performs transform, half-pixel, viewport and texture-size
normalization. The Xbox branch instead receives one vertex per sprite and uses five `vfetch`
instructions plus the vertex index to generate, rotate, mirror and texture four corners on the
GPU. The shared pixel shader multiplies sampled texture and vertex colour. The readme itself warns
that this SpriteBatch/shader interface is undocumented and may differ by platform or future
framework release.

## It is not the XNA 4 stock SpriteEffect

Microsoft's local XNA 4 `StockEffectsSample_4_0` provides the authoritative successor. Its
44-line `SpriteEffect.fx` removes the Xbox `vfetch` branch and `ViewportSize`/`TextureSize`, accepts
the XNA4 sprite vertex/normalized-UV contract, uses `SV_Position`, and compiles `vs_2_0`/`ps_2_0`.
The complete source diff is retained. After line-ending normalization, that official XNA4 source
is byte-identical to current FNA's authoritative `SpriteEffect.fx`.

Replacing CNA's XNA4 SpriteEffect with this older source would therefore regress the framework
contract. Merely changing its profile declarations would manufacture a modernization while
leaving its obsolete vertex-data/register agreement unresolved.

## Authentic compiler evidence

The unchanged archive shader was passed to the already-qualified SAMPLE-004 `CompileEffect.exe`
using Microsoft's XNA Framework and Content Pipeline assemblies, both version **4.0.0.0**. It
fails deterministically at the original pixel-shader assignment:

```text
error X3539: ps_1_x is no longer supported
(133,18): ID3DXEffectCompiler::CompileEffect: There was an error compiling expression
ID3DXEffectCompiler: Compilation failed
```

This is an expected cross-version result, not an invalid-source or CNA finding. No source line was
changed. As a positive control, the same compiler builds the official XNA4 SpriteEffect to a
1,120-byte blob whose SHA-256 is
`93ca64d798facc9a3448e1eb74d805962fb3d821cc9b6eabf89440f4aa96cffc`; it is byte-identical to
SAMPLE-004's retained official output.

## Current CNA evidence and boundary

CNA correctly implements the XNA4/FNA SpriteBatch and compiled-effect contract rather than a
pre-XNA4 source compiler:

- the exact official XNA4 blob parses as three parameters, one technique/pass, five objects and
  two shader objects containing 572 bytes of Shader Model bytecode;
- MojoShader binds that pair on a real offscreen GLES3 context using `glsles3`;
- **5/5** focused EasyGL compiled-Effect SpriteBatch tests pass, covering an actual custom effect,
  stock-vertex inheritance for pixel-only effects, render-target sources, multi-pass ordering and
  sprite texture/source/flip delivery;
- a separate custom-uniform draw passes, as do **13/13** viewport/transform checks and **8/8**
  horizontal/vertical flip pixel probes.

No CNA or Sharp Runtime source change was justified. CNA's runtime compiled-effect support is not
a design-time HLSL authoring tool, and upstream supplies no authentic compiled pre-XNA4 blob. There
is no original/native/browser runtime gate because upstream has no runnable product. Inventing a
sprite viewer would test new code rather than this delivery.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-147-SpriteBatchShader_ARCHIVE_2_0/`.

- `xna2-original/` is the complete byte-for-byte three-file upstream snapshot;
- `evidence/source-inventory.tsv`, `original.sha256`, `readme-text.txt` and the empty
  `snapshot-diff.txt` cover the full delivery;
- `evidence/shader-summary.tsv` records the measured source shape and its `XNA_30` metadata;
- `evidence/prexna4-xna4-sprite-shader.diff` retains the complete old-versus-XNA4 delta, while
  `xna4-fna-normalized-diff.txt` is empty;
- `evidence/compiler-identity.tsv`, `xna4-compile-status.tsv`, `xna4-compile.log`,
  `xna4-reference-compile.log` and `xna4-reference-blob.sha256` retain both official compiler
  outcomes;
- `xna4-diagnostic/SpriteEffect.bin` is the positive-control XNA4 output;
- the six `evidence/cna-*.log` files retain parse, real-context bind and focused SpriteBatch gates;
- `scripts/audit.sh` and `qualify.sh` reproduce the complete offline evidence. No browser or host
  display is opened.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port/archive boundary for this complete educational pre-XNA4
   shader source, analogous to SAMPLE-119's BasicEffect shader archive;
2. retain it as historical documentation/support data for CNA's XNA4 SpriteBatch implementation,
   without claiming it as a compiled or runnable product; or
3. authorize a distinct legacy-shader project, including an authentic matching pre-XNA4
   compiler/runtime contract or an explicitly approved translation, a useful native product, a
   WEBGL2 product definition and fidelity criteria for both Windows and Xbox branches.

Until that choice, raising shader profiles, treating the modern XNA4 SpriteEffect as this archive's
output or wrapping the file in a new demonstration would be a source-fidelity workaround.
