# SAMPLE-116 — `AvatarRig_4_0_Max_2010` audit and owner decision

## Status

Fresh audit complete enough to require an owner representation decision under
`SAMPLES-DEC-004` and `SAMPLES-DEC-005`. This is a documented Autodesk 3ds Max 2010 authoring rig,
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

If a runtime preview is later authorized, SAMPLE-113 already supplies authentic processor-built
XNB inputs. The remaining visual dependency is the normal XNA Avatar boundary: current normal CNA
presets are zero-length/zero-matrix, `AvatarRenderer` remains `Unavailable`, bind pose is unavailable
and normal `Draw` is a no-op. CNA's working `CNAEXT` renderer is a deliberately non-authentic
substitute and cannot be selected silently.

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
