# Missing / Differences from XNA 4.0 original

## Current-head requalification — 2026-09-26

**Complete (`✅`) on cna-samples `develop`, CNA `next` `cefe6c83b`, and
SharpRuntime `next` `d86adb65`.** No dependency source changed. The older
2026-09-06 record below is historical; the fresh build, game, audio and
browser evidence is under
`/rv/tmp/samples/SAMPLE-065-NinjAcademy_4_0/evidence/requal-20260926/`.
The artifact root's `MANIFEST.md` names the retained products and exact
rebuild scripts. The owner-authorized prune on 2026-09-26 removed 27
intermediate paths and reduced the artifact root from 371.2 MB to 115.2 MB.
The retained stripped native product again passed Pause → Quit → title →
clean exit (`evidence/post-prune-20260926/native-pause-quit/`); a repeat prune
dry run proposes zero paths.

The physical upstream `NinjAcademy_4_0` and retained `xna4-original/` still
match byte for byte across 100 files. Its sole game project is Windows
Phone/Reach with 33 compiled C# files; the shared-type and sample-owned
pipeline projects add four each. The original needs the Phone shell and has
no desktop game configuration, and this host has no runnable Phone shell.
`scripts/build-original.sh` freshly built the unchanged four-unit shared
assembly, four-unit sample pipeline and 46 non-Song content items through
XNA 4.0 WindowsPhone/Reach under Wine. It SHA-verified the authentic offline
Win7 Song XNB/WMA pair and restored it after Wine's unavailable Song encoder.
The resulting 47 XNBs and WMA are all 48 byte-identical to the committed
content. The fresh native product has those 48 plus the same Ogg-FLAC
companion (49 matches).
The checked-in WMA/XNB/Ogg-FLAC SHA-256 hashes still match the table below;
the Ogg-FLAC is 48 kHz stereo and decodes to the recorded source-WAV PCM hash.

**The old web loading workaround is gone.** The original
`Screens/LoadingScreen.cs` always creates a `System.Threading.Thread` to
execute `GameplayScreen.LoadAssets`, and only transitions once it stops.
The port now uses that same thread path on native and WEBGL2, and tests the
`ThreadState::Stopped` transition. Its Emscripten-only synchronous branch,
`loadFinished_` flag and conditional update were removed. The web build sets
`CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=ON`. Current CNA's general EasyGL
context lease and WebGL proxying allow the loading thread to create content;
no renderer workaround or runtime change was needed. The remaining `CNAEXT`
uses are the approved input opt-in and necessary C++ reflection/Guide
mechanics already explained in `diff.md`; a targeted bypass scan found no
other sample workaround.

Fresh Release OPENGLES3 and threaded WEBGL2 builds use the active `../cna`
and `../sharp-runtime` checkouts. The native executable resolves colocated
`libcna.so` through `$ORIGIN`; its 49 content files match the repository.
The isolated native run recorded 12 inspected 800×480 states: menu,
instructions, loading/countdown, live gameplay, pause/resume, shuriken,
sword drag, high scores and clean exit. A separate short run proved that
the pause menu's Quit choice returns to the title and then exits cleanly.
A second run seeded a valid
`State.txt` with score 1200 and one remaining life, reached the real
missed-bamboo Game Over, opened the Guide keyboard by tapping as the Phone
game does, entered `Codex65`, and persisted a 1200-point table entry. A
fresh process read that entry and displayed it in the high-score screen;
both processes exited cleanly. The private-sink native capture contains
50.16 seconds of real 44.1 kHz stereo audio (mean −19.8 dB, peak 0.0 dB).

Real Chrome loaded the 800×480 WEBGL2 product with
`crossOriginIsolated=true`, passed menu, instructions, threaded loading,
gameplay, pointer gestures, pause/resume and 600 animation frames, with no
runtime exception, relevant HTTP error or fatal console message. A private
sink recorded 34.55 seconds of real stereo browser audio (mean −30.0 dB,
peak 0.0 dB). Firefox 140.15.0esr independently rendered gameplay through
the same loading thread; tapping a target increased the visible score from
0 to 10. The gallery now has the exact four-file Release bundle, a scoped
COOP/COEP launcher, active-gameplay screenshot, detail page and 62nd card.
The exact staged six-file bundle passed a clean Chrome run over plain HTTP
with `crossOriginIsolated=true`, gameplay, pause, 600 frames and no runtime
or content failure. All four Emscripten assets returned HTTP 200.

The selected original is Phone-only and cannot be run as a live XNA visual
or audio reference on this Linux host. The unchanged source, exact official
content, the prior source audit and the fresh native/browser runs are the
available fidelity evidence. The original Microsoft license is now also
restored beside the port's `NinjAcademy.htm`. No active known sample
workaround or runtime gap remains.

## Historical completion record — 2026-09-06

**Historical claim: no known differences.** The port covered all 33 game units, the four
`NinjAcademyCommonTypes` units and the content the four `NinjAcademyPipeline` units built.
Three deliberate differences were recorded in [`diff.md`](diff.md). The current-head audit
above supersedes the old no-workaround claim.

Artifact root: `/rv/tmp/samples/SAMPLE-065-NinjAcademy_4_0/`.

| Directory | Contents |
|---|---|
| `xna4-original/` | The unchanged upstream snapshot (`NinjAcademy.sln`, Windows Phone / Reach). |
| `xna4-build/` | The official XNA 4.0 pipeline runner and its `Content-phone` output. |
| `cna-native-opengles3/` | Reusable CMake build tree and the native `NinjAcademy_cna_samples`. |
| `cna-web-webgl2/` | Reusable Emscripten build tree and the complete WEBGL2 bundle. |
| `evidence/` | Native, Guide/resume and both browsers' captures with their logs. |
| `scripts/` | `build-original.sh`, `capture-cna-native.sh`, `capture-cna-native-guide.sh`, `capture-cna-web.sh`, `capture-cna-firefox.sh`. |

## Original reference

`NinjAcademy.csproj` declares one configuration pair only — `Debug|Windows Phone` and
`Release|Windows Phone`, `XnaPlatform=Windows Phone`, `XnaProfile=Reach` — and the game class
takes its lifecycle from `Microsoft.Phone.Shell.PhoneApplicationService`. There is no Windows or
Xbox configuration to build, and no Windows Phone 7 host on this machine, so the original cannot be
executed here. The reference is therefore file-level plus the official content pipeline, which
**does** run: `scripts/build-original.sh` compiles the unchanged `NinjAcademyCommonTypes.dll` and
`NinjAcademyPipeline.dll` and drives the unchanged content project through XNA 4.0's own
`BuildContent` task for WindowsPhone/Reach.

## Content

All **47** `.xnb` in `Content/` are byte-identical to that build's `Content-phone` output — six
Moiré SpriteFonts, 28 textures, ten SoundEffects, the Song, and the two reflective objects the
sample's own `XDocumentImporter` / `AnimationProcessor` / `ConfigurationProcessor` produce from
`Animations.xml` and `Configuration.xml`. No loose PNG, WAV, font JSON or hand-authored data
remains.

The Song is the genuine pair produced by the owner's offline Win7 SP1 VM running XNA Game Studio
4.0's own `WavImporter`/`SongProcessor` (`/rv/tmp/samples/SAMPLES-DEC-007-Win7-SongProcessor/`):

| File | SHA-256 | Size |
|---|---|---|
| `NinjAcademy_Music.xnb` | `c2730ebb85da9b08ec76efe93412d03a50a6f54204c3874ca6d60768c6d72490` | 135 B |
| `NinjAcademy_Music.wma` | `d97e8c44ef106871cf9594e9704bfd842c73ec79336b5d44265823c918f07079` | 1,287,767 B |

SDL3_mixer decodes no WMA on either target, so `NinjAcademy_Music.oga` is a lossless deployment
companion that CNA's ordinary sibling-media probing finds — the same arrangement SAMPLE-060
established. It is generated from the **original** `NinjAcademy_Music.wav` rather than from the
lossy WMA, so it carries exactly the PCM the pipeline itself consumed:

```text
ffmpeg -nostdin -hide_banner -loglevel error -y -i NinjAcademy_Music.wav -map_metadata -1 \
  -c:a flac -compression_level 8 -fflags +bitexact -flags:a +bitexact -serial_offset 0 \
  NinjAcademy_Music.oga
```

Repeated conversion gives SHA-256
`016758e37f5f2de5e65596ce243b9da3fe3f1599b73ddb38473b58a838891b6b` (5,287,091 bytes). Decoded to
stereo 48 kHz signed 16-bit PCM it is byte-identical to the source WAV — 10,053,820 bytes, SHA-256
`552277e7420ee2bf4fd7c994e2a5621a981c7fb16a7f362b8b703f276ccf4437`. (Decoding the WMA instead
yields 10,059,776 bytes: 1,489 stereo frames of decoder padding on top of the same audio, which is
why the lossless source was preferred.) The authentic XNB and WMA are checked in unchanged; asset
identity and `Content.Load<Song>("Sounds/NinjAcademy_Music")` are untouched.

The historical `help.png` is preserved at the sample root beside `CMakeLists.txt`. It is not in
`Content/`, is never loaded, and the non-upstream F1 overlay the old port carried is gone.

## Framework defects this sample found and fixed

Five in `cnanext`; none in `sharp-runtimenext`.

**1. `ReflectiveTypeReader` could not read a `Point` field** (`22e89f18f`). `Animation`'s
`rowAndColumnAmount` and `frameSize` are `Point`s written inline with no reader index, and the
member dispatcher had no case for the type. Two decode tests.

**2. A visible `Guide` withheld nothing from the game** (this session). On every real XNA platform
the shell draws and drives the Guide and owns the screen while it is up; CNA draws its overlay
inside the game's own `Draw()`, so the game kept reading the same taps. Tapping outside the
"Load game" box landed back on the menu entry that raised it, raised a second box and terminated
on *"A message box is already pending."* `TouchPanel` grew a withhold that `Guide` raises for as
long as `Guide.IsVisible`, and the withhold outlives the answering click until its button is
released — otherwise the release of the click that answers the box arrives as a tap on whatever the
box was covering. That second half was not cosmetic: it started a second content load, and the
resumed game showed a score of 0 because the first load had already consumed the saved state.
Four tests, all confirmed red with the fix removed.

**3. `Game`'s component lists were not safe across a loading thread** (this session). The XNA
loading-screen pattern builds the next screen's components on a background thread and adds them to
`Game.Components` — NinjAcademy's `LoadingScreen` and SAMPLE-061's both do it — while `Update()`
and `Draw()` iterate the two ordered lists. A stress test that reproduces the shape aborted in
**3 of 8** runs before the fix and **0 of 8** after.

**4. `GameComponentCollection` did not own what XNA's owns.** XNA's collection is a
`Collection<IGameComponent>` and holds a strong reference: a component stays alive for exactly as
long as it is registered, so a game that drops its own last reference leaves a live component in
`Game.Components` rather than a dangling one. The original relies on that without thinking about
it — `MainMenuScreen`'s saved-game branch abandons a loading screen, and with it a `GameplayScreen`
that has already registered 87 components, on every frame it is still transitioning off. CNA held
raw pointers only, so a faithful port had to track and unregister what it owned. The collection now
takes an ownership-taking `Add(std::shared_ptr<IGameComponent>)` overload, released after
`ComponentRemoved` is raised so a handler still sees a live component; the raw-pointer overload is
unchanged and remains right for a component the game holds as a member. Five tests.

**5. `Game` kept calling a component removed mid-frame** (this session). XNA can leave a removed
component in the frame's snapshot because the snapshot holds a strong reference; CNA's snapshot is
raw pointers, and whoever removes a component usually frees it in the same breath — a screen
manager dropping the screen that owns them, for instance. A removed component is now cleared from
the in-flight snapshot; its test aborts in **4 of 4** runs with the fix removed.

## Verification

**Native, `CNA_GRAPHICS_RENDERER=OPENGLES3`** (`scripts/capture-cna-native.sh`, twelve captures in
`evidence/cna-native-opengles3-final/`): title screen with the timed ninja and title reveal, Start →
instructions → loading → 3-2-1 countdown → gameplay, a tap throwing a shuriken, a free drag slicing
bamboo, Escape opening the pause menu, Resume returning through the countdown, Quit returning to the
main menu, the high-score table with all seven places, and Exit leaving with status 0. The run log
carries no exception, and `highscores.txt` is written into isolated storage.

**Guide and tombstone resume** (`scripts/capture-cna-native-guide.sh`,
`evidence/cna-native-guide/`): with a seeded `State.txt` (1200 points, three hit points, one phase
passed, 7.5 s elapsed) Start raises the real `Guide.BeginShowMessageBox` overlay with Yes/No; a tap
that misses both buttons changes nothing; Yes resumes through the loading screen into a game whose
HUD reads **Score: 1200** with **three of five hearts**. The same capture is the calibration for
defects 2 and 4 above — before them it aborted, and before the click-release half of defect 2 it
reached gameplay showing Score: 0 and five hearts.

**Browser, `CNA_GRAPHICS_RENDERER=WEBGL2`**: the complete `.html`/`.js`/`.wasm`/`.data` bundle is
served over HTTP and driven in real Google Chrome (`scripts/capture-cna-web.sh`,
`evidence/cna-web-webgl2-chrome-firefox-fix/`) — WEBGL2 context, audio mixer at 44.1 kHz stereo,
600 animation frames, menu, instructions, gameplay, sprite movement, the pause menu and resume, with
no runtime exception, no fatal console message and no HTTP error other than the browser's own
`favicon.ico`. Firefox 140.10.1 ESR is checked independently
(`scripts/capture-cna-firefox.sh`, `evidence/cna-web-webgl2-firefox-mouse-qualified/`) and does not
stall on the loading screen.

**Tests.** `CnaGamerServicesTests` 372/372, `CnaInputModuleTests` 500/500, `CnaRuntimeTests` 161/161
except the pre-existing environmental
`GameWindowPlatformTest.DelegatesStateAndGeometryToTheSelectedPlatformWindow` (it expects a window
manager to honour `AllowUserResizing`, and Xvfb has none; it fails in isolation on an unmodified
tree too). `CnaContentTests` 1792/1798 with four skips and two failures that predate this work and
belong to other sessions' content-pipeline tasks
(`XnbContentPipelineTest.SpriteFontRuntimeXnbAndTranscodedCnbHaveEquivalentSemantics`,
`ContentManagerVideoXnbTest.TheObjectReferencedFormLoadsToTheSameValuesAsTheInlineOne`); building
that suite at all first needed a one-argument `CnbDocument::Parse` call left behind by
`347139500`, fixed separately.

## Upstream behaviour reproduced rather than repaired

- `Animation::Update()` never increments `frameChangeTimer` — it is only ever reset — so animations
  rarely advance past their first frame. Reproduced.
- `ManageGamePhase()` adds `ElapsedGameTime` to the three target timers and `ManagePhaseTargets()`
  adds it again, so targets appear at roughly twice the cadence `Configuration.xml` states.
  Reproduced.
- `SwitchConfigurationPhase()` tests `Phases.Count > GamePhasesPassed` and then indexes
  `Phases[++GamePhasesPassed]`, one past the end on the final phase. Reproduced; the sample's own
  three-phase configuration never reaches it in play.
- `MainMenuScreen.Update`'s saved-game branch never clears `isMovingToLoading`, so it rebuilds its
  background and loading screens on every frame the menu is still transitioning off, abandoning a
  fully loaded `GameplayScreen` each time. Reproduced — and it is what exposed framework defects 4
  and 5.
- `EndingAnimationComponent.cs` declares a class named `DisappearingAnimationComponent`; the port
  keeps the class name and names the file after it.
- `SubCreateBambooSliceComponets` keeps its upstream spelling.

## Representation choices inherited from the framework

`AnimationStore` holds its animations in a `std::unordered_map` because that is what CNA's
`DictionaryReader` produces, and `GamePhase`'s arrays are `std::vector` for the same reason. Where
the sample chooses for itself it uses the .NET type the original names:
`System::Collections::Generic::Stack` for the six target/bamboo/dynamite pools, `Dictionary` for
`HighScoreScreen`'s place mapping, `System::EventHandler<System::EventArgs>` for the two C# events
`LaunchedComponent` and `StraightLineMovementComponent` declare (subscribed with `+=` and
identifying their component from `sender`, as the original's handlers do), and
`ArgumentOutOfRangeException` / `InvalidOperationException` / `KeyNotFoundException` where the
original throws them.

## Superseded records

Every deviation the 2026-07 port recorded — dropped tombstoning, the invented `NameEntryScreen`,
plain-file high scores, mouse gesture synthesis inside `InputState`, synchronous asset loading,
hand-translated `Animations.xml`/`Configuration.xml`, and the F1 help overlay — is gone. So is the
2026-08-31 audit's conclusion that a Windows Phone lifecycle host was still needed: `Game.Activated`
and `Game.Deactivated` carry the launch/deactivate/activate signals the sample actually uses, and
the resume path is exercised end to end above.
