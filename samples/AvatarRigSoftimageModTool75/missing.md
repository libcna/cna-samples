# SAMPLE-118 — `AvatarRig_4_0_SoftImage_Mod_Tool7_5` audit and owner decision

## Current-head re-analysis — 2026-10-03

**Current status: 🛑, independent owner decision pending.** The owner explicitly instructed
"ponech 117 cancelled a analyzuj 118". 112–117 are individually owner-cancelled; that does not
classify this Softimage base rig or authorize a replacement authoring product.

All **128 files / 9,052,598 bytes** freshly match the physical upstream source and retained
snapshot by relative path, size and SHA-256. The 6,441,472-byte `Scenes/Avatar Rig.exp` retains
SHA-256 `d9a2d2adb606e5aeb2fbadf82904cc9c3b06da82bc2184d235e1950a6cc8a8d5`.
Every OLE stream was read and hashed without parsing defects: 30 streams and six storages,
including the 5,701,793-byte internal model. Both `7z l -slt` and `7z t -slt` pass for the
base and all 21 authored SAMPLE-115 scenes. Their named stream structure matches; authored
internal model streams range from 5,732,919 to 5,837,967 bytes. All retain the Softimage
`7.5.2009.0414::DemoVersion::Regular Save` marker; the base also retains 0409 metadata.
Embedded strings identify `XBLAvatarV2`, boy/girl meshes, Skin_Joints, joint/chain/IK controls,
MaleFemale, Fat_or_Thin and Fake_Tall_or_Short expressions. These were inspected as data,
not executed or treated as a decoded/evaluated complete Softimage object graph.

All **125 images decode**: 106 TGA, 13 TIFF and six PNG screenshots. Every picture and the
complete Documentation directory are byte-identical by path/hash to SAMPLE-115. The source
stream contains **46 unique picture specifications**, including seven numbered sequences;
expanding their stated ranges and padding gives exactly all 119 supplied Pictures files.
There are no missing files after rebasing these references to the project Pictures directory.
This proves intact source dependencies, not Softimage's actual path-resolution behavior.
No solution, project, content project, source code, executable or runtime entry point exists.

The entire original HTML workflow and all six screenshots were reviewed. Load through a new
Softimage project whose root contains Pictures/Scenes; directly opening the scene gives incorrect
texture paths. Select every Skin_Joints object, then Plot → All Transformations. The shown
PlotToActions settings are action `plot`, frames 0–37, step 1, Standard/Spline curves, rotation
continuity on, curve fitting off (disabled tolerance 0.01), apply to object on, paste keys rather
than replace curve on, delete plotted action on. The range is an authoring example, not a runtime
constant. Deselect bones and return to the starting frame to avoid end-frame popping; Crosswalk
→ Export FBX saves beside the scene. Crosswalk 3.3 has Geometry, Skin, Embed Textures, Export
Animation at 30 FPS, Export envelope deformer as skeleton and warnings/errors dialog enabled.
Selection Only, Shapes, Cameras, Lights, Convert to TIFF, ASCII and Keep XSI Effectors are disabled.

**The historical CNA no-op assessment is superseded.** At current CNA `next 75b55659c`, normal
AvatarDescription, preset animation, Ready/BindPose/ParentBones and Draw(bones, expression) work
on original CNA bodies. Completed SAMPLE-094 consumes genuine processor-built custom-animation
XNBs; its 64,822 original-player and 1,041 game assertions plus native/real-Chrome and post-prune
replay gates are retained qualifications at CNA 2b4ff28d7. Avatar source is unchanged between
that qualification and current HEAD. These are downstream consumer results, not newly run
Softimage exports or framework tests. Sharp remains `next db86514c`; neither dependency changed.

CNA also accepts whole compatible avatar catalog packs, with a validated 71-bone model contract
(`../cna/docs/avatars.md` → Catalog packs). AvatarRenderer takes an AvatarDescription, not an
arbitrary `.exp`/FBX/model file. This general catalog capability neither directly loads the
Microsoft rig nor establishes body conversion, Softimage control evaluation or an authentic
Crosswalk export. The approved CNA-body boundary of 094 does not authorize those new products.

**Authoring/export remains unqualified.** `xsi`, `xsibatch`, `softimage` and `crosswalk` are
absent from PATH, with no corresponding executables in the established XNA Wine prefix.
No authentic project/scene load, IK/skin/expression evaluation, editing, plotting or FBX export
was executed. The finished FBXs of 113 do not prove a new export from this base rig. Upstream
has no application to translate; a runtime OLE parser or invented viewer would not reproduce
the documented authoring product. No source/runtime implementation or native/browser gate
was introduced. No artifact pruning was requested.

Current evidence: `/rv/tmp/samples/SAMPLE-118-AvatarRig_4_0_SoftImage_Mod_Tool7_5/evidence/current-head-analysis-20261003/`.
It retains complete inventory/image/stream hashes, all 21 scene comparisons, texture expansions,
rig marker strings, the full HTML text, tool checks and manual screenshot review. Reproduce with
`python3 .../scripts/current-head-analysis-20261003.py`; this compares without replacing the
original snapshot. The earlier `audit.sh` and historical evidence remain retained.

**Owner options:** accept archival cancellation like 112–117 (recommended, all sources retained);
retain editable support data without a standalone target; or explicitly define a new DCC
migration/authoring product. The latter first needs an authentic Softimage/Crosswalk reference,
then accepted rig/control/skin/material/animation/export parity and any native/browser preview
scope. This audit does not establish a reliable implementation estimate for such a product.
`rules.md`, SAMPLES-DEC-004/005 reserves the choice to the owner; 118 stays 🛑 until decided.

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

The earlier runtime assessment (zero presets, Unavailable/no-op rendering and missing bind pose)
is superseded by the current-head evidence above. Standard CNA avatar rendering and completed
SAMPLE-094 already consume custom animations on CNA bodies, and compatible catalog packs are
supported. This closes the old runtime blocker while leaving Softimage authoring/export and
conversion of this Microsoft rig unqualified. SAMPLE-113 retains genuine processor-built XNB
evidence; a newly invented preview still requires explicit owner scope and normal native/web gates.

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
