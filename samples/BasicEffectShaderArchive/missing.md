# SAMPLE-119 — `BasicEffectShader_ARCHIVE_2_0` audit and owner decision

## Owner decision — cancelled, 2026-10-03

After the SAMPLE-119 analysis, the owner explicitly instructed:
**"ponech cancelled a analyzuj sample s cislem o 1 vetsim"**.
In that context this cancels 119 and requests analysis of 120. SAMPLE-119 is `⛔` (cancelled).
All three original files and audit/compiler evidence remain retained. No legacy compiler,
profile conversion, viewer or native/browser product is authorized, and no artifact cleanup
was requested. Cancellation accepts the educational-source archive boundary; it does not
claim an authentic XNA2 build or runtime qualification. CNA's general effect/pipeline support
remains available. The analysis below records the evidence and the options before this decision.

## Historical current-head re-analysis — 2026-10-03

**Status at analysis: 🛑, independent owner scope decision pending.** The owner instructed
"ponech 118 cancelled a analyzuj 119". 118 is cancelled; that does not classify this shader
archive. Every one of the **three files / 80,778 bytes** freshly matches the physical source
and retained `xna2-original` by path, size and SHA-256. The whole 685-line shader, HTML workflow/
twelve-mode table and visible Ms-PL licence body were reviewed. Shader SHA-256 remains
`718a159b23765dd75c9ec848e149ef447688f70c1dd029f53505a1c1016743eb`.
There is no project, content project, application, compiled blob or original runtime scene.

All twelve documented ShaderIndex values match the vertex/pixel arrays: twelve unique vertex
entry points compile as `vs_1_1`; four unique pixel entry points occupy eight `ps_1_1` and four
`ps_2_0` slots. One technique/pass indexes both arrays directly. This is a self-contained
world-space texture/material/three-directional-light/fog implementation, without includes.
World/View/Projection and world-space EyePosition are required even for the default flat mode;
lights are disabled by zero colours, and fog is controlled by FogEnabled = 0/1. The twelve
documented texture/vertex-colour/vertex-lighting/pixel-lighting combinations are retained intact.

The modern original SAMPLE-004 BasicEffect still equals authoritative FNA byte-for-byte after
line-ending normalization (588 lines, normalized SHA-256
`2c51c012b2659114c86ab9b6b0d461a637308e87e752c810c5da0c63ad38b107`). It has 20 vertex
and ten pixel shaders selected through 32 mapping slots, shared includes, WorldInverseTranspose,
WorldViewProj/FogVector and separate fog/no-fog and one-light variants. It is materially
different from this archive; replacing it with 119 or aliasing it as 119's output is not a port.

**The historical blanket pipeline-absence claim is superseded.** Current CNA `next 75b55659c`
has XNA-shaped EffectImporter/EffectProcessor and a canonical `.fx` pipeline that drives an
external legacy `fxc` at `fx_2_0`, tracks includes and validates compiled containers. It does not
embed HLSL compilation or rewrite the shader profiles. SAMPLE-004 is now a completed original
CompileEffect CLI port, rather than the earlier archival non-port. Unlike 004, 119 supplies no
CLI or other executable product. A new legacy preview/compiler product remains a scope choice.

**Fresh compiler probes:** the unchanged original Microsoft XNA4 CompileEffect.exe (framework
and pipeline assemblies both 4.0.0.0) rejects the exact 119 source in Windows/Reach and HiDef,
exit 1 at line 660 with `X3539: ps_1_x is no longer supported`; no output is created. The retained
CNA SAMPLE-004 Release host tool gives the same rejection for both profiles using the genuine
June 2010 SDK compiler 9.29.952.3111. As positive controls, both tools successfully compile the
unchanged modern XNA4 BasicEffect, each producing 28,840 bytes. Their hashes are retained;
byte identity is not claimed across the two compiler versions. These are fresh compiler runs
using existing qualified executables, not a new build or current-HEAD runtime qualification.

The SDK diagnostic also suggests `/LD` for an old compiler DLL. A separate genuine `fxc /LD
/T fx_2_0` probe on the unchanged legacy source exits 1 with `compilation failed; no code produced`,
and no output; this diagnostic establishes no more specific cause. `/Gec` was not used because it
automatically upgrades profiles. No authentic XNA2 build/bytecode or original runtime capture is
qualified. No profile edit, shader substitute, invented viewer or framework/runtime repair was added.

Current evidence is under
`/rv/tmp/samples/SAMPLE-119-BasicEffectShader_ARCHIVE_2_0/evidence/current-head-analysis-20261003/`:
complete inventory, shader/mode/register data, modern-source comparison/diff, full readme/licence
text, compiler identities/commands/logs and two explicitly labelled XNA4 positive-control blobs.
Reproduce source/compiler checks with `python3 .../scripts/current-head-analysis-20261003.py --compile`
and the `/LD` probe with `python3 .../scripts/probe-legacy-ld-20261003.py`. Snapshot is compared,
not replaced; all historical evidence remains. No artifact pruning or new native/browser product.

CNA/Sharp source is unchanged (Sharp `next db86514c`); no new runtime regression or browser gate
was required or claimed. The older 46/46/render/C API/parser records below remain historical
modern-framework compatibility evidence, not a requalification at today's HEAD.

**Owner options:** accept archival cancellation (recommended, retain all three files/evidence);
retain historical support documentation without a target; or explicitly define a legacy-shader
product, obtaining a usable authentic compiler/reference or approving a translation contract,
then defining native/WEBGL2 behavior and fidelity for all twelve modes. The source-only audit
does not establish a reliable new-product estimate. `rules.md`, SAMPLES-DEC-005 reserves that
choice to the owner; 119 stays 🛑 until decided.

## Status

`⛔` — cancelled by the explicit owner decision above under `SAMPLES-DEC-005`.
The fresh audit established the educational-source boundary. This is a
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

## Historical authentic compiler evidence

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

## Historical CNA evidence and current boundary

CNA targets the XNA 4 API/behavior represented by FNA. The original 2026-09-01 audit retained:

- 46/46 focused `BasicEffectDefaults`, stock-effect content-reader and CNJ stock-effect tests pass
  on real offscreen OPENGLES3;
- the standalone OPENGLES3 BasicEffect render gate reports all checks passing;
- the C API BasicEffect lifecycle smoke exits successfully;
- MojoShader parses SAMPLE-004's authentic XNA 4 BasicEffect blob as 26 parameters, one technique,
  one pass, 35 objects and 30 shader objects containing 24,296 bytes of shader bytecode.

These tests prove that the modern framework surface and compiled-effect route work. They do not
claim an authentic XNA 2 compiled payload or today's runtime qualification. The older assertion
that CNA has no design-time Content Pipeline/Effect source route is superseded: the external
compiler pipeline and completed SAMPLE-004 CLI are recorded in the current-head section above.
No compiler is embedded in runtime Effect; the archived source's legacy compilation and any
new application remain independent of CNA's working modern route.

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

## Historical owner options — resolved by cancellation above

Choose one:

1. accept an evidence-backed non-port/archive boundary for this complete educational XNA 2 shader
   source, which supplies no CLI or other runtime product (SAMPLE-004's original CLI is now ported);
2. classify it as retained historical documentation/support data for CNA's modern BasicEffect,
   without compiling or exposing it as a product; or
3. authorize a distinct legacy-shader project, including an authentic XNA 2 compiler/toolchain or
   explicitly approved translation contract, a useful native product, a WEBGL2 product definition
   and fidelity criteria against the old twelve-mode behavior.

Until that choice, profile-upgrading the file, aliasing CNA's current BasicEffect as its output or
creating a demonstration game around it would be a source-fidelity workaround.
