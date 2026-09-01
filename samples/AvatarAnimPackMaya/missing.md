# SAMPLE-114 — `AvatarAnimPack_4_0_Maya` audit and owner decision

## Status

Fresh audit complete enough to require an owner representation decision under
`SAMPLES-DEC-004` and `SAMPLES-DEC-005`. This is a complete Autodesk Maya authoring pack, not an
XNA application. No game, viewer, modern-DCC conversion, raw-scene runtime loader or CNA substitute
Avatar was invented. Only the owner may accept an archival/support-data boundary or authorize a
modernized authoring product.

## Classification and complete inventory

The physical directory contains **252 files / 167,289,073 bytes**:

- 21 Maya ASCII 2009 scenes named `Climb`, `Crawl`, `Faint`, `Jump`, `Kick`, `PickUp`, `Pull`,
  `Punch`, `Push`, `Run`, `SitIdle`, `Steer`, `StrafeLeft`, `StrafeRight`, `SwimFreestyle`,
  `SwimUnderwater`, `SwingBat`, `SwingClub`, `SwingHammer`, `Throw` and `Walk`;
- 220 valid TGA textures: nine RGB and 211 RGBA images from 32x32 through 128x128;
- ten Maya swatch-cache files;
- the Microsoft Permissive License in RTF form.

There is no `.sln`, XNA/content project, C#/C++ source, entry point, readme, HTML page, build script
or runtime UI. Each `.ma` explicitly declares:

```text
//Maya ASCII 2009 scene
requires maya "2009";
requires "Mayatomr" "10.0.1.8m - 3.7.1.26 ";
currentUnit -l centimeter -a degree -t film;
fileInfo "product" "Maya Unlimited 2009";
fileInfo "version" "2009 x64";
```

All 21 scenes are structurally substantive and consistent rather than empty placeholders: each
contains 104 joints, 30 meshes and exactly one `BASE__Skeleton`; scene length is 179,042–181,316
text lines and animation data spans 202–238 `animCurve` nodes. The complete inventory and hashes
are retained. Representative scene identities are:

| Scene | SHA-256 |
|---|---|
| `Climb.ma` | `ec0e7452d93df21af1074cbf9f4bbb752e07755f50cc70fa2d0996e64f3bd2ec` |
| `Jump.ma` | `dd40cead403cf70d804fc20ae1054892b6147ed576024897ace9c619f0deed76` |
| `Run.ma` | `c1d2761ac81c3c44dded3297cf7d870d21398f240c89918763a7cdd79b787a52` |
| `Walk.ma` | `ad8aa9485f767bf4bfdfb16eb55f0abe009dbd28b289c9d2dd859bf193a06d7b` |

## Relationship to the FBX and XNA deliveries

The local Microsoft `CustomAvatarAnimation` documentation describes this exact workflow class:
authors use an Avatar-specific animation rig in Maya or Softimage, export FBX, then process that
FBX with the sample's custom content processor. All 21 Maya action names pair case-insensitively
and exactly with SAMPLE-113's 21 FBX names. The scenes contain the same `Avatar_Mesh` and
`BASE__Skeleton` naming expected by that pipeline.

This supports the classification as the editable DCC source companion, but it is not enough to
claim the shipped FBX bytes were generated from these exact scene revisions. Autodesk Maya 2009,
`mayapy` and its renderer are absent on the live host, and the offline Win7 XNA VM was provisioned
for Visual Studio/XNA rather than the proprietary Autodesk authoring suite. No unauthorised
software installation or internet access was attempted.

SAMPLE-113 independently proves that every paired FBX is valid: all 21 unchanged files pass the
official XNA 4.0 `FbxImporter` plus exact Microsoft custom-avatar processor and produce
deterministic Xbox360/HiDef XNB v5/LZX output. Therefore neither the DCC source inventory nor the
downstream XNA content path is missing. The unverified boundary is only the proprietary Maya
scene-to-FBX export step and the absence of any standalone product in this directory.

## CNA boundary

CNA is a runtime framework and intentionally has no Autodesk Maya authoring environment. Adding a
runtime `.ma` parser would violate the campaign's compiled-content policy and would not reproduce
Maya's rig editing, skinning, material, preview and FBX export workflows. It also would not create
an upstream game that does not exist.

If a runtime preview is later authorized, SAMPLE-113 already supplies authentic processor-built
XNB inputs. The remaining visual dependency is the same normal XNA Avatar boundary: current
presets are zero-length/zero-matrix, `AvatarRenderer` remains `Unavailable`, bind pose is
unavailable and normal `Draw` is a no-op. The current-HEAD focused gate passed 76/76 tests with
`SDL_VIDEODRIVER=offscreen`; the working real-render route is a deliberately non-authentic
`CNAEXT` substitute and cannot be selected silently.

No CNA or Sharp Runtime change was made. A Maya authoring product would be a deliberate tool/DCC
scope expansion, not a bounded runtime repair.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-114-AvatarAnimPack_4_0_Maya/`.

- `xna4-original/` is the complete byte-for-byte 252-file upstream snapshot;
- `evidence/file-inventory.txt` and `sha256sum.txt` cover every scene, texture, swatch and licence;
- `evidence/scene-headers.txt` records the Maya/Mental Ray/version/unit declarations;
- `evidence/scene-structure.tsv` records line, joint, animation-curve, mesh and root-skeleton counts
  for all 21 scenes;
- `evidence/texture-type-counts.txt` validates and groups all 220 TGA files;
- `evidence/ma-fbx-basename-pairing.txt` contains all 21 paired action names;
- `evidence/maya-tool-availability.txt` records the absent proprietary tools;
- `evidence/snapshot-diff.txt` is empty;
- `scripts/audit.sh` deterministically recreates all evidence above.

There is no original/native/browser runtime gate because the source contains no executable or XNA
consumer. A modern DCC export test requires an explicit scope decision; any newly authorized
runtime preview must pass normal OPENGLES3 and WEBGL2 gates.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port/archive boundary for this complete Maya 2009 authoring pack;
2. classify it as retained editable source/support data for SAMPLE-113, SAMPLE-094 or a future
   faithful Avatar backend, without inventing a standalone sample target; or
3. authorize an explicit modern-DCC migration/tool project, defining accepted Maya-to-FBX parity,
   rig/animation/material preservation and whether a separate OPENGLES3/WEBGL2 preview product is
   required.

Until that choice, converting the scenes with an unrelated parser, displaying only their TGAs or
wrapping the downstream XNBs in an invented game would violate source fidelity.
