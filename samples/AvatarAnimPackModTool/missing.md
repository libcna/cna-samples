# SAMPLE-115 — `AvatarAnimPack_4_0_Mod_Tool` audit and owner decision

## Owner cancellation — 2026-10-03

**Current status: ⛔.** The owner instructed **"ponech 115 cancelled"** after the current-head
re-analysis. SAMPLE-115 remains a retained non-port/archive under SAMPLES-DEC-004/005, consistently
with112–114. Preserve the complete Softimage scenes, images, documentation and audit evidence.
No standalone viewer, replacement authoring tool, scene conversion or export product is authorized.
The completed094 consumer does not reproduce Softimage/Crosswalk authoring or create an upstream
application. Next numbered sample:116.

## Current-head re-analysis — 2026-10-03

**Status at this re-analysis: 🛑; resolved by the owner cancellation above.** Fresh audit at CNA `next 2b4ff28d7`,
Sharp `next db86514c` and samples `develop 6585370`. The owner
has now cancelled112/113/114; their source and evidence remain archived. No sample target, viewer,
scene parser, content conversion or framework repair is added for115.

All **148 files / 139,968,950 bytes** still match the complete upstream snapshot. Every one of
21 EXP files is a valid OLE compound document with the Softimage
`7.5.2009.0414::DemoVersion::Regular Save` marker and InternalModel0/Objects data. All **125 images**
decode successfully:106 TGAs,13 TIFFs and six PNG instruction screenshots. The original HTML and
license are retained. All21 scene names still pair exactly with the FBX pack; matching names do
not prove export-byte identity. There is still no source project, entry point or runtime UI.

The complete original instructions were re-read; Plot All and Crosswalk screenshots were
visually checked. The workflow depends on a Softimage project with Pictures/Scenes, Skin_Joints
selection, Plot All Transformations, deselecting bones and returning to the first frame before
export. Crosswalk3.3 settings include geometry/skin, embedded textures, animation30FPS and envelope
deformers as skeleton; cameras/lights/shapes/ASCII/XSI effectors/TIFF conversion are off. The shown
plot defaults use standard spline curves, rotation continuity and the documented applied/using-
keys/delete action options. A raw OLE reader would not reproduce this authored/export behavior.
Softimage, xsi and xsibatch are absent; authentic scene-to-FBX export remains unqualified.

**The old CNA no-op Avatar blocker is obsolete.** Standard descriptions, custom71-bone matrices,
expressions, BindPose/ParentBones and drawing now work on CNA's original art. Owner-approved
SAMPLE-094 is complete and its unchanged custom player matches genuine Windows XNA in64,822
assertions, plus1,041 game assertions; native and exact-gallery WEBGL2 controls, movement, profile/
random avatar and clean Back pass. Separate empty-profile native/web runs preserve the original
blank-avatar start, Q with no player and E random selection. Current focused Guide/avatar130/130
passes. This supplies an existing animation consumer; it does not reproduce Softimage's authoring
and Crosswalk export or authorize adding the cancelled pack's assets to094.

Owner options remain:

1. **Cancel/archive115**, consistently with112/113/114, retaining its editable data/instructions
   and audit evidence (small documentation task; no upstream runtime gate exists).
2. **Retain explicit support materials** with no standalone target and a defined relationship to
   the existing094 consumer (small documentation task; scene export remains unverified).
3. **Authorize a replacement authoring/export project**, defining available tools, accepted EXP/
   FBX rig/skin/texture/animation parity and any preview requirement. This is a tool migration of
   undefined scope, not a small missing CNA API; a reliable effort estimate needs those decisions.

No CNA/Sharp source changed. Full current inventory/hash/image records and per-scene OLE listings
are in `/rv/tmp/samples/SAMPLE-115-AvatarAnimPack_4_0_Mod_Tool/evidence/current-head-analysis-20261003/`.
`scripts/current-head-analysis-20261003.py` reproduces the all-file, all-container, decoded-image,
basename-pairing and tool checks. Local recording only, no push or cleanup. Historical audit follows.

## Status

Fresh audit complete enough to require an owner representation decision under
`SAMPLES-DEC-004` and `SAMPLES-DEC-005`. This is a documented Autodesk Softimage Mod Tool 7.5
authoring pack, not an XNA application. No game, viewer, raw-scene loader, DCC conversion or CNA
substitute Avatar was invented. Only the owner may accept an archival/support-data boundary or
authorize a replacement authoring product.

## Classification and complete inventory

The physical directory contains **148 files / 139,968,950 bytes**:

- 21 Softimage `.exp` scenes named `Climb`, `Crawl`, `Faint`, `Jump`, `Kick`, `PickUp`, `Pull`,
  `Punch`, `Push`, `Run`, `SitIdle`, `Steer`, `StrafeLeft`, `StrafeRight`, `SwimFreestyle`,
  `SwimUnderwater`, `SwingBat`, `SwingClub`, `SwingHammer`, `Throw` and `Walk`;
- 106 valid TGA and 13 valid TIFF rig textures;
- six PNG documentation screenshots and one HTML instruction page;
- the Microsoft Permissive License in RTF form.

There is no `.sln`, project, XNA/content project, C#/C++ source, entry point or runtime UI. Every
6,472,704–6,578,688-byte scene is an OLE Compound Document with the exact header
`d0cf11e0a1b11ae1`, and every scene contains the marker:

```text
7.5.2009.0414::DemoVersion::Regular Save
```

The container is substantive rather than a renamed or opaque placeholder. For example,
`Climb.exp` exposes 30 compound-document streams in six folders, including layers, clips, sources,
properties and a 5,809,061-byte `InternalModel0/Objects` stream. UTF-16 strings also identify
`XBLAvatarV2`, scene objects and rig controls. The complete inventory and hashes are retained;
representative identities are:

| File | SHA-256 |
|---|---|
| `Scenes/Climb.exp` | `d07d43c86b8e4d11df60f42100b91be7c6585198430a5d405b62c0143896cf14` |
| `Scenes/Jump.exp` | `16a2c674c447c139d2f09fdecab380d1f5251446d18efb3a302ba9a6199a61b0` |
| `Scenes/Run.exp` | `877f184e7fa54413b46e0a1e66731aa2a621c3ea0df5cb62ddb9aa4dd51e9fe8` |
| `Scenes/Walk.exp` | `36ff98dd5903cc933345c600389bb13dfcb4a04c8262e8b0536819fb68ff5087` |
| `Documentation/readme.htm` | `1b2dcfc2bf8fb061f7d8fec63e507c19bd274099ba89869fddaaa20470446b63` |
| `Documentation/Crosswalk.png` | `83e3e8d6ceab6e7cb3717822b81526de7c1d673a9f530c2d0bc6aaf7eed432ad` |

## Original authoring and export contract

Unlike the Maya pack, this delivery documents its intended workflow. Softimage must open it as a
project whose root contains the supplied `Pictures` and `Scenes` directories; opening a scene
directly produces incorrect texture paths. To export an Avatar animation, the instructions require:

1. selecting every object in the `Skin_Joints` layer;
2. plotting all transformations with the documented default options;
3. deselecting the bones and moving the timeline to the animation's first frame to avoid end-frame
   popping;
4. using File → Crosswalk → Export FBX and saving beside the source `.exp` so texture paths remain
   correct;
5. enabling geometry, skin, embedded textures and animation at 30 FPS, exporting envelope
   deformers as a skeleton, and leaving the documented camera/light/shape/ASCII/effectors and
   TIFF-conversion options disabled.

The retained screenshot identifies Crosswalk 3.3 and records the complete export dialog. The old
download link targets the discontinued Premium XNA Creators Club distribution; it is evidence, not
a currently usable dependency source.

## Relationship to the FBX and XNA deliveries

All 21 scene basenames pair case-insensitively and exactly with SAMPLE-113's 21 FBX animations.
This and the explicit export instructions establish the intended source-to-FBX relationship, but
they do not prove that the shipped FBX bytes were generated from these exact scene revisions.
Softimage Mod Tool 7.5, `xsi` and `xsibatch` are absent on the live host, so the proprietary
scene-to-FBX step cannot be reproduced honestly. No internet access, unauthorised installation or
unrelated converter was used.

SAMPLE-113 independently proves the downstream content path: every paired unchanged FBX passes the
official XNA 4.0 `FbxImporter` and exact `CustomAvatarAnimationProcessor`, producing deterministic
Xbox360/HiDef XNB v5/LZX output. Therefore neither the delivered authoring data, the source FBX pack
nor the XNA content pipeline is missing. The remaining boundary is the retired Softimage/Crosswalk
authoring environment and the absence of a standalone product in this directory.

## Historical CNA boundary — superseded by the re-analysis above

CNA is a runtime framework and intentionally has no Softimage authoring environment. Adding a
runtime OLE/`.exp` parser would violate the campaign's compiled-content policy and would not
reproduce Softimage's rig editing, plotting, texture-path or Crosswalk export behavior. It also
would not create an upstream game that does not exist.

If a runtime preview is later authorized, SAMPLE-113 already supplies authentic processor-built
XNB inputs. The remaining visual dependency is the normal XNA Avatar boundary: current normal CNA
presets are zero-length/zero-matrix, `AvatarRenderer` remains `Unavailable`, bind pose is unavailable
and normal `Draw` is a no-op. CNA's working `CNAEXT` renderer is a deliberately non-authentic
substitute and cannot be selected silently.

No CNA or Sharp Runtime change was made. A replacement authoring application or modern-DCC
migration would be a deliberate product/scope expansion, not a bounded runtime repair.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-115-AvatarAnimPack_4_0_Mod_Tool/`.

- `xna4-original/` is the complete byte-for-byte 148-file upstream snapshot;
- `evidence/file-inventory.txt` and `sha256sum.txt` cover every scene, texture, screenshot,
  instruction and licence;
- `evidence/readme-text.txt` is a stable text rendering of the original HTML instructions;
- `evidence/scene-containers.tsv` records size, OLE header and Softimage version marker for all 21
  scenes;
- `evidence/climb-ole-streams.txt` records the representative compound-document structure;
- `evidence/image-type-counts.txt` validates and groups all 125 images;
- `evidence/exp-fbx-basename-pairing.txt` contains all 21 paired action names;
- `evidence/tool-availability.tsv` records the absent proprietary tools;
- `evidence/snapshot-diff.txt` is empty;
- `scripts/audit.sh` deterministically recreates all evidence above.

There is no original/native/browser runtime gate because the source contains no executable or XNA
consumer. An authentic export requires Softimage Mod Tool 7.5/Crosswalk; any newly authorized
runtime preview must pass normal OPENGLES3 and WEBGL2 gates.

## Historical owner options — resolved above

Choose one:

1. accept an evidence-backed non-port/archive boundary for this complete Softimage Mod Tool 7.5
   authoring pack;
2. classify it as retained editable source/support data for SAMPLE-113, SAMPLE-094 or a future
   faithful Avatar backend, without inventing a standalone sample target; or
3. authorize an explicit modern-DCC migration/tool project, defining accepted `.exp`/FBX parity,
   rig/animation/texture preservation and whether a separate OPENGLES3/WEBGL2 preview product is
   required.

Until that choice, displaying only the textures, wrapping the downstream XNBs in an invented game
or decoding OLE streams without reproducing the authoring/export contract would violate source
fidelity.
