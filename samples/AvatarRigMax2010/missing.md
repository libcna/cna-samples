# SAMPLE-116 — `AvatarRig_4_0_Max_2010` audit and owner decision

## Owner decision — cancelled, 2026-10-03

The owner explicitly instructed: **"ponech 116 cancelled a analyzuj 117"**.
SAMPLE-116 is therefore `⛔` (cancelled), consistent with the source-only authoring companions
112–115. The complete original Max rig, textures, documentation and all audit evidence remain
retained. No new authoring, exporter, parser or preview product is authorized, and no cleanup of
this artifact root was requested. Cancellation does not claim any Max load/export or runtime gate.
The current-head analysis below remains the evidence for this accepted non-port boundary;
its pending-decision language records the state before the owner's decision. Continue with117.

## Historical current-head re-analysis — 2026-10-03

**Current status: 🛑, owner decision pending.** SAMPLE-115 remains owner-cancelled, and the
owner explicitly asked to analyze this row after publishing and pruning 085/086/101/094.
No cancellation of 116 is inferred from the decisions on 112–115.

Every one of the **239 files / 14,100,444 bytes** freshly matches both the physical upstream
directory and the retained original snapshot. The 8,560,674-byte Max scene retains SHA-256
`fd70c875b43a0de144fd13fe24f56ea04f8407d37d345f0238641f5ccab75af8`.
Both `7z l -slt` and `7z t -slt` pass; the container has ten named streams, including an
8,110,461-byte Scene, ScriptedCustAttribDefs and FileAssetMetaData2, and retains the documented
Avatar skeleton/controller/boy/girl and biped/bones plugin markers. Container integrity and
markers establish substantive source data; they do **not** qualify loading or exporting in Max.
All 220 TGA textures decode (211 RGBA, nine RGB), all three PNG workflow screenshots decode,
both XML files parse and the Office theme ZIP/XML passes integrity checks. The 230 texture/swatch
SHA-256 multiset still exactly matches SAMPLE-114. There is no source, project, executable,
content project or standalone game in this directory.

The entire original HTML workflow and all three screenshots were reviewed again. They specify
adding the extracted texture directory to Max external-file paths, reopening the project and
exporting a newly authored animation through Autodesk FBX Export 2009.4: bake frames 0–100
step 1; animation/deformations/skins/morphs enabled; curves/point caches/cameras/lights disabled;
embedded media without TIFF conversion; geometry-as-bones enabled, split normals disabled;
centimeters, scale 1.0, Y-up and binary FBX200900. Show Warning Manager and Show UI are enabled.
Textures are explicitly optional for animation export/import into XNA. The upstream HTML's
`title`/MSHelp metadata incorrectly names Softimage Mod Tool 7.5; its visible heading, body,
scene and screenshots consistently identify Max 2010. This upstream documentation defect is
retained verbatim, not used to reclassify the source as a Softimage project.

**The old CNA no-op blocker is obsolete.** At CNA `next 2b4ff28d7`, normal CreateRandom,
31 preset clips, Ready/BindPose/ParentBones and Draw(bones, expression) work on CNA's own bodies.
Completed SAMPLE-094 is an actual original custom-animation consumer using seven official XNA
Windows/HiDef products. Its 64,822 original-player and 1,041 game assertions plus native and
real-Chrome gates passed; the post-prune native retest and genuine original Windows CPU reference
replay pass too. CNA's final Guide/avatar regression pair is 130/130. These retained consumer
results are evidence of the downstream runtime, not a newly executed Max export or a rerun of
framework tests for this source-only audit. Sharp remains `next db86514c`; no runtime source changed.

**The authoring/export boundary remains.** `3dsmax`, `3dsmaxcmd` and `3dsmaxbatch` are absent
from PATH and their executables are absent from the established XNA Wine prefix. No authentic
Max scene load, rig/control/modifier editing, newly authored animation or FBX export was executed.
SAMPLE-113's qualified finished FBXs do not prove an export from this particular base rig, and
SAMPLE-094's approved CNA body/proportion boundary does not authorize importing Xbox avatar bodies
or replacing Max with a new editor. This delivery is a reusable authoring rig, not the 21-action
animation pack of 113–115. No invented viewer or raw `.max` runtime loader was added.

Current evidence is under
`/rv/tmp/samples/SAMPLE-116-AvatarRig_4_0_Max_2010/evidence/current-head-analysis-20261003/`:
`inventory.json`, `images.json`, `metadata.json`, `summary.json`, `max-ole-{l,t}.txt`,
`readme-text.txt` and `review.json`. Reproduce the read-only checks with
`python3 .../scripts/current-head-analysis-20261003.py`; the script compares rather than replaces
the historical snapshot. The retained earlier evidence below remains valid. No native/browser
application qualification applies to this source delivery; a new preview would need both gates.

**Owner options:** accept ⛔ archival cancellation like 112–115 (recommended, with all sources
retained); retain editable support data without a standalone port; or explicitly authorize a new
DCC migration/authoring product. The last option first needs a working authentic Max/export
reference, then defined rig/control/animation/texture parity and native/browser scope. The audit
does not establish a reliable implementation estimate for that new project. The authoring scope
decision is required by `rules.md` under SAMPLES-DEC-004/005.

## Status

`⛔` — cancelled by the explicit owner decision above under `SAMPLES-DEC-004`/`005`.
The previous analysis established the authoring/export boundary. This is a documented Autodesk 3ds Max 2010 authoring rig,
not an XNA application. No game, viewer, raw-scene loader, DCC conversion or CNA substitute Avatar
was invented. Only the owner may accept an archival/support-data boundary or authorize a
replacement authoring product.

## Classification and complete inventory

The physical directory contains **239 files / 14,100,444 bytes**:

- one 8,560,674-byte `Avatar Rig Max 2010.max` scene;
- 220 valid TGA rig textures and ten Maya swatch-cache files;
- three PNG documentation screenshots, two XML metadata files and one Office theme file;
- one HTML instruction page and the Microsoft Permissive License in RTF form.

There is no `.sln`, project, XNA/content project, C#/C++ source, entry point or runtime UI. The
`.max` scene is an OLE Compound Document with exact header `d0cf11e0a1b11ae1` and ten top-level
streams. Its 8,110,461-byte `Scene` stream and embedded markers including `Avatar_Skeleton0`,
`Avatar_CTRL0`, `Avatar_Mesh_boy0`, `Avatar_Mesh_girl0`, `bonesDef.DLM`, `biped.dlc` and skin/custom
attribute definitions establish a substantive editable Avatar rig rather than an empty container.
The source-scene SHA-256 is:

```text
fd70c875b43a0de144fd13fe24f56ea04f8407d37d345f0238641f5ccab75af8
```

All 220 TGA files plus all ten swatches form the exact same 230-entry SHA-256 multiset as the
texture/swatch portion of SAMPLE-114's audited Maya animation pack. Paths differ because this rig
nests the assets below `Textures/Animation/SupportingTextures`, but no texture payload was silently
recreated or substituted.

## Original authoring and export contract

The supplied documentation identifies Autodesk 3ds Max 2010 as the required editor. If the scene
cannot locate textures, it instructs the author to add the extracted `Textures` directory to Max's
external-file paths and reopen the project. It explicitly notes that textures are not required for
animation export/import into XNA Game Studio.

For an animation suitable for a game, the author uses File → Export, selects Autodesk FBX and
applies the retained FBX Export 2009.4 dialog settings:

- Animation and Bake Animations enabled, with start 0, end 100 and step 1;
- Deformations, Skins and Morphs enabled;
- curve filters, point-cache files, cameras and lights disabled;
- embedded media enabled without TIFF conversion;
- split per-vertex normals disabled and geometry-used-as-bones conversion enabled;
- scale factor 1.0, centimeters and Y-up;
- binary FBX version `FBX200900`.

The original page says export warnings may safely be ignored. This defines the intended authoring
contract; it does not make the proprietary Max exporter reproducible on the current host.

## Relationship to the validated Avatar content path

This delivery is a single reusable base rig for creating animations, not another copy of the 21
finished actions in SAMPLE-113–SAMPLE-115. The identical texture multiset connects it to the same
Microsoft Avatar authoring data, while its documentation explicitly targets FBX suitable for XNA
Game Studio.

SAMPLE-113 independently proves the downstream content path for finished Avatar FBX: all 21
unchanged animations pass the official XNA 4.0 `FbxImporter` and exact
`CustomAvatarAnimationProcessor`, producing deterministic Xbox360/HiDef XNB v5/LZX output. That
evidence confirms the FBX-to-XNA side, but it does not prove an export from this particular blank
rig. Autodesk 3ds Max 2010, `3dsmax`, `3dsmaxcmd` and `3dsmaxbatch` are absent on the live host. No
internet access, unauthorized installation or unrelated converter was used.

## CNA boundary

CNA is a runtime framework and intentionally has no 3ds Max authoring environment. Adding a
runtime OLE/`.max` parser would violate the campaign's compiled-content policy and would not
reproduce Max's rig controls, biped/skin modifiers, animation editing or Autodesk FBX export. It
also would not create an upstream game that does not exist.

The original 2026-09-01 runtime assessment (zero presets, Unavailable/no-op renderer and missing
bind pose) is superseded by the current-head evidence above: normal CNA custom-animation rendering
and SAMPLE-094 now work on CNA's own bodies. This closes the runtime blocker while leaving Max
authoring/export unqualified. SAMPLE-113's pipeline-built XNBs are retained downstream evidence;
using them in a newly invented preview would still require explicit owner scope.

No CNA or Sharp Runtime change was made. A replacement authoring application or modern-DCC
migration would be a deliberate product/scope expansion, not a bounded runtime repair.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-116-AvatarRig_4_0_Max_2010/`.

- `xna4-original/` is the complete byte-for-byte 239-file upstream snapshot;
- `evidence/file-inventory.txt` and `sha256sum.txt` cover every scene, texture, swatch,
  documentation support file and licence;
- `evidence/readme-text.txt` is a stable text rendering of the original HTML instructions;
- `evidence/scene-container.tsv`, `max-ole-streams.txt` and `max-scene-markers.txt` record the OLE
  structure and representative rig identity;
- `evidence/image-type-counts.txt` validates and groups all 233 images;
- `evidence/max-texture-hashes.txt` and `maya-pack-texture-hashes.txt` contain the compared 230-entry
  multisets, while `texture-hash-multiset-diff.txt` is empty;
- `evidence/tool-availability.tsv` records the absent proprietary tools;
- `evidence/snapshot-diff.txt` is empty;
- `scripts/audit.sh` deterministically recreates all evidence above.

There is no original/native/browser runtime gate because the source contains no executable or XNA
consumer. An authentic export requires 3ds Max 2010 and its FBX 2009 exporter; any newly authorized
runtime preview must pass normal OPENGLES3 and WEBGL2 gates.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port/archive boundary for this complete 3ds Max 2010 authoring
   rig;
2. classify it as retained editable source/support data for the validated Avatar animation packs or
   a future faithful Avatar backend, without inventing a standalone sample target; or
3. authorize an explicit modern-DCC migration/tool project, defining accepted `.max`/FBX parity,
   rig/control/animation/texture preservation and whether a separate OPENGLES3/WEBGL2 preview
   product is required.

Until that choice, displaying only the textures, wrapping other animation XNBs in an invented game
or decoding OLE streams without reproducing the authoring/export contract would violate source
fidelity.
