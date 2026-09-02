# SAMPLE-142 — `RobotGame_ARCHIVE_2_0` audit and owner decision

## Status

Fresh audit complete enough to require an older-XNA product decision under `SAMPLES-DEC-005`,
an exact-content route under `SAMPLES-DEC-002`, and later integration with the owner's shared
`System.Xml.Serialization` decision under `SAMPLES-DEC-008`. This is Microsoft's complete XNA 2
Robot Game Kit, not an asset pack or a fragment. No XNA4 rewrite, reduced viewer, loose-asset
loader, replacement audio, handwritten XML parser or partial CNA game was invented.

## Exact physical delivery and product

Artifact root: `/rv/tmp/samples/SAMPLE-142-RobotGame_ARCHIVE_2_0/`.

`xna4-original/` is the complete physical upstream directory retained byte for byte despite the
campaign-standard path name: its projects explicitly target XNA **2.0**. The snapshot and live
source each contain **660 files / 99,104,062 bytes**, and their deterministic 660-row manifests
both hash to
`df4ff53709fe52a96732ca10bc4ef32cf1cd7e7bcfd237b53762e98595049ffe`.
The included Microsoft Permissive License applies to the delivery.

There are two complete solutions, one Windows and one Xbox 360. Five C# projects provide the two
game executables, two platform-specific builds of the shared runtime library, and one custom
processor assembly. The 109 C# units contain **36,622 lines**:

| Source component | Files | Lines | Role |
|---|---:|---:|---|
| `RobotGame` | 28 | 15,975 | game, stages, units, weapons, items, particles, menus and entry point |
| `RobotGameData` | 72 | 19,425 | scene/runtime framework, cameras, rendering, collision, input, sound and content readers |
| `RobotGameProcessors` | 9 | 1,222 | animation, particle, collision and normal/specular-map content processing |

Microsoft's included `RobotGame.htm` calls this a complete mini-game and documents the product:
advanced shaders/post-processing, an advanced particle system, collision, positional 3D sound,
input and screen management, a single-player campaign and split-screen two-player versus. The
screen graph reaches main menu → loading → first stage → second stage, or player selection →
versus-ready → versus stage, with message/exit paths. The data proves three distinct modes:

| Level | Mode | Victory condition | Enemies | Respawns | Weapons | Items |
|---|---|---|---:|---:|---:|---:|
| `FirstStage.level` | StageClear | DestroyAllEnemies | 11 | 0 | 4 | 8 |
| `SecondStage.level` | StageClear | DestroyOnlyBoss | 19 | 0 | 5 | 10 |
| `VersusStage.level` | Versus | Survive | 0 | 8 | 10 | 15 |

The documented controls cover both gamepads and two simultaneous keyboard layouts: WASD/F/H/G/T/
Space/V for player one and arrows/numpad for player two, plus menu accept/cancel, weapon switching,
reload, boosters and the debug camera toggle. Debug-only FPS/invulnerability/camera branches and
the Xbox-specific display/device branches are material original code, not dead files to discard.

`evidence/asset-preview.png` is an offline 134-tile contact sheet of all 58 JPG, 37 PNG and 39 DDS
files. It was visually inspected: robot/weapon/world diffuse, additive, normal and specular maps;
both stage environments; particle frames; menu/loading/HUD/versus/mission art; fonts and the
official thumbnail all decode meaningfully. It is evidence of the delivered content, not a
substitute for a running reference.

## XML and custom Content Pipeline are required behavior

The documentation explicitly distinguishes two contracts, and the source confirms both:

- **31 loose runtime XML files** are copied beside the game and loaded via
  `System.Xml.Serialization.XmlSerializer`: 25 robot/weapon/item specs, three levels and three
  particle lists. Their concrete roots are `GameEnemySpec`, `GameWeaponSpec`, `ItemBoxSpec`,
  `GamePlayerSpec`, `GameLevelInfo` and `ParticleReaderList`. `HelperFile.LoadData` drives levels
  and polymorphically selected specs; `ParticleManager.LoadParticleList` then resolves 27, 27 and
  17 named particle instances for the two campaign stages and versus.
- **288 authored XML pipeline assets** are not loose runtime substitutes: 253 `.Animation` files
  deserialize to `AnimationSequence` and 35 `.Particle` files deserialize to
  `ParticleSequenceInfo`, after which the sample's 115-line animation writer and 488-line particle
  writer emit the exact binary contracts consumed by `AnimationReader` and `ParticleReader`.

The main content project has 358 declared items. Processor counts are Animation 253, Particle 35,
Collision 4, ShaderModel 3, stock Model 16, Texture 38, Effect 4, PassThrough 3, FontTexture 1 and
XACT 1. The separate `RobotGameData` content project adds two SpriteFonts and one texture.

The other custom processors are equally semantic:

- `CollideProcessor` transforms every indexed triangle position into world space and stores both
  `Vector3[] Vertices` and a derived `BoundingSphere` in a string/object dictionary on `Model.Tag`;
- `ShaderModelProcessor` generates tangent/binormal channels, rejects models missing their authored
  Maya normal/specular-map properties, strips unused channels and replaces materials with the
  sample effect;
- `ShaderTextureProcessor` remaps normal pixels from `[0,1]` to `[-1,1]`, encodes
  `NormalizedByte4` and generates mipmaps.

These cannot be replaced by stock models, sidecars, loose XML or runtime geometry under the
campaign rules. CNA already supports typed/dictionary model tags and closed sample readers; the
missing evidence is the authentic processor output, not a reason to weaken CNA's runtime model.

## Authentic XNA 2 reference boundary

The prepared `win7` VM was rechecked live, headless and offline (`nic1=none`). It runs Windows
7 SP1 and contains only:

```text
C:\Program Files\Microsoft XNA\XNA Game Studio\v4.0
C:\Program Files\MSBuild\Microsoft\XNA Game Studio\v4.0
```

The exact source is present at
`C:\Users\vboxuser\Desktop\XNAGameStudio\Samples\RobotGame_ARCHIVE_2_0`, but recursive searches
found no retained `.exe`, `.dll`, `.xnb`, `.xgs`, `.xsb` or `.xwb`. The VM returned to `poweroff`.
Thus the former generic “VM unavailable” reason is not used: the precise blocker is that neither
this VM nor the retained Wine toolset has the required XNA 2 Game Studio/XACT2 toolchain or a prior
build product. The unchanged original therefore could not honestly be built or run in this audit.

`scripts/compile-unchanged-against-xna4.sh` instead provides a bounded compatibility measurement.
It compiles all 100 game/runtime units unchanged against official XNA4 Windows assemblies. A
compiler-only shim declares just `ShaderProfile` and `ResolveTexture2D`, the two XNA2 types removed
wholesale, so the compiler can report downstream member changes; it does not edit, run or claim to
port the sample. The result is **117 errors / 0 warnings**. Major exact families include 29
`RenderState` diagnostics, 20 `Begin`/`End` calls, 19 old indexed-render-target/index-buffer
overload diagnostics, 15 `SpriteBlendMode`/`DeviceType`/`SaveStateMode` references, old depth
formats, `ResolveBackBuffer`/`GetTexture`, old vertex declarations/elements/strides, sampler filter
properties, shader-profile/capability checks and old `BasicEffect`/`RenderTarget2D` constructors.
This is an XNA2→XNA4 migration surface, not 117 missing XNA4 APIs for CNA to add.

## Official XNA4 content diagnostic

`scripts/build-xna4-stock-content.sh` uses the official XNA Game Studio 4.0 pipeline under an
isolated Xvfb display, Windows/HiDef/Release, without modifying source assets:

- the main project has 62 stock-processor candidates and 296 sample/XACT2 candidates;
- **60/62** stock items build; `ShaderModelEffect.fx` line 86 and `GaussianBlur.fx` line 18 fail
  because their function identifiers `VertexShader`/`PixelShader` are reserved by the XNA4 effect
  compiler. `ShaderModelEffect` additionally asks for `vs_1_1`/`ps_2_a`;
- all **3/3** `RobotGameData` stock items build;
- the two successful effects are `BoosterEffect` and `PostScreen`;
- the exact `XACT2`, version 16/content version 43/August 2007 project is explicitly rejected as
  incompatible by XNA4 `BuildXact`.

The 63 successful source entries emit **126 XNBs / 63,437,811 bytes** after model dependencies.
The XAP is not decorative: it declares four categories, seven variables, three RPC curves, 55
waves, 55 sounds and 55 cues; the runtime maps 52 of those names across music, menus, movement,
weapons, impacts, destruction and pickups, including 3D emitter playback. Loose-WAV playback would
discard cue/RPC/category semantics and is forbidden.

The 296 custom/XACT entries were deliberately not represented with stubs and were not claimed
built. Building them through XNA4 first requires a deliberate complete runtime/pipeline migration,
which is one of the owner choices rather than an audit workaround.

## CNA diagnostic and live capability

CNA HEAD `5347b52eae1311fbea1f89955ae8a48c6843a88a` converts **124/126** of the authentic XNA4
diagnostic XNBs to canonical CNB. The only two failures are the successful `EffectReader` XNBs,
which remain on CNA's distinct compiled-effect/runtime-XNB path and truthfully have no native CNB
effect transcode. Models (including dependency textures), cube maps, ordinary textures and all
three fonts take canonical routes.

On an isolated real EasyGL OpenGL ES 3.2 context, **156/156** focused XNB/CNB model, model-tag,
texture, cube, SpriteFont, effect and content-pipeline tests pass. In particular, the existing
`DictionaryModelTagLoadsAndKeepsEachEntrysOwnType` and typed-list tests disprove a general CNA model
tag blocker. This qualification is subsystem evidence only; it does not claim the absent XNA2
processor products or 36,622-line game run.

Sharp Runtime HEAD was `9cc96cd57cde394940cc24d58743edf9bf63d3fb`. A committed
`xml-serialization` module is now visible in that shared checkout, but the owner explicitly said
another session owns it and will announce when it is ready for analysis. It was therefore neither
modified, qualified nor treated as integrated for this sample. Robot Game supplies another
concrete `SAMPLES-DEC-008` fixture for that later review; XML availability alone will not resolve
the XNA2, custom-pipeline and XACT2 decisions.

No CNA or Sharp Runtime source change was required or made by this audit.

## Evidence and reproduction

- `evidence/inventory.tsv` pins every source path, size and SHA-256;
- `source-projects.tsv`, `content-items.tsv`, `xml-assets.tsv`, `levels.tsv`,
  `particle-lists.tsv`, `effects.tsv` and `xact-project.tsv` retain the measured contracts;
- `image-assets.tsv`, `audio-assets.tsv` and `asset-preview.png` retain content hashes/metadata and
  the visually inspected offline sheet;
- `win7-xna-toolchain.txt` records the live offline VM/toolchain/prebuilt-output audit without
  storing its credential;
- `unchanged-runtime-vs-xna4.log` and `xna4-runtime-errors.tsv` retain the 117-diagnostic migration
  measurement;
- `xna4-stock-content.log` and `xna4-diagnostic/Content-stock/` retain all 65 official XNA4 stock
  attempts and their 126 outputs;
- `cna-stock-transcode.log`, `cna-diagnostic/Content-stock/` and
  `cna-focused-content-tests.log` retain the 124 canonical outputs and 156-test GLES3 gate;
- `scripts/qualify.sh` rebuilds every non-VM diagnostic, regenerates the evidence/contact sheet,
  reruns the CNA tests and enforces at most eight CPU cores/workers without using a real host
  display or browser.

## Owner decision required

Choose one product boundary:

1. **Authentic XNA2 route:** supply or authorize installation of the original XNA Game Studio 2.0
   and matching XACT2 tools in the offline VM (or supply verified complete build products), then
   capture the running original and preserve its exact processor/audio outputs while translating
   the full game to CNA's XNA4-shaped API;
2. **explicit complete XNA4 modernization:** authorize the 117-site API migration, both rejected
   effects, the full custom processor assembly and equivalent XACT audio as one reviewed product
   change before the faithful C++/CNA translation; or
3. **historical non-port:** accept this complete but XNA2-only game as evidence rather than an XNA4
   sample product.

After the owner declares the shared Sharp Runtime XML work ready, separately qualify its exact
wire behavior against Robot Game's six runtime root shapes and the two pipeline XML graphs. Do not
start a partial port, drop versus/audio/particles/animation, translate only the menu, hand-edit
assets, or treat the XNA4 stock subset as the complete game while these decisions remain open.
