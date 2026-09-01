# SAMPLE-118 — `AvatarRig_4_0_SoftImage_Mod_Tool7_5` audit and owner decision

## Status

Fresh audit complete enough to require an owner representation decision under
`SAMPLES-DEC-004` and `SAMPLES-DEC-005`. This is a documented Autodesk Softimage Mod Tool 7.5
authoring rig, not an XNA application. No game, viewer, raw-scene loader, DCC conversion or CNA
substitute Avatar was invented. Only the owner may accept an archival/support-data boundary or
authorize a replacement authoring product.

## Classification and complete inventory

The physical directory contains **128 files / 9,052,598 bytes**:

- one 6,441,472-byte `Scenes/Avatar Rig.exp` scene;
- 106 valid TGA and 13 valid TIFF rig textures;
- six PNG documentation screenshots and one HTML instruction page;
- the Microsoft Permissive License in RTF form.

There is no `.sln`, project, XNA/content project, C#/C++ source, entry point or runtime UI. The
`.exp` scene is an OLE Compound Document with exact header `d0cf11e0a1b11ae1`, 30 streams in six
folders and the exact marker:

```text
7.5.2009.0414::DemoVersion::Regular Save
```

Its 5,701,793-byte `InternalModel0/Objects` stream and embedded `XBLAvatarV2`, `Avatar Rig
v2-Fixed`, joint, chain, IK, body-proportion and controller expressions establish a substantive
editable rig rather than an empty container. The scene SHA-256 is:

```text
d9a2d2adb606e5aeb2fbadf82904cc9c3b06da82bc2184d235e1950a6cc8a8d5
```

## Measured relationship to SAMPLE-115

This base-rig delivery and SAMPLE-115's 21 finished Softimage animation scenes are a directly
matched authoring set:

- their complete `Documentation` directories are byte-for-byte identical;
- all 119 files in `Pictures` are byte-for-byte identical;
- the base and all 21 animation scenes are OLE Compound Documents with 30 streams in six folders,
  and all carry the same Softimage 7.5 version marker;
- the base rig's internal model stream is 5,701,793 bytes, while the 21 authored scenes range from
  5,732,919 through 5,837,967 bytes.

These facts establish the base-versus-authored-scene roles without claiming that arbitrary OLE
stream differences can be interpreted outside Softimage.

## Original authoring and export contract

Softimage must open the delivery as a project whose root contains the supplied `Pictures` and
`Scenes` directories; opening the `.exp` directly produces incorrect texture paths. To export an
animation, the retained instructions require:

1. selecting every object in the `Skin_Joints` layer;
2. plotting all transformations with the documented default options;
3. deselecting the bones and moving the timeline to the animation's first frame to avoid end-frame
   popping;
4. using File → Crosswalk → Export FBX and saving beside the `.exp` source;
5. enabling geometry, skin, embedded textures and animation at 30 FPS, exporting envelope
   deformers as a skeleton, and leaving the documented camera/light/shape/ASCII/effectors and
   TIFF-conversion options disabled.

The retained dialog identifies Crosswalk 3.3. The old Premium XNA Creators Club download link is
historical evidence, not a currently usable dependency source.

## Relationship to the validated Avatar content path

This delivery is the reusable clean Softimage source from which authors create clips. SAMPLE-115
contains the corresponding 21 authored `.exp` scenes, and SAMPLE-113 contains the matching 21 FBX
deliveries. SAMPLE-113 independently proves the downstream path: every unchanged FBX passes the
official XNA 4.0 `FbxImporter` and exact `CustomAvatarAnimationProcessor`, producing deterministic
Xbox360/HiDef XNB v5/LZX output.

That evidence confirms the finished FBX-to-XNA side but does not reproduce a fresh Crosswalk
export from this base rig. Softimage Mod Tool 7.5, `xsi`, `xsibatch` and `softimage` are absent on
the live host. No internet access, unauthorized installation or unrelated converter was used.

## CNA boundary

CNA is a runtime framework and intentionally has no Softimage authoring environment. Adding a
runtime OLE/`.exp` parser would violate the campaign's compiled-content policy and would not
reproduce Softimage's rig controls, plotting, texture-path or Crosswalk export behavior. It also
would not create an upstream game that does not exist.

If a runtime preview is later authorized, SAMPLE-113 already supplies authentic processor-built
XNB inputs. The remaining visual dependency is the normal XNA Avatar boundary: current normal CNA
presets are zero-length/zero-matrix, `AvatarRenderer` remains `Unavailable`, bind pose is unavailable
and normal `Draw` is a no-op. CNA's working `CNAEXT` renderer is a deliberately non-authentic
substitute and cannot be selected silently.

No CNA or Sharp Runtime change was made. A replacement authoring application or modern-DCC
migration would be a deliberate product/scope expansion, not a bounded runtime repair.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-118-AvatarRig_4_0_SoftImage_Mod_Tool7_5/`.

- `xna4-original/` is the complete byte-for-byte 128-file upstream snapshot;
- `evidence/file-inventory.txt` and `sha256sum.txt` cover every scene, texture, screenshot,
  instruction and licence;
- `evidence/readme-text.txt` is a stable text rendering of the original HTML instructions;
- `evidence/rig-ole-streams.txt` records the representative compound-document structure;
- `evidence/rig-animation-containers.tsv` compares the base with all 21 SAMPLE-115 scenes;
- `evidence/image-type-counts.txt` validates and groups all 125 images;
- `evidence/rig-picture-hashes.txt` and `animation-pack-picture-hashes.txt` contain the compared
  119-entry multisets, while `picture-hash-multiset-diff.txt` is empty;
- `evidence/documentation-diff.txt` and `snapshot-diff.txt` are empty;
- `evidence/tool-availability.tsv` records the absent proprietary tools;
- `scripts/audit.sh` deterministically recreates all evidence above.

There is no original/native/browser runtime gate because the source contains no executable or XNA
consumer. An authentic export requires Softimage Mod Tool 7.5/Crosswalk; any newly authorized
runtime preview must pass normal OPENGLES3 and WEBGL2 gates.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port/archive boundary for this complete Softimage Mod Tool 7.5
   authoring rig;
2. classify it as retained editable source/support data for SAMPLE-113/SAMPLE-115 or a future
   faithful Avatar backend, without inventing a standalone sample target; or
3. authorize an explicit modern-DCC migration/tool project, defining accepted `.exp`/FBX parity,
   rig/control/animation/texture preservation and whether a separate OPENGLES3/WEBGL2 preview
   product is required.

Until that choice, displaying only the textures, wrapping other animation XNBs in an invented game
or decoding OLE streams without reproducing the authoring/export contract would violate source
fidelity.
