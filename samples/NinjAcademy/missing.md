# Missing / Differences from XNA 4.0 original

## Fresh 2026-08-31 audit — blocked on authentic Song output

**Current status: `🛑`; the existing C++ port is not qualified under the current no-workaround
rules.** The complete package contains 33 game units, four runtime common-type units and four
sample-owned content-pipeline units (8,506 C# lines total). Its unchanged Windows Phone/Reach
content project has 47 compiled assets: six SpriteFonts, 28 textures, ten SoundEffects, one Song
and two custom reflective objects produced from XML. The complete 100-file source package and
fresh build scripts are retained at `/rv/tmp/samples/SAMPLE-065-NinjAcademy_4_0/`.

The retained `scripts/build-original.sh` first compiles the unchanged
`NinjAcademyCommonTypes.dll` and `NinjAcademyPipeline.dll`, then drives the unchanged content
project through XNA 4.0's official `BuildContent` task. This disproves several historical claims:

- all six original Moire SpriteFonts and all 28 textures compile; loose PNG/font JSON sidecars are
  substitutions, not a required platform boundary;
- all ten SoundEffects compile through `SoundEffectProcessor` and do not need loose WAV loading;
- the sample's `XDocumentImporter`, `AnimationProcessor` and `ConfigurationProcessor` work
  unchanged and produce authentic `Textures/Animations.xnb` and
  `Configuration/Configuration.xnb` files;
- those two XNBs use normal XNA reflective/list/dictionary/array readers. Live CNA already exposes
  game-registered custom and reflective XNB readers, so hand-translating their XML values is no
  longer justified.

The exact build fails at its sole required Song:

```text
Loaded 47 exact compiled content-project assets.
Processing Sounds\NinjAcademy_Music.wav with Microsoft.Xna.Framework.Content.Pipeline.Processors.SongProcessor
Could not convert audio file NinjAcademy_Music.wav to WindowsMedia format.
BuildContent (WindowsPhone/Reach) result: False
```

A separately labelled diagnostic run omitting only that conversion builds all other 46 XNBs,
including both sample-processed reflective assets. It is not runnable parity: the original loads
`Sounds/NinjAcademy_Music` as `Song` and plays it through `MediaPlayer`. Authentic completion
requires the processor-produced Song XNB and its external Windows Media stream from a real Windows
XNA 4.0 environment. The unchanged game also requires the Windows Phone SDK's
`Microsoft.Phone.Shell.PhoneApplicationService` and a real phone host to qualify launch,
deactivate/tombstone/activate/resume behavior; neither is present in the Wine reference prefix.

The current port contains the loose Song and all other loose assets, hard-coded replacements for
both reflective XNBs, synchronous replacement of the background loader, a custom
`NameEntryScreen`, plain-file high scores, omitted phone lifecycle/tombstoning and screen-stack
serialization, omitted fullscreen/orientation, mouse gesture synthesis and an invented F1 help
overlay. These are historical repair evidence, not accepted differences. In particular, the live
CNA `Guide` now has real asynchronous keyboard-input and message-box overlays, Sharp Runtime has
real `IsolatedStorageFile`/`IsolatedStorageFileStream`, and CNA's threaded content-loading route was
qualified by SAMPLE-061. The old explanations below that call those APIs unavailable are stale.

Do not resume this port by retaining the loose music WAV, hand-authoring a Song XNB, disabling
music, keeping the custom Guide/storage/XML/thread substitutes, or dropping phone behavior. First
obtain the authentic Song pair and a usable Windows Phone reference route (`SAMPLES-DEC-007`). Then
replace every loose asset with official XNBs, register the sample's exact reflective readers,
re-audit all 41 C# units line by line, use the live framework/runtime APIs, and run unchanged-XNA,
native OPENGLES3 and real-Chrome WEBGL2 lifecycle/input/audio parity.

## Historical port notes — retained as untrusted evidence, not waivers

## Windows Phone tombstoning dropped
**XNA behaviour:** `NinjAcademyGame` hooks `PhoneApplicationService.Activated/Deactivated/Launching`
and serializes `GameState` (score, hit points, phase, elapsed phase time) to
`IsolatedStorageFile` so the game can resume mid-round after the OS
tombstones and reactivates the app.
**CNA port behaviour:** Always starts fresh at the main menu; `StartSelected()`
always takes the "no saved game" branch.
**Root cause:** No Windows Phone application-lifecycle equivalent exists on
desktop; CNA has no tombstoning concept.
**Tracked in:** Same class of deviation as this project's other ScreenManager
ports (GameStateManagement, HoneycombRush, UISample) — established precedent,
not a new gap.

## High-score name entry uses a custom keyboard popup instead of Guide
**XNA behaviour:** `Guide.BeginShowKeyboardInput` shows the OS on-screen
keyboard and returns the typed name asynchronously.
**CNA port behaviour:** CNA's `Guide::BeginShowKeyboardInput`/`EndShowKeyboardInput`
always completes with an empty string (no system keyboard on this platform),
and `Guide::BeginShowMessageBox` always throws — so using `Guide` directly was
not viable. `Screens/HighScoreScreen.hpp` adds `NameEntryScreen`, a small
popup that reads real keyboard input (A-Z, Backspace, Enter) via CNA's actual
`Keyboard` API and then calls `HighScoreScreen::PutHighScore()` — a genuine
substitute feature, not a stub returning a fixed "Player" name.
**Root cause:** CNA's `Guide` is a stub for these two calls (see
`cna/include/Microsoft/Xna/Framework/GamerServices/Guide.hpp`); this is a
real, documented platform limitation, not a bug to fix in CNA.
**Tracked in:** CLAUDE.md "Assets"/"do not work around CNA bugs" — this is a
sample-side feature addition to route around a stub API, not a workaround for
a framework bug.

## High-score persistence uses a plain text file, not IsolatedStorageFile
**XNA behaviour:** `HighScoreScreen` saves/loads the table via
`IsolatedStorageFile`.
**CNA port behaviour:** Uses `std::ofstream`/`std::ifstream` against
`highscores.txt` next to the binary.
**Root cause:** Historically CNA/sharp-runtime had no `IsolatedStorageFile`
equivalent, and this port followed that established precedent. As of this
audit, sharp-runtime *does* now provide a real, working
`System::IO::IsolatedStorage::IsolatedStorageFile` (documented "Status: DONE"
in `sharp-runtime/include/System/IO/IsolatedStorage/IsolatedStorageFile.hpp`,
backed by `std::filesystem` — file/directory CRUD, `GetUserStoreForApplication()`,
etc.) — it was completed well before this sample was ported and is reachable
here since `CNA` links `SHARP_RUNTIME` `PUBLIC`. So this is no longer a
framework gap, just a case of the port not (yet) using an API that exists;
the plain-`std::ofstream` approach still works correctly and is faithful in
spirit (a persisted local file), so this is left as-is rather than switched
over speculatively.
**Tracked in:** Same precedent as HoneycombRush's `HighScoreScreen.hpp` (which
has the same now-stale "no equivalent" framing) and RolePlayingGame's
`Session.hpp`; not re-verified/updated there since this audit is scoped to
NinjAcademy only.

## Mouse fallback for tap/drag input
**XNA behaviour:** All input is `TouchPanel` gestures (`Tap` for menu
selection/throwing a shuriken, `FreeDrag` for sword slashes) — this is a
Windows Phone game with no keyboard/mouse input path at all.
**CNA port behaviour:** `ScreenManager/InputState.hpp` synthesizes a `Tap`
gesture on a mouse left-click rising edge, and a `FreeDrag` gesture each frame
the button stays held, matching this project's established "every
interaction is a discrete tap/drag" mouse fallback (DynamicMenu/UISample
precedent, pattern 3/4 in NEXT.md section 6).
**Root cause:** This desktop has no touchscreen and CNA does not synthesize
touch/gesture events from mouse input.
**Tracked in:** NEXT.md section 6.

## Background-thread asset loading simplified to synchronous
**XNA behaviour:** `LoadingScreen` starts a `System.Threading.Thread` running
`GameplayScreen.LoadAssets()` in the background while showing a "loading"
texture, polling `Thread.ThreadState` each frame.
**CNA port behaviour:** `LoadingScreen::LoadResources()` calls
`gameplayScreen_->LoadAssets()` synchronously in the same frame it's
triggered, then immediately marks loading as finished. This sample's asset
set is small and loads well within one frame on desktop hardware, so the
busy-texture screen is only ever visible for at most one frame.
**Root cause:** No concrete benefit to a background thread on desktop for
this sample's small asset set; avoids adding thread-safety concerns to
GameContent loading, which is not documented as thread-safe in CNA.
**Tracked in:** Same precedent as HoneycombRush's `GameplayScreen::LoadAssets()`.

## Animation/Configuration XML hand-translated to C++ construction code
**XNA behaviour:** `Textures/Animations.xml` and `Configuration/Configuration.xml`
are parsed at content-build time by `NinjAcademyPipeline/AnimationProcessor.cs`
and `ConfigurationProcessor.cs` into `AnimationStore`/`GameConfiguration`
objects, then loaded via `Content.Load<AnimationStore>(...)` /
`Content.Load<GameConfiguration>(...)`.
**CNA port behaviour:** `AnimationStore.hpp`'s `BuildAnimationStore()` and
`GameConfiguration.hpp`'s `BuildConfiguration()` hand-translate the exact
values from both XML files into C++ construction code, called directly
instead of going through `ContentManager::Load`.
**Root cause:** CNA has no general XML content-pipeline deserializer.
**Tracked in:** Same precedent as DynamicMenu's `MenuPage2.xml`.

## Faithful reproduction of two original timing quirks (not "fixed")
**XNA behaviour (Animation.cs):** `Animation::Update()` only advances a frame
when `frameChangeTimer >= frameChangeInterval`, but `frameChangeTimer` is only
ever reset to `TimeSpan.Zero` — it is never incremented by elapsed game time
anywhere in the original `Animation.cs`. In practice this means animations
(gold target spin, dynamite fuse, explosion) rarely advance past their first
frame transition in the shipped original.
**XNA behaviour (GameplayScreen.cs):** `ManageGamePhase()` adds
`gameTime.ElapsedGameTime` to `upperTargetTimer`/`middleTargetTimer`/
`lowerTargetTimer` *and then* `ManagePhaseTargets()` adds it again internally
— targets appear on a roughly 2x-faster cadence than `Configuration.xml`'s
`Interval` attributes specify.
**CNA port behaviour:** Both quirks are reproduced exactly as in the original
(`Animation::Update()` in `Animation.hpp`; the double-increment in
`GameplayScreen::ManageGamePhase()`/`ManagePhaseTargets()`), per this
project's "stay as close as possible to the original" philosophy — not
treated as bugs to fix.
**Root cause:** N/A — this is the original's own behavior.
**Tracked in:** N/A; documented here so it isn't mistaken for a porting bug.

## `TextDisplayComponent` ownership kept in a vector, not a single field
**XNA behaviour:** `GameplayScreen.MarkGameOver()` adds a new
`TextDisplayComponent` (the "Game Over" text) to `Game.Components` each time
it's called; the C# GC keeps it alive for as long as `Game.Components`
references it.
**CNA port behaviour:** `GameplayScreen` accumulates every created
`TextDisplayComponent` in a `std::vector<std::shared_ptr<TextDisplayComponent>>`
rather than overwriting a single field, since `Game.Components` only stores a
raw pointer with no ownership — overwriting the single owning `shared_ptr`
would destroy the object while a dangling raw pointer stayed registered.
**Root cause:** C++ has no GC; `Game.Components` needs an explicit owner.
**Tracked in:** Same class of adaptation as the `pendingDestruction_`
shared_ptr-lifetime pattern documented in NEXT.md's "pattern to watch for".

## No known CNA framework gaps hit
Nothing in this port required a change to `cna` or `sharp-runtime`. Every
adaptation above is either an established project precedent or a
sample-local addition (NameEntryScreen) built entirely from existing, real
CNA APIs (`Keyboard`, `TouchPanel`, `SpriteBatch`, `Guide` only where its stub
behavior didn't matter).
