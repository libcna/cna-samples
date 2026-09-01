# SAMPLE-112 — `AvatarAnimPack_4_0_BIN` audit and owner decision

## Status

Fresh audit complete enough to require an owner representation decision under
`SAMPLES-DEC-004` and `SAMPLES-DEC-005`. No invented viewer, `Game`, decoder, loose-animation
substitute or CNA extension route was added. This physical directory is a licensed binary asset
delivery, not an XNA sample application, library project or buildable content project. Only the
owner may accept an evidence-backed non-port/archive boundary or authorize a new product.

## Classification and complete inventory

The exact upstream directory contains 23 files:

- 21 custom-avatar animation binaries: `climb`, `crawl`, `faint`, `jump`, `kick`, `pickUp`,
  `pull`, `punch`, `push`, `run`, `sitIdle`, `steer`, `strafeLeft`, `strafeRight`,
  `swimFreestyle`, `swimUnderwater`, `swingBat`, `swingClub`, `swingHammer`, `throw` and `walk`;
- a 512-character UTF-16BE PowerShell conversion script;
- the Microsoft Permissive License in RTF form.

There is no `.sln`, project/content-project file, C#/C++ source, entry point, readme, HTML
documentation, screenshot or declared consumer. Consequently there is no original configuration
to compile and no original/native/browser program whose visuals or controls can be qualified.
Calling a newly invented previewer a port would attribute behavior and UI to Microsoft that this
directory does not contain.

Every binary is distinct, ranges from 10,192 to 16,988 bytes, is a multiple of four bytes and
starts with the identical 24-byte sequence:

```text
53 54 52 42 01 00 59 bd e0 8b 49 09 4e e4 b9 91 ad a8 7b 7c 0b 6b 01 04
```

The observable four-byte magic is `STRB`. These are not XNB containers. The retained manifest
records SHA-256 for every file; representative identities are:

| Asset | SHA-256 |
|---|---|
| `climb.bin` | `faa6af84356ab3a60892f21162b2bedfb72f95f5e69dfa7a4d5624142e340434` |
| `jump.bin` | `5e26389b5ace9562cf63e3ea10d84424658984510427722d89aa30f0f58cc173` |
| `run.bin` | `78b0b30639f017261a921ac8e30615264485380949bda11832791e95637690a1` |
| `walk.bin` | `05bde63df2ca6d8d0d9491fd5e0079b4503ae994055011217cc6a97cbb52bdf7` |

No undocumented field semantics are claimed from the header alone.

## Supplied conversion path and related packs

`ConvertTool/ConvertFbxToBin.ps1` recursively selects `.fbx` inputs and describes the intended
asset category unambiguously through its converter arguments: male body type, custom asset usage,
Maya authoring package, `animation` asset type, a 0.01 scale, four tolerance/sample-rate values,
XZ translation and Y rotation mapping. It would write one same-basename `.bin` per FBX.

The shipped route is not executable as received:

- `AvatarAssetConverter_e.exe` is absent from this directory, the complete local XNA sample
  snapshot and the live CNA repositories;
- the script assigns `.\AvatarAssetConverter_e.exe` to `$convertToolRel` but invokes the
  undefined `$convertTool` variable.

The neighboring `AvatarAnimPack_4_0_FBX`, Maya and Mod Tool directories are separate physical
deliveries and therefore remain separate SAMPLE-113 through SAMPLE-115 audits. A targeted
cross-check confirms the FBX pack has the same 21 action names, but the five names also present in
SAMPLE-094 (`Faint`, `Jump`, `Kick`, `Punch`, `walk`) have different SHA-256 values from that
sample's FBX inputs. SAMPLE-094's documented custom content processor does not reference these
`STRB` files; it imports its own FBX/CSV data into XNB. SAMPLE-112 therefore cannot truthfully be
collapsed into that prior row or claimed as its pregenerated output.

## Live CNA audit and qualification

The live dependency state inspected for this audit was:

- CNA `7712534d3d22c7e284714e0e87afebba3f3cb472`;
- Sharp Runtime `9cc96cd57cde394940cc24d58743edf9bf63d3fb`.

CNA contains no decoder or content contract keyed by the `STRB` magic. Its own `.bin` sidecars use
documented CNA formats and do not imply compatibility with this distinct payload. The normal
XNA-shaped Avatar surface also remains the already documented off-Xbox boundary: 71-entry
hierarchy, zero-length/zero-matrix preset animations, invalid random descriptions,
`AvatarRendererState::Unavailable`, unavailable bind pose and no-op normal `Draw`. CNA's real
avatar rendering path is explicitly `CNAEXT` and uses substitute bodies/clips, so it cannot be
used to invent a faithful preview for this pack.

The current `CnaGamerServicesTests` target built with `-j8`. The first display-unspecified run
passed 74/76 and failed two initialization cases only because SDL had no selected video driver.
Repeating the unchanged 76-test AvatarAnimation/Description/Expression/Renderer selection with
`SDL_VIDEODRIVER=offscreen` passed **76/76**. Both logs are retained; the clean gate proves the
live boundary but does not claim that CNA can decode or display these binaries.

No CNA or Sharp Runtime source change is justified by an asset pack that provides neither its
binary schema nor a runtime consumer. Reverse-engineering `STRB`, mapping it to a reusable
animation representation and rendering it with an authentic Xbox Avatar would be a new subsystem,
not a bounded sample repair.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-112-AvatarAnimPack_4_0_BIN/`.

- `xna4-original/` is a byte-for-byte complete upstream snapshot;
- `evidence/file-inventory.txt`, `sha256sum.txt` and `bin-headers.txt` record every file, size,
  identity and binary prefix;
- `evidence/ConvertFbxToBin.utf8.txt` is a readable transcoding of the unchanged UTF-16BE script;
- `evidence/snapshot-diff.txt` is empty after the reproducibility run;
- `evidence/cna-avatar-tests-default-display.log` preserves the environmental 74/76 run;
- `evidence/cna-avatar-tests-offscreen.log` preserves the passing 76/76 gate;
- `scripts/audit.sh` recreates the snapshot and deterministic inventory evidence from the exact
  upstream directory.

Native OPENGLES3 and WEBGL2 gates are not applicable because upstream ships no executable or
runtime product. They become mandatory if the owner authorizes a new viewer/product.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port/archive boundary for this exact licensed binary asset
   delivery, without pretending it is an XNA application;
2. explicitly classify SAMPLE-112 as retained support content for a future faithful Avatar
   backend, keeping the files and evidence but creating no standalone sample target; or
3. authorize a new reusable `STRB` importer and animation preview product, after supplying or
   approving the format contract and choosing the authentic Xbox Avatar versus deliberately
   substitute body scope; define OPENGLES3 and WEBGL2 behavior as an explicit modernization.

Until that decision, a decoder guessed from bytes, a generic skinned model, CNA's substitute
Avatar extension or a viewer assembled from SAMPLE-113 assets would all violate the zero-workaround
and source-fidelity rules.
