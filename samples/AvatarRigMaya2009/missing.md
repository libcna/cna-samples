# SAMPLE-117 — `AvatarRig_4_0_Maya_2009` audit and owner decision

## Status

Fresh audit complete enough to require an owner representation decision under
`SAMPLES-DEC-004` and `SAMPLES-DEC-005`. This is a documented Autodesk Maya 2009 authoring rig,
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

If a runtime preview is later authorized, SAMPLE-113 already supplies authentic processor-built
XNB inputs. The remaining visual dependency is the normal XNA Avatar boundary: current normal CNA
presets are zero-length/zero-matrix, `AvatarRenderer` remains `Unavailable`, bind pose is unavailable
and normal `Draw` is a no-op. CNA's working `CNAEXT` renderer is a deliberately non-authentic
substitute and cannot be selected silently.

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

## Owner decision required

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
