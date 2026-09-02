# SAMPLE-133 — `Movipa` audit and owner decision

## Status

Fresh audit complete enough to require both an older-XNA product decision under
`SAMPLES-DEC-005` and the shared XML-serialization ruling under `SAMPLES-DEC-008`. Movipa is a
complete XNA 2 moving-image puzzle game, not a small video-texture technique. No reduced demo,
loose-audio replacement, handwritten save parser or unapproved XNA4 modernization was introduced.

## Complete product inventory

The upstream delivery contains 315 files / 238,071,055 bytes:

- seven XNA 2 projects: `Movipa`, `MovipaLibrary`, `SceneData`, `SceneDataPipeline`,
  `SkinnedModel`, `SkinnedModelPipeline` and `MovipaPipeline`;
- 111 C# files / 29,259 lines and 37 XML files / 45,442 lines;
- 20 scene-layout `.stg` files and 15 `.l4t` layout assets;
- 187 content files, including 87 images, 13 FBX models, 11 WAV files and one PSD source;
- a 127-item content project using stock Texture/Model/Effect importers plus sample-defined
  pass-through, font-texture, scene, layout and skinned-model processors;
- an XACT2 authoring project containing eleven waves, eleven sounds and eleven cues;
- product documentation and the Microsoft Permissive License.

The game targets 1280×720 and Shader Model 2.0. Its documentation explicitly presents a complete
minigame using non-continuous texture coordinates to turn moving imagery into independently
selectable puzzle pieces. Background loading, scene transitions, animation, save slots and the
custom content pipeline are part of the product.

## Audited game behavior

The normal game contains ten authored stages and advances through them after a successful result.
Board dimensions progress from 3×2 to 8×5; later stages add tile changes and rotation, with time
limits from three to ten minutes. File selection loads one of the normal-mode save records and
supports deletion. Completion records play count, score, best result and accumulated display time;
failure enters Game Over. The complete scene path is Logo → Title → Mode, then either normal file
selection or free-play configuration, Puzzle, Result and the next stage/staff roll or title.

Free mode independently selects puzzle style, tile change/revolve/slide behavior, rotation, one of
ten movies and board division. The ten moving-image sources include two anime layouts, ghost,
penguin, sea, street and UFO scenes, a skinned `Dude` animation, particles and a 19-frame/30-fps
rendered sequence. The layered 2D scene system loads authored XML/stage layouts asynchronously and
combines them with the custom movie/layout/skinning content graphs.

| Action | Keyboard | Gamepad |
|---|---|---|
| navigate menus / move tile cursor | Arrow keys | D-pad |
| accept / select tile | Enter | A |
| cancel selection / menu | Escape | B |
| rotate clockwise | Y | Y |
| rotate counter-clockwise | X | X |
| hint | Q | left shoulder |
| zoom | E | right shoulder |
| pause | Tab | Start |
| delete selected save | X | X |

The XACT graph is behavioral content. Its cues are `TitleBackgroundMusic`,
`SelectBackgroundMusic`, `SoundEffectOkay`, `SoundEffectCancel`, `SoundEffectCursor2`,
`SoundEffectCursor1`, `GameOverBackgroundMusic`, `GameClearBackgroundMusic`,
`GameBackgroundMusic`, `ResultScore` and `SoundEffectClear`. Four categories and three authored
255-repeat loops govern music/UI/gameplay playback. Replacing these with eleven direct loose WAVs
would discard the authored graph and change the sample.

## Save-data boundary

Normal-mode persistence uses `System.Xml.Serialization.XmlSerializer` through reachable load and
save paths. `SaveData` publishes seven serialized properties for current stage, play count, score,
best score, best-time text, total-play-time text and file name; its `TimeSpan` convenience
properties are explicitly ignored. This is a smaller object graph than SAMPLE-070, but it is the
same missing reusable .NET contract that reopened SAMPLE-014 and blocks SAMPLE-066/SAMPLE-070.
Storage itself is present in CNA and its focused tests pass. A one-off parser would therefore be a
sample workaround and is not authorized while `SAMPLES-DEC-008` remains pending.

## Authentic reference and XNA4 migration boundary

The archive ships source content rather than an executable or XGS/XSB/XWB output. The available
offline toolchain is XNA 4; an authentic local XNA2 game/XACT2 build route has not been found.
Consequently no XNA2 execution or audiovisual parity is claimed.

Diagnostic compilation of the unchanged 94-file combined runtime against XNA4 first reaches the
removed `ResolveTexture2D` and `SpriteBlendMode` APIs. A full static scan also records the broader
expected XNA2 surface: mutable `RenderState`, indexed `SetRenderTarget`, `GetTexture`, explicit
resolve operations and old Effect/Pass Begin/End calls. The unchanged `SceneDataPipeline` and all
eight `MovipaPipeline` sources compile against XNA4. The unchanged skinned-model pipeline reaches
only three `TargetPlatform` namespace errors.

The official XNA4 Windows/HiDef pipeline was then exercised with two explicitly isolated
diagnostic compatibility edits, never applied to the upstream snapshot:

1. import the XNA4 Content Pipeline namespace for `TargetPlatform` in copied diagnostic writers;
2. rename shader functions named `VertexShader`/`PixelShader`, which XNA4's effect compiler treats
   as reserved identifiers, without changing shader calculations.

With those bounded edits, all 126 non-XACT content declarations build successfully and model/shared
dependencies produce 138 valid XNBs. The exact XACT2 project is rejected as an incompatible XACT
version. This proves the visual/data migration is technically bounded; it does not turn the result
into an authentic XNA2 build or solve the required audio graph.

## Measured CNA boundary

At CNA starting HEAD `7712534d3d22`, `cna-content` losslessly converted and validated 93 of the 138
XNB products. It honestly rejected 45 products whose graphs require sample-defined SceneData,
LayoutInfo, movie or skinned-model readers, a non-null custom `Model.Tag` representation, generic
collection/dictionary materialization, or the legacy EffectReader route. These are not presented
as 45 framework defects: a future faithful C++ port would own closed readers for its own types and
retain the exact XNB graph, while any genuinely reusable gap discovered during that port must be
fixed in CNA rather than bypassed.

The focused live regression suite exposed one unrelated timing assumption in CNA's XACT test: a
fixed 50 ms sleep expected SDL3's asynchronous device callback to have stopped a fire-and-forget
voice, although a healthy first callback can arrive later. CNA commit `5347b52ea` makes that test
wait for the real mixer state, without manually reconciling the Cue or bank. The corrected final
evidence is 256/256 focused XACT/audio tests, 31/31 focused content tests and 9/9 focused storage
tests. CNA ending HEAD is `5347b52ea`.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-133-Movipa/`.

- `xna-original/` is the complete byte-for-byte upstream snapshot;
- `evidence/` retains the inventory, exact snapshot comparison, compiler diagnostics, XNA4 build,
  XACT rejection, CNA transcode/validation results and all focused regression logs;
- `xna4-diagnostic/Content/` retains the 138 official XNA4 XNB products;
- `cna-diagnostic/` retains the 93 validated CNB products;
- `scripts/compile-unchanged-runtime-against-xna4.sh`,
  `compile-unchanged-pipelines-against-xna4.sh` and
  `compile-unchanged-movipa-pipeline-against-xna4.sh` reproduce the source boundary;
- `scripts/build-xna4-content-diagnostic.sh` reproduces the isolated content diagnostic;
- `scripts/audit.sh` verifies all measured counts and `scripts/qualify.sh` reproduces the complete
  offline qualification. All CNA content work is capped at eight workers and every Windows tool
  runs under isolated Xvfb rather than the host desktop display.

## Owner decision required

Choose one:

1. authorize a faithful port after providing an authentic XNA2/XACT2 build route or exact banks,
   approve the measured XNA2→XNA4 API/pipeline compatibility boundary, and complete the shared
   `XmlSerializer` decision before translating all seven projects and closed content readers;
2. explicitly authorize a complete XNA4 modernization: migrate every measured graphics/effect and
   pipeline call, re-author equivalent XACT3 banks, retain all ten stages/movies/scenes/readers and
   qualify original/native OPENGLES3/WEBGL2 behavior after the shared serializer exists;
3. accept an evidence-backed historical-game archive/non-port boundary while retaining its complete
   source, content, authoring graph and documentation.

Until that ruling, a video-only viewer, one puzzle board, omitted save system, direct-WAV audio,
flattened/custom-parsed scene data or a claim that the diagnostic XNA4 content build is the complete
original product would violate the campaign's fidelity and no-workaround rules.
