# SAMPLE-138 — `Pickture_ARCHIVE_2_0` audit and owner decision

## Status

Fresh audit complete enough to require an older-XNA product decision under `SAMPLES-DEC-005`.
This is a complete XNA 2 sliding-picture puzzle game for Windows and Xbox 360, not an asset pack,
shader fragment or generic Game State Management example. No audio-free puzzle, loose-asset
replacement, simplified board or unapproved XNA4 rewrite was introduced.

## Complete product inventory

The upstream delivery contains 66 files / 10,417,523 bytes:

- separate Windows and Xbox 360 XNA 2 solutions/projects;
- 26 runtime C# files / 4,473 lines plus a two-file, 96-line custom content-pipeline assembly;
- one 179-line custom Ashikhmin/Shirley HLSL effect;
- eleven distinct 1024×1024 photographs, seven menu/game textures, a SpriteFont declaration and
  two FBX models;
- an XACT2 project with five stereo PCM waves and five corresponding cues;
- a 958-line HTML design/control document, icon, thumbnail and Microsoft Permissive License.

The content project declares 23 items: eighteen textures, two models, one SpriteFont, one effect
and one XACT project. No executable, XNB, XACT bank or built effect is shipped.

## Audited behavior

The game offers five complete modes:

| Difficulty | Board | Sides |
|---|---:|---:|
| Simpleton | 3×3 | one |
| Routine | 4×4 | one |
| Tricky | 4×4 | two |
| Backbreaking | 6×6 | two |
| Bedlam | 10×10 | two |

One random board location is empty. Selecting any chip in its row or column shifts every
intervening chip toward the hole with a 0.1-second animation. On two-sided boards, each chip can
also rotate by π around either axis in 0.65 seconds with an overshooting sine interpolation; front
and back texture coordinates deliberately mirror horizontally. Completion requires every chip to
occupy its authored location and to have both orientation-correct flags restored.

Before play, each visible side is studied for 30 seconds and may be skipped independently. The
shuffle performs `width * height * 2` alternating horizontal/vertical shifts and flips random
chips while limiting simultaneous flip activity to one quarter of the board. The camera slowly
wanders, rotates around the board in 0.6 seconds, and the point light follows its own randomized
motion. A completed board transitions to a forty-photo celebration with falling/swaying models
and a fog fade.

Keyboard controls are arrows to select, Space to shift, Q/E to rotate the board, W/A/S/D to flip
the selected chip and Escape to cancel/exit. The gamepad equivalents are D-pad/left stick, A,
triggers, right stick and Back. The complete screen stack contains the main menu, study/shuffle,
playing, completion and confirmation screens with 2.5-second transitions. Audio cues cover chip
shift, chip flip, board flip, menu focus and puzzle completion.

`PictureDatabase` randomizes all eleven photographs without repeats before cycling. The original
`PictureSet` creates a private `ContentManager` but loads through the global game manager and later
unloads only the unused private manager; this delivered lifecycle quirk must be preserved or
explicitly corrected as part of an approved modernization, not silently “cleaned up” during a
translation.

## Custom model and effect contract

`LightingProcessor` runs the stock model processor after generating tangent frames, then copies
each mesh part's material name into `ModelMeshPart.Tag`. The resulting exact model carries string
tags `Front` and `Back`; runtime `Chip.Draw` uses those identities to select the corresponding
picture and texture-coordinate transform. Dropping or reconstructing the tags in sample code would
be a prohibited content workaround.

The effect implements anisotropic Ashikhmin/Shirley diffuse/specular lighting, a glow override and
untextured/textured techniques using Shader Model 2. Its exact optimized XNA4 payload has 20
parameters, two techniques, two passes and seven objects. MojoShader parses all four shader
objects / 5,636 bytes of bytecode, and a real offscreen OpenGL ES 3 context compiles and binds both
passes, including the textured sampler. The effect and EasyGL compiled-effect path are therefore
not the product blocker.

## Measured XNA4 migration boundary

The 26 unchanged runtime sources were compiled against the complete official XNA4 Windows
assemblies. They reach exactly 20 errors and zero warnings, all in bounded XNA2→XNA4 families:
`ElapsedRealTime`/`TotalRealTime`, Effect/Pass `Begin`/`End`, mutable `RenderState`, old model
vertex/index binding and indexed draw fields, and removed minimum `ShaderProfile` requirements.
No missing game type or unknown application algorithm appears in that diagnostic.

The sample-owned `LightingProcessor` itself compiles unchanged against the official XNA4 content
pipeline. A Windows/HiDef Debug content build produces all 21 ordinary non-effect items and rejects
the unoptimized shader at 89 arithmetic instructions versus the `ps_2_0` limit of 64. The normal
optimized Release pipeline compiles that exact shader and all 21 other non-XACT items, producing
22 unchanged XNBs. Thus no shader-source migration is required. XNA4 `BuildXact` separately and
explicitly rejects the unchanged `Signature = XACT2`, version 16/content-version 43 project as
version-incompatible.

## Current CNA evidence

At CNA HEAD `5347b52eae13`, `cna-content` converts and validates eighteen Texture2D products, the
SpriteFont and the ordinary photograph model: 20/22 Release XNBs. It truthfully refuses two lossy
conversions:

- EffectReader XNB has no native CNB effect encoding and is intended for the compiled-effect
  runtime route already proven above;
- the chip model's `StringReader` mesh-part tags cannot fit Model CNB schema 1, so the converter
  refuses to discard `Front`/`Back`.

The exact chip XNB demonstrably contains both strings and CNA's normal XNB reader implements
mesh-part Tag assignment. Forty-seven focused real-GLES3 ModelTag/XNB/Texture2D/SpriteFont/CNB
tests pass. A future approved port must qualify the exact chip through the chosen runtime/browser
content route; adding a sidecar, deriving the side from mesh order or stripping the tag is not an
acceptable substitute. Extending generic CNB model-tag representation would be a broader schema
design, not a justified intermediate change for a product that still lacks an XACT2 decision.

No CNA or Sharp Runtime source change was needed for this audit. No native or WEBGL2 game is
claimed before the product/audio modernization decision.

## Authentic reference boundary

The retained Wine compiler/content pipeline and offline Win7 VM contain XNA Game Studio 4.0. This
archive requires XNA Game Studio 2.0 and ships no compiled product or banks. No authentic local
XNA2 compiler, XACT2 builder or original executable was found, so the audit makes no original-run,
audio or pixel-parity claim. The complete HTML behavior/control document, source and artwork are
retained as reference evidence rather than treated as a runtime substitute.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-138-Pickture_ARCHIVE_2_0/`.

- `xna2-original/` is the complete byte-for-byte upstream snapshot;
- inventory, SHA-256 and empty snapshot-diff evidence cover all 66 source files;
- `evidence/unchanged-runtime-vs-xna4.log` retains all 20 exact compiler diagnostics;
- Debug/Release pipeline logs and `xna4-diagnostic/Content-Release/` retain all 22 exact XNBs and
  the independent XACT2 rejection;
- the raw optimized effect plus parse and real-GLES3 bind logs retain its exact runtime contract;
- `cna-diagnostic/` retains the 20 lossless CNBs; conversion and 47 passing focused tests are under
  `evidence/`;
- `scripts/qualify.sh` reproduces the full offline audit. Its only parallel step is capped at eight
  workers and it opens no host browser or real-display window.

## Owner decision required

Choose one:

1. provide/authorize an authentic XNA2/XACT2 reference and bank route, then port the complete game
   with the 20 measured graphics/timing mappings while preserving exact cue and model-tag behavior;
2. authorize an explicit complete XNA4 modernization: migrate those 20 API sites, rebuild the
   five-cue audio graph through an approved XACT3/equivalent contract, keep the exact optimized
   effect and tagged chip model, then qualify every mode natively and in real-browser WEBGL2;
3. accept an evidence-backed historical-game non-port boundary while retaining the full source,
   documentation and exact XNA4/CNA content evidence.

Until that ruling, dropping audio, replacing cues with loose WAV calls, removing two-sided chips,
discarding tags, reducing the board/state flow or calling a generic sliding puzzle the port would
violate the campaign's fidelity rules.
