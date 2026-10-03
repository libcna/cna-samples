# SAMPLE-117 — `AvatarRig_4_0_Maya_2009` audit and owner decision

## Owner decision — cancelled, 2026-10-03

The owner explicitly instructed: **"ponech 117 cancelled a analyzuj 118"**.
SAMPLE-117 is therefore `⛔` (cancelled). The complete 234-file Maya rig, textures,
documentation and all audit evidence remain retained. No new authoring, exporter, parser
or preview product is authorized, and no artifact cleanup was requested. This accepts the
source-only non-port boundary; it does not claim successful Maya loading, editing or export.
The analysis below records the evidence and the pending options before this decision.

CNA can render its own compatible avatar catalogs and consume custom animations, as verified
by SAMPLE-094. That capability does not directly import this Microsoft `.ma` rig or reproduce
Maya's authoring/export environment. Cancellation of this authoring delivery does not cancel
CNA's general avatar capabilities. Continue with an independent analysis of SAMPLE-118.

## Historical current-head re-analysis — 2026-10-03

**Status at analysis: 🛑, independent owner decision pending.** The owner explicitly cancelled 116
and requested analysis 117. Cancellation of 112–116 does not decide this row.

All **234 files / 12,991,133 bytes** freshly match the physical upstream source and retained
snapshot. The 7,618,517-byte Maya ASCII scene retains SHA-256
`411a924fd9801c30b5bc5c9de0bcb9dd1d2818c110b3e95211f1384d04e41a71` and all 178,173 lines.
Its header requires Maya 2009 and Mayatomr 10.0.1.8m/3.7.1.26, records Maya Unlimited
2009 x64 as the authoring product, and uses centimeters/degrees/film. Complete node declarations and connections were inspected as data:
985 nodes, 104 joints, 30 meshes, 15 skinClusters, 18 IK handles, 93 expressions, two embedded
UI/playback script nodes and 4,938 connections. Embedded MEL was not executed. The six curve
inputs all come from the left/right foot controls' FullFootRoll attributes; this is a base
authoring rig, not the 21 finished clips. The saved playback range is 1–24, animation range 1–48.

The named 104-joint hierarchy (names and parents) exactly matches all 21 authored Maya scenes
in SAMPLE-114. Those retain 30 meshes and 202–238 curves each. All 220 TGA images decode
(211 RGBA/nine RGB), as does the FBX exporter PNG. All 230 texture/swatch hashes match both
114 and 116; all 57 scene texture references resolve to 34 retained relative files. This proves
source continuity and intact referenced assets, not correct editor behavior or a successful export.
There is no project, source code, content project, executable or standalone runtime UI.

The whole original HTML workflow and exporter screenshot were reviewed again. Export All must
save beside the scene for relative texture paths. FBX Exporter 2009.2 for Maya 2009 uses animation,
baking 1–48 step 1, deformations/skins/blend shapes, curve filters and Constant Key Reducer enabled.
Reducer precisions are translation 0.0001, rotation 0.0090, scaling 0.0040, other 0.0090; auto-tangents
only is enabled. Smoothing groups, split normals, conversion to null objects, geometry cache,
constraints, character definition, cameras/lights and TIFF conversion are disabled. NURBS stays
NURBS, quaternion mode is Resample As Euler Interpolation, media is embedded, scale 1.0 in
centimeters, Y-up, binary FBX200900. The shown frame range is authoring state, not a runtime constant.

**The historical CNA no-op assessment is superseded.** Current CNA `next 75b55659c` implements
the standard avatar API on CNA's own bodies, including valid descriptions, presets, Ready,
BindPose/ParentBones and Draw(bones, expression). Completed SAMPLE-094 consumes original custom
animations using exact official XNA XNBs; its 64,822 original-player and 1,041 game assertions,
native/real-Chrome gates and post-prune native/original-player replay already passed. These are
retained consumer qualifications at CNA 2b4ff28d7, not newly run Maya-export or framework gates.
The intervening CNA commit changes GameWindow and its tests, not avatar code. Sharp remains
`next db86514c`; no CNA/Sharp/runtime/sample implementation was changed by this source-only audit.

**Maya authoring/export remains unqualified.** Maya, mayapy and Render are absent from PATH
and their executables are absent from the established XNA Wine prefix. No authentic scene load,
IK/skin/expression/UI evaluation, animation editing or FBX export was executed. SAMPLE-113's
qualified downstream FBXs do not establish a fresh export from this rig. A runtime ASCII loader
or newly invented preview would not reproduce the authoring product. Existing 094 does not
authorize importing Xbox avatar bodies or replacing Maya with a new DCC tool.

Evidence: `/rv/tmp/samples/SAMPLE-117-AvatarRig_4_0_Maya_2009/evidence/current-head-analysis-20261003/`
holds inventory/image hashes, `base-scene.json`, `rig-details.json`, all 21 scene comparisons,
`maya-header.txt`, `readme-text.txt`, `summary.json` and `review.json`. Reproduce with
`python3 .../scripts/current-head-analysis-20261003.py`; it compares the complete snapshot rather
than overwriting it. The earlier audit evidence and helper remain retained. No artifact pruning.

**Owner options:** archival cancellation like 112–116 (recommended, retaining all sources);
retain editable support data without a standalone target; or explicitly scope a new DCC migration/
authoring product, first obtaining an authentic Maya 2009/FBX reference and defining rig/control/
animation/material parity plus any native/browser preview. No reliable estimate for that new
product is established by this source audit. `rules.md` SAMPLES-DEC-004/005 reserves the choice
to the owner; 117 stays 🛑 until decided.

## Status

`⛔` — cancelled by the explicit owner decision above under `SAMPLES-DEC-004`/`005`.
The fresh audit established the authoring/export boundary. This is a documented Autodesk Maya 2009 authoring rig,
not an XNA application. No game, viewer, raw-scene loader, DCC conversion or CNA substitute Avatar
was invented. Only the owner may accept an archival/support-data boundary or authorize a
replacement authoring product.

## Classification and complete inventory

The physical directory contains **234 files / 12,991,133 bytes**:

- one 7,618,517-byte `Avatar Rig Maya 2009.ma` scene;
- 220 valid TGA rig textures and ten Maya swatch-cache files;
- one PNG export-settings screenshot, one HTML instruction page and the Microsoft Permissive
  License in RTF form.

There is no `.sln`, project, XNA/content project, C#/C++ source, entry point or runtime UI. The
scene identifies its exact authoring contract:

```text
//Maya ASCII 2009 scene
requires maya "2009";
requires "Mayatomr" "10.0.1.8m - 3.7.1.26 ";
currentUnit -l centimeter -a degree -t film;
fileInfo "product" "Maya Unlimited 2009";
fileInfo "version" "2009 x64";
```

It is a substantive editable base rig: 178,173 lines, 104 joints, 30 meshes, one
`BASE__Skeleton` root and six animation-curve nodes. Its SHA-256 is:

```text
411a924fd9801c30b5bc5c9de0bcb9dd1d2818c110b3e95211f1384d04e41a71
```

All 220 TGA files plus all ten swatches form the exact same 230-entry SHA-256 multiset as the
texture/swatch portion of SAMPLE-114's audited Maya animation pack. The scene comparison establishes
the complementary roles: every one of SAMPLE-114's 21 finished scenes also has exactly 104 joints,
30 meshes and one `BASE__Skeleton`, but has 202–238 animation curves rather than the base rig's six.
This is measurable authoring-source continuity, not a classification based only on filenames.

## Original authoring and export contract

The supplied page identifies Autodesk Maya 2009 and the base scene by name. After creating an
animation, the author uses Export All, saves beside the `.ma` scene so texture paths remain correct,
selects FBX and applies the retained FBX Exporter 2009.2 settings:

- Animation and Bake Animations enabled, with start 1, end 48 and step 1 in the shown rig state;
- Deformations, Skins and Blend Shapes enabled;
- Curve Filters and Constant Key Reducer enabled, with the documented translation/rotation/scaling
  tolerances and auto-tangents-only behavior;
- geometry cache, constraints, character definition, cameras and lights disabled;
- embedded media enabled without TIFF conversion;
- scale factor 1.0, centimeters and Y-up;
- binary FBX version `FBX200900`.

This defines the intended authoring/export path. The frame range naturally changes with a newly
authored animation; it is not a runtime contract that CNA should hardcode.

## Relationship to the validated Avatar content path

This delivery is the reusable unanimated Maya source from which authors create clips. SAMPLE-114 is
the corresponding 21-scene authored animation pack, and SAMPLE-113 is its matching 21-FBX source
delivery. SAMPLE-113 independently proves the downstream path: every unchanged FBX passes the
official XNA 4.0 `FbxImporter` and exact `CustomAvatarAnimationProcessor`, producing deterministic
Xbox360/HiDef XNB v5/LZX output.

That evidence confirms the finished FBX-to-XNA side but does not prove a fresh export from this
particular base scene. Autodesk Maya Unlimited 2009, Mayatomr, `maya`, `mayapy` and `Render` are
absent on the live host. No internet access, unauthorized installation or unrelated converter was
used.

## CNA boundary

CNA is a runtime framework and intentionally has no Maya authoring environment. Adding a runtime
`.ma` parser would violate the campaign's compiled-content policy and would not reproduce Maya's rig
controls, skinning, animation editing or Autodesk FBX export. It also would not create an upstream
game that does not exist.

The old runtime assessment (zero presets, Unavailable/no-op renderer and missing bind pose)
is superseded by the current-head evidence above: standard CNA avatar APIs and completed 094
already consume custom animations on CNA's own bodies. This closes that runtime blocker while
leaving Maya authoring/export unqualified. SAMPLE-113 retains authentic processor-built XNB
evidence; a newly invented runtime preview would still need explicit owner scope.

No CNA or Sharp Runtime change was made. A replacement authoring application or modern-DCC
migration would be a deliberate product/scope expansion, not a bounded runtime repair.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-117-AvatarRig_4_0_Maya_2009/`.

- `xna4-original/` is the complete byte-for-byte 234-file upstream snapshot;
- `evidence/file-inventory.txt` and `sha256sum.txt` cover the scene, every texture/swatch,
  documentation and licence;
- `evidence/readme-text.txt` is a stable text rendering of the original HTML instructions;
- `evidence/maya-header.txt` preserves the exact Maya, Mayatomr, unit and source-platform contract;
- `evidence/rig-animation-structure.tsv` compares the base scene with all 21 SAMPLE-114 scenes;
- `evidence/image-type-counts.txt` validates and groups all 221 images;
- `evidence/rig-texture-hashes.txt` and `animation-pack-texture-hashes.txt` contain the compared
  230-entry multisets, while `texture-hash-multiset-diff.txt` is empty;
- `evidence/tool-availability.tsv` records the absent proprietary tools;
- `evidence/snapshot-diff.txt` is empty;
- `scripts/audit.sh` deterministically recreates all evidence above.

There is no original/native/browser runtime gate because the source contains no executable or XNA
consumer. An authentic export requires Maya 2009 and its FBX 2009.2 exporter; any newly authorized
runtime preview must pass normal OPENGLES3 and WEBGL2 gates.

## Historical owner options — resolved by cancellation above

Choose one:

1. accept an evidence-backed non-port/archive boundary for this complete Maya 2009 authoring rig;
2. classify it as retained editable source/support data for SAMPLE-113/SAMPLE-114 or a future
   faithful Avatar backend, without inventing a standalone sample target; or
3. authorize an explicit modern-DCC migration/tool project, defining accepted `.ma`/FBX parity,
   rig/control/animation/texture preservation and whether a separate OPENGLES3/WEBGL2 preview
   product is required.

Until that choice, displaying only the textures, wrapping other animation XNBs in an invented game
or treating ASCII parseability as a replacement for Maya's authoring/export behavior would violate
source fidelity.
