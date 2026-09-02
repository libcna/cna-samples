# SAMPLE-131 — `Minjie_ARCHIVE_2_0` audit and owner decision

## Status

Fresh audit complete enough to require an older-XNA product decision under `SAMPLES-DEC-005`.
Minjie is a complete, independently runnable XNA 2.0 strategy game. No audio-free C++ port, loose-WAV
replacement or unapproved XNA4 modernization was introduced merely to move the queue.

## Complete product inventory

The upstream delivery contains 56 files / 10,531,402 bytes:

- Windows and Xbox 360 XNA 2.0 solutions/projects;
- nineteen C# files / 3,311 lines, including the board, local and AI players, screen manager, arc
  camera and XACT-backed audio manager;
- one content project declaring fourteen visual items and one XACT 2 project;
- six FBX models and twelve source images. Model dependency processing makes two TGA textures
  independent runtime items, so the unchanged XNA4 diagnostic emits sixteen visual XNBs;
- nine stereo PCM WAV sources and a 1,040-line `minjie.xap` authoring graph;
- HTML design/control documentation, application artwork and the Microsoft Permissive License.

The documentation calls this a complete minigame ready to compile and run. It is a 10×10
Reversi/Othello-style product: each legal placement surrounds and flips an opposing line, turns
alternate, a player with no legal move passes, and the game ends on a full board or after both
players pass. The result screen selects player-one, player-two or tied artwork from the final
piece counts.

## Audited behavior

The title screen selects one-player-versus-AI, two local players or exit. The first four board
pieces use the normal Reversi layout. A legal move starts a ten-frame falling-piece animation;
the authored `Drop` cue plays when it lands, while an illegal attempt plays `Drop_Illegal`.

The AI chooses random valid moves until 25 pieces are on the board, then performs depth-three
minimax with a 7,000-node limit. Its heuristic is the difference between the two piece counts and
retains a random component until 90 pieces are present. The six models draw tiles, highlighted
selection and standing/on-board variants for both colours through BasicEffect lighting. A movable,
resettable and zoomable arc camera determines cursor-relative board navigation.

| Action | Keyboard | Gamepad |
|---|---|---|
| move player 1 / menus | W, S, A, D | left thumbstick |
| move player 2 | I, H, K, L | left thumbstick |
| place player 1 / accept | Space | A |
| place player 2 | Enter | A |
| orbit camera | Arrow keys | right thumbstick |
| reset camera | R | right-stick press |
| zoom in / out | Z / X | right / left trigger |
| exit | Escape | Back |

The XACT authoring graph is behavioral content, not a bag of interchangeable sound files. Its nine
waves feed eight sounds and eight cues: `Navigate`, `Drop`, `Menu_Accept`, `Menu_Scroll`,
`Music_Game`, `Music_Title`, `Drop_Illegal` and `Music_Win`. It contains authored pitch/volume and
variation settings. The Music category is limited to one instance with a 1,500 ms fade-out, and
`Music_Game` uses loop count 255 over a WAV carrying an explicit loop region.

## Authentic reference boundary

The archive ships no executable, XNB, XGS, XSB or XWB output. The available offline Wine and Win7
reference installation is XNA 4.0; no local XNA 2.0 content pipeline/XACT builder has been found.
The exact XNA 2 game therefore cannot yet be built or captured authentically. The three retained
collection thumbnails document the intended title, board and result presentation, but are not
substituted for executable reference evidence.

## Measured XNA4/CNA migration boundary

All nineteen unchanged sources were compiled diagnostically against the official XNA 4 Windows
assemblies. They reach exactly one error: XNA 2's
`GraphicsDevice.RenderState.DepthBufferEnable = true`. The XNA4-shaped replacement is the bounded
`DepthStencilState.Default` state assignment; no missing board, AI, screen, input, model, camera or
audio API was inferred from the compiler result.

The official XNA 4 Windows/HiDef Content Pipeline successfully builds every unchanged non-XACT
asset, producing sixteen valid XNBs. Live CNA at `7712534d3d22` converts all sixteen to validated
CNBs with zero failures. The same official pipeline rejects the exact `minjie.xap` with:

```text
The .xap file was created with a version of XACT that is incompatible with the XNA Framework
Content Pipeline version used by this project.
```

At that CNA HEAD, all 256 focused `XactParser`/`AudioEngine`/`WaveBank`/`SoundBank`/`Cue` tests and
all nine focused Texture2D/Model XNB tests pass. This proves the current XNA4-shaped runtime and
the successfully migrated visual content; it cannot prove compatibility with XACT2 banks that do
not yet exist. Omitting the cue graph or replacing it with direct loose-WAV playback would alter
music transitions, looping, category limiting, variations and the public XACT usage.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-131-Minjie_ARCHIVE_2_0/`.

- `xna2-original/` is the complete byte-for-byte upstream snapshot;
- `evidence/file-inventory.tsv`, `sha256sum.txt`, `image-metadata.tsv`,
  `model-metadata.tsv`, `audio-metadata.tsv`, `readme-text.txt` and the empty
  `snapshot-diff.txt` cover every input and prove snapshot identity;
- `evidence/xact-waves.txt`, `xact-sounds.txt` and `xact-cues.txt` retain the exact graph inventory;
- `evidence/unchanged-source-vs-xna4.log` retains the sole source diagnostic;
- `xna4-diagnostic/Content/` and `cna-diagnostic/` retain all sixteen successful XNB/CNB products;
- `evidence/xna4-content-build.log` retains both the successful visual build and exact XACT-version
  rejection;
- `evidence/cna-focused-audio-tests.log` and `cna-focused-content-tests.log` retain the 256/9 live
  regression passes;
- `scripts/qualify.sh` reproduces the complete offline qualification. CNA conversion is explicitly
  capped at eight workers and every Microsoft tool runs under isolated Xvfb rather than the real
  desktop display.

## Owner decision required

Choose one:

1. authorize a faithful port of this distinct XNA 2 game after providing an authentic XNA2/XACT2
   build route or exact XGS/XSB/XWB banks, and explicitly accept the measured one-line graphics
   state migration as the C++ target contract;
2. explicitly authorize an XNA4 modernization: upgrade/re-author the complete XACT graph with
   equivalent banks, apply the measured state migration, then perform original/native/WEBGL2
   behavioral, visual and audio qualification;
3. accept an evidence-backed historical-game archive/non-port boundary while retaining its complete
   sources, content and product documentation.

Until that ruling, an audio-free port, nine direct SoundEffects, a reduced board demo or a claim
that the XNA4 visual-only build is the authentic XNA2 product would violate the campaign's fidelity
rules.
