# SAMPLE-129 — `LobbyChatImages` audit and owner decision

## Status

The complete asset, collection-wide consumer and content-pipeline audit is finished. This is a
licensed reusable lobby/chat icon delivery, not an XNA game, library, content project or executable
utility. No gallery, fake lobby, input visualizer or alias of an existing network sample was
invented. The owner must choose a resource-pack/non-port or support-data boundary under
`SAMPLES-DEC-005` before this physical upstream directory can be called resolved.

## Complete inventory

The entire upstream directory contains exactly **6 files / 68,746 bytes**:

- `chatAble.png` — a person/voice-available symbol;
- `chatMute.png` — a crossed-out voice symbol;
- `chatTalking.png` — a person with emitted voice waves;
- `local.png` — a house/local-player symbol;
- `ready.png` — a thumbs-up/ready symbol;
- `Microsoft Permissive License.rtf`.

All five images are 64×64, 8-bit RGBA, non-interlaced PNGs. They use monochrome shaded artwork and
transparent backgrounds. The 45,920-byte RTF is byte-identical to the Microsoft Permissive License
delivered with SAMPLE-120 `ButtonImages` and SAMPLE-123 `ControllerImages`.

There is no solution, project, content project, C#/VB/C++ source, entry point, executable, input
mapping, screen, scene, runtime UI or usage document. Consequently there is no authentic original
application run or separately defined native/WEBGL2 product to reproduce.

## Exact consumer audit

A SHA-256 scan of every other PNG in the complete upstream sample collection found exactly four
copies. SAMPLE-062 `NetRumble_4_0` contains byte-identical copies of:

| Pack asset | NetRumble asset | Relation |
|---|---|---|
| `chatAble.png` | `Content/Textures/chatAble.png` | exact |
| `chatMute.png` | `Content/Textures/chatMute.png` | exact |
| `chatTalking.png` | `Content/Textures/chatTalking.png` | exact |
| `ready.png` | `Content/Textures/ready.png` | exact |

NetRumble's content project compiles all four with `TextureImporter` / `TextureProcessor`.
`World.LoadContent` loads the same four identifiers. Its player-data draw selects mute, talking or
available according to `IsMutedByLocalUser`, `IsTalking` and `HasVoice`, and draws the ready icon
for `IsReady`; `NetworkBusyScreen` additionally reuses `chatTalking` as its animated busy icon.
These are real consumed assets, not coincidental same-name files.

`local.png` has no byte-identical copy and no source/project consumer anywhere else in the
collection. The only broad-name text hit, documentation image `NetworkPrediction/Local.png`, is a
different diagram and unrelated to this lowercase 64×64 icon. NetRumble represents local gamers
by pulsing their gamertag rather than using the house icon.

SAMPLE-075 `NGSMSample_4_0` demonstrates the same four lobby states, but owns distinct
`chat_able`, `chat_mute`, `chat_talking` and `chat_ready` files. All are 28×28, have different
hashes and are loaded under their underscored identifiers. They are semantic alternatives, not
copies or missing links to this pack. Both consuming games remain independently owner-blocked on
their already documented networking/platform boundaries; this asset audit does not resolve or
duplicate either game.

## Authentic XNA 4 and CNA content evidence

A retained `BuildContent` harness passed all five unchanged PNGs through Microsoft's official XNA
4.0 Windows/Reach `TextureImporter` → `TextureProcessor` path. The build returned true and emitted
five 16,571-byte `Texture2D` XNBs.

At live CNA HEAD `7712534d3d22`, `cna-content` consumed those exact XNBs through
`CNA.XnbImporter` → `CNA.TextureProcessor` → `CNA.Texture2DContentWriter`, producing five validated
CNBs with zero failures. The representative `chatAble` container reports type
`Microsoft.Xna.Framework.Graphics.Texture2D`, no external references and 16,384 bytes of texture
data, exactly 64×64×4. Nine focused OPENGLES3 XNB/content tests covering real loading,
cache/unload behavior, container variants and runtime-XNB/transcoded-CNB pixel equality pass.

This proves the ordinary PNG/Texture2D route exposes no CNA or Sharp Runtime defect. No framework
change, sample workaround or content substitution was needed. There is no runtime/browser gate for
the pack itself: constructing a viewer would test newly authored behavior rather than an upstream
product.

## Owner choice required

Choose one:

1. Accept an evidence-backed non-port/resource-pack boundary for this complete licensed delivery.
2. Retain it as shared source/support data, principally for NetRumble and potentially future lobby
   products, without inventing a standalone target. NetRumble already owns exact local copies, so
   deduplication must preserve provenance and cannot imply its network game is complete.
3. Authorize a new lobby-icon gallery/status visualizer and define its behavior plus native/WEBGL2
   acceptance criteria. This is an explicit new product, not a port inferred from these six files.

Do not claim SAMPLE-062 or SAMPLE-075 as this pack's executable, create a fake single-player lobby,
or discard the currently unused `local.png` merely because no shipped consumer was found.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-129-LobbyChatImages/`.

- `xna4-original/LobbyChatImages/` — complete byte-for-byte six-file snapshot;
- `evidence/file-inventory.txt`, `sha256sum.txt`, `image-metadata.tsv` and the empty
  `snapshot-diff.txt` — complete input identity;
- `evidence/all-other-png-sha256.txt` and `exact-copy-scan.tsv` — collection-wide hash proof;
- `evidence/netrumble-exact-consumer.tsv`, `consumer-reference-scan.txt` and
  `generic-name-reference-scan.txt` — exact code/content consumers and false broad-name hit;
- `evidence/network-state-semantic-icons.tsv` — distinct 28×28 SAMPLE-075 comparison;
- `evidence/license-comparison.txt` — exact shared Ms-PL RTF identity;
- `xna4-build/Content/` and `evidence/xna4-content-build.log` — five official XNA 4 XNBs;
- `cna-build/`, `cna-xnb-transcode.log` and `cna-texture-cnb-info.txt` — five validated CNA CNBs;
- `evidence/cna-focused-content-tests.log` — 9/9 focused OPENGLES3 tests;
- `scripts/XnaPipelineRunner.cs`, `build-content.sh`, `audit.sh` and `qualify.sh` — repeatable
  offline qualification, with CNA conversion capped at eight workers.

Re-run everything with:

```bash
/rv/tmp/samples/SAMPLE-129-LobbyChatImages/scripts/qualify.sh
```
