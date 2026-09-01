# SAMPLE-119 — `BasicEffectShader_ARCHIVE_2_0` audit and owner decision

## Status

Fresh audit complete enough to require an owner scope decision under `SAMPLES-DEC-005`. This is a
three-file XNA 2.0 educational shader archive, not a game, authoring tool or runtime library. The
source was not silently upgraded to XNA 4 shader models, substituted for CNA's XNA 4 `BasicEffect`
or wrapped in an invented viewer.

## Classification and complete inventory

The entire upstream directory contains **three files / 80,778 bytes**:

- `BasicEffectShader/BasicEffect.fx`: 17,906 bytes / 685 source lines;
- `BasicEffectShader/Readme.htm`: 16,952 bytes;
- `BasicEffectShader/Microsoft Permissive License.rtf`: 45,920 bytes.

There is no solution, project, C#/C++ code, compiled effect, executable, entry point, content asset,
scene, input path or runtime UI. The readme explicitly describes the HLSL as the educational source
behind the BasicEffect shipped with the contemporary XNA Framework and recommends the simpler
Shader Series for introductory shader learning.

The exact shader SHA-256 is:

```text
718a159b23765dd75c9ec848e149ef447688f70c1dd029f53505a1c1016743eb
```

It implements the documented twelve `ShaderIndex` modes across:

- twelve vertex entry points, all compiled as `vs_1_1`;
- four pixel entry points, referenced as eight `ps_1_1` and four `ps_2_0` array entries;
- one dynamically indexed `BasicEffect` technique and one pass;
- optional texture, vertex color, three directional lights, per-vertex/per-pixel lighting,
  material and fog behavior.

## It is not the XNA 4 stock shader

This XNA 2 source is structurally distinct from the XNA 4 `StockEffectsSample_4_0` BasicEffect:
the old file is a self-contained world-space implementation with explicit World/View/Projection
registers and dynamic shader arrays; XNA 4 uses shared includes, modern constant layouts, explicit
fog/no-fog permutations and a substantially different 588-line implementation. Their complete
source diff is retained. After line-ending normalization, the XNA 4 sample shader is byte-identical
to the current authoritative FNA `BasicEffect.fx`, confirming the correct modern comparison point.

Replacing CNA's XNA 4 implementation with this older shader would violate the campaign's XNA 4
fidelity requirement. Conversely, rewriting `ps_1_1` to a newer profile would create a new port,
not validate the archived source.

## Authentic compiler evidence

The unchanged shader was passed to the already-qualified SAMPLE-004 `CompileEffect.exe` using the
official Microsoft XNA Framework and Content Pipeline assemblies, both version 4.0.0.0. The
compiler fails deterministically at the original pixel-shader declaration:

```text
error X3539: ps_1_x is no longer supported
(660,13): ID3DXEffectCompiler::CompileEffect: There was an error compiling HLL shader parameter
ID3DXEffectCompiler: Compilation failed
```

This is an expected cross-version incompatibility, not an invalid-source finding and not a CNA
defect. No XNA 2 Content Pipeline/toolchain is present on the live host, and upstream did not ship a
compiled blob. Therefore there is no authentic XNA 2 bytecode to feed to CNA or compare visually.
An unrelated compiler or edited profile would be a workaround and was not used.

## Current CNA evidence and boundary

CNA correctly targets the XNA 4 API/behavior represented by FNA. On current HEAD:

- 46/46 focused `BasicEffectDefaults`, stock-effect content-reader and CNJ stock-effect tests pass
  on real offscreen OPENGLES3;
- the standalone OPENGLES3 BasicEffect render gate reports all checks passing;
- the C API BasicEffect lifecycle smoke exits successfully;
- MojoShader parses SAMPLE-004's authentic XNA 4 BasicEffect blob as 26 parameters, one technique,
  one pass, 35 objects and 30 shader objects containing 24,296 bytes of shader bytecode.

These tests prove that the modern framework surface and compiled-effect route work. They do not
claim that CNA compiles HLSL source or accepts an unavailable XNA 2 compiled payload. As established
for SAMPLE-004, CNA has no design-time XNA Content Pipeline/Effect source compiler; runtime
`Effect` parsing is not an authoring tool.

No CNA or Sharp Runtime change was made. There is no original/native/browser runtime gate because
upstream supplies no runnable product. Inventing a triangle viewer would test new code rather than
this archive.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-119-BasicEffectShader_ARCHIVE_2_0/`.

- `xna2-original/` is the complete byte-for-byte three-file upstream snapshot;
- `evidence/file-inventory.txt` and `sha256sum.txt` cover the full delivery;
- `evidence/readme-text.txt` is a stable rendering of the original documentation;
- `evidence/shader-summary.tsv` records the shader entry points, profiles and technique/pass count;
- `evidence/xna2-xna4-shader.diff` is the complete old-versus-modern source delta;
- `evidence/xna4-fna-normalized-diff.txt` is empty;
- `evidence/compiler-identity.tsv`, `xna4-compile-status.tsv` and `xna4-compile.log` retain the exact
  official cross-version compiler result;
- `evidence/cna-basic-effect-tests.log`, `cna-basic-effect-render.log`,
  `cna-c-api-basic-effect.log` and `cna-compiled-effect-parser.log` retain the successful modern CNA
  gates;
- `evidence/snapshot-diff.txt` is empty;
- `scripts/audit.sh` and `scripts/qualify.sh` reproduce the source and CNA evidence.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port/archive boundary for this complete educational XNA 2 shader
   source, analogous to the accepted non-game boundary for SAMPLE-004;
2. classify it as retained historical documentation/support data for CNA's modern BasicEffect,
   without compiling or exposing it as a product; or
3. authorize a distinct legacy-shader project, including an authentic XNA 2 compiler/toolchain or
   explicitly approved translation contract, a useful native product, a WEBGL2 product definition
   and fidelity criteria against the old twelve-mode behavior.

Until that choice, profile-upgrading the file, aliasing CNA's current BasicEffect as its output or
creating a demonstration game around it would be a source-fidelity workaround.
