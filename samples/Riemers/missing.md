# SAMPLE-141 — `Riemers` archive audit and owner decision

## Status

Fresh audit complete enough to require a support-data/product-scope decision under
`SAMPLES-DEC-005`, with two measured content-pipeline boundaries recorded under
`SAMPLES-DEC-002`. The physical delivery contains five archives of tutorial assets, but no
tutorial source, project, entry point or runnable product. No game, lesson implementation,
asset viewer, shader rewrite or reconstructed endpoint was invented.

## Exact physical delivery

The upstream directory contains exactly **5 ZIPs / 8,925,631 compressed bytes**. All archives pass
CRC/integrity checks, use safe relative member paths and expand to **54 files / 13,160,500 bytes**:

| Archive | ZIP bytes | Files | Expanded bytes | Delivered role |
|---|---:|---:|---:|---|
| `2D Series - Shooters - Assets.zip` | 2,176,510 | 13 | 2,568,808 | authored MonoGame content manifest plus shooter art/audio/font source |
| `3D Series1 - Terrain - Assets.zip` | 16,788 | 3 | 60,681 | height data and a basic effect |
| `3D Series2 - FlightSim - Assets.zip` | 1,606,482 | 16 | 1,941,964 | models, skybox/terrain art, audio and a basic effect |
| `3D Series 3 - HLSL Intro.zip` | 1,326,863 | 10 | 2,144,625 | car/lamppost models and textures |
| `3D Series 4 - Advanced Terrain.zip` | 3,798,988 | 12 | 6,444,422 | terrain/billboard art, a dome model and two legacy effects |

The expanded inventory is 35 images/textures, six audio files, six DirectX `.x` models, four
effects, one SpriteFont declaration, one `Content.mgcb` and one 4,096-byte raw height/logo asset.
There is no `.sln`, `.csproj`, `.contentproj`, `.cs`, executable, entry point, README or licence
file inside any archive. The only exact duplicate is the 128×128 `heightmap.bmp` shared by Series
1 and Series 4.

The supplied 2D MGCB is the only authored build contract: DesktopGL/Reach with twelve `/build`
items (eight textures, three SoundEffects and one Arial 14 SpriteFont). The other four archives
have no importer/processor/profile declarations. Any inferred content mapping for them is therefore
diagnostic rather than an original project contract.

The two Series 1/2 basic effects each carry a Riemers comment permitting use/modification, but this
does not license the other files. One shooter texture retains a third-party `NOCTUA Graphics`
metadata comment. The absence of a package-wide licence is recorded rather than generalized from
those isolated comments.

## Local tutorial context is supplemental, not missing source

The collection README identifies five Riemers tutorial series: 2D Shooters, 3D Terrain, 3D
Flightsim, 3D HLSL Intro and 3D Advanced Terrain. It says the first three are being upgraded to
MonoGame and the last two are not yet converted. Its links point to wiki lesson pages, but their
text and code are not part of the physical sample directory.

`/rv/tmp/XNAGameStudio/Images/Riemers` contains 192 supplemental screenshots / 294,910,675 bytes.
Three are byte-identical to delivered assets (`explosion.png`, `riemerstexture.bmp` and
`carlight.jpg`), which corroborates the archive-to-series mapping. Screenshots do not supply the
missing source, lesson checkpoints, runtime rules, input behavior or build declarations and are
not treated as authority from which to reverse-engineer a new product.

## Authentic XNA 4.0 content diagnostic

Artifact root: `/rv/tmp/samples/SAMPLE-141-Riemers/`.

`xna4-original/` is a byte-for-byte snapshot of the complete physical upstream directory (all five
ZIPs). `scripts/build-xna4-assets.sh` invokes a
bounded official XNA Game Studio 4.0 pipeline probe under offline Wine/Xvfb. It uses the exact
authored 2D MGCB parameters; for the four contract-free series it labels conventional importer/
processor choices explicitly as diagnostic defaults and uses HiDef. It attempts all 52 pipeline
candidates, excluding only `Content.mgcb` itself and `XNA.raw`.

The official pipeline succeeds for **47/52** candidates and emits 56 XNBs including model
dependencies. The five failures describe original delivery/version problems rather than CNA
regressions:

- authored 2D `hitterrain.wav` is 24-bit PCM, while XNA's `WavImporter` accepts only 8/16-bit
  input;
- the diagnostic Series 2 `SongProcessor` cannot convert `music.mp3` to Windows Media; no source
  project says whether that file was meant to be a Song or SoundEffect;
- both Series 4 effects compile pixel shader 1.1 and official XNA 4 rejects that retired profile;
- Series 4 `dome.x` names absent `clouds.jpg`, while the archive instead supplies `cloudMap.jpg`.

The exact 2D source also takes CNA's ordinary image/WAV route: ten assets build and the same 24-bit
WAV is honestly rejected because CNB PCM16 cannot preserve it exactly. The source tool does not
silently consume the `.spritefont`; the official SpriteFont XNB does pass CNA's canonical route.
No file was converted, renamed or substituted merely to make the audit green.

## CNA canonical-content boundary

An XNB-only staging tree deliberately excludes the zero-byte WMA left by the failed diagnostic
Song conversion. CNA converts **51/56** authentic XNB products to CNB:

- both basic `EffectReader` XNBs remain on CNA's compiled-effect/runtime path and are truthfully
  unsupported by the distinct native CNB effect transcode;
- `target.xnb`, `xwing.xnb` and `racer.xnb` contain valid nullable shared-resource references, but
  CNA's current canonical XNB model sink requires a non-null vertex/index/effect resource for each
  mesh part and rejects them.

The latter is not repaired with a dummy buffer/effect or asset edit. Supporting it faithfully would
require a deliberate nullable-resource representation and runtime contract in CNB model schema v2,
including ownership and backend behavior. With no delivered program proving how these particular
models are consumed, that schema work is recorded under `SAMPLES-DEC-002` and deferred until the
owner supplies/authorizes a product that needs it.

The remaining output includes textures, sounds, the SpriteFont, two models and their dependency
textures. **120/120** focused model/texture/SoundEffect/SpriteFont/XNB/CNJ/CNB tests pass on real
EasyGL OpenGL ES 3.2 under an isolated Xvfb display. These are content-subsystem diagnostics, not a
claim that a missing game renders. No CNA or Sharp Runtime source change was needed at CNA HEAD
`5347b52eae13` and Sharp Runtime HEAD `9cc96cd57cde`.

## Evidence and reproducibility

- `evidence/inventory.tsv` pins every ZIP/member hash and byte count;
- `evidence/zip-members.tsv` records member sizes, compression, CRC and safe-path status;
- `evidence/image-metadata.tsv`, `audio-metadata.tsv` and `model-effect-metadata.tsv` retain exact
  format/profile/reference metadata;
- `evidence/asset-preview.png` is a 36-tile offline contact sheet, visually inspected to confirm all
  35 images plus the 64×64 Gray8 XNA logo in `XNA.raw` decode meaningfully;
- `evidence/screenshot-overlaps.tsv` records the three exact supplemental-image matches;
- `evidence/xna4-asset-probe.log` and `xna4-diagnostic/` retain the 47/52 official XNA results and
  56 XNBs;
- `evidence/cna-source-2d.log`, `cna-content-transcode.log` and `cna-focused-content-tests.log`
  retain the measured CNA boundaries and 120 passing real-GLES3 regressions;
- `scripts/qualify.sh` re-extracts the snapshots, rebuilds all diagnostics and reruns the complete
  audit offline, with at most eight CPU cores/workers and no host browser or real display.

## Owner decision required

Choose one:

1. accept `Riemers` as an evidence-backed asset/tutorial support archive with no standalone CNA
   product;
2. provide or authorize a complete, appropriately licensed tutorial-source snapshot and define
   whether each series means all lesson checkpoints or a specific final endpoint, after which each
   real product can be ported and its concrete content needs revisited; or
3. explicitly scope one or more new native/WEBGL2 demonstrations using these assets and define the
   desired behavior/licensing, acknowledging that these would be newly designed modernizations,
   not ports of delivered runtime products.

If option 2 or 3 actually requires the three nullable-resource models, separately approve the
bounded CNB model-schema/runtime design. Do not repair the old assets, translate shaders, invent
game rules from screenshots or call a contact sheet a port before that product boundary exists.
