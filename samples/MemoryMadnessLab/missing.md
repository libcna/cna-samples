# Missing / Differences from XNA 4.0 original

## Fresh 2026-09-01 audit — two runnable lab endpoints and a phone lifecycle boundary

**Current status: `🛑`; no C++ port, reduced game or sample workaround was added.** The exact
package contains two independently runnable Windows Phone XNA projects, not a starter tree plus one
solution. The owner needs to select the training-stage representation under `SAMPLES-DEC-005`.
The polished endpoint also makes Windows Phone launch/tombstone/reactivation behavior part of the
product, so it needs the same authentic phone-host/reference boundary already recorded for
SAMPLE-065 before it can be qualified faithfully.

The complete 116-file, 9,367,107-byte upstream snapshot, reproducible official-pipeline scripts and
build evidence are retained at
`/rv/tmp/samples/SAMPLE-097-MemoryMadnessLab_4_0/`. The byte-identical original 82-page Word lab is
also retained beside this file as `Multi-touch Game Development.doc` (SHA-256
`f88618a00fa95aa64fe35601f6bc7d1a7d36b2de9cb37e1c2fd48d15bccff152`).

## Measured source and stage structure

The 90-minute document explicitly divides its 158 numbered steps into two exercises:

- `EX1_BasicGameWithStateManagement` is a complete 12-source, 2,667-line Phone/Reach game. The
  document directs the student to compile and deploy it, calls it “completely playable”, and only
  then begins the next exercise. It implements the four-button memory sequence, forgiving
  multi-touch sampling, level XML, timers, scoring, full-screen 480x800/30 Hz presentation and the
  shared XNA Game State Management screen stack. It deliberately starts directly in gameplay and
  lacks the later polish.
- `EX2_PolishAndMenus` is a separate solution/project/content endpoint with 20 source files and
  4,455 lines. It retains the gameplay and adds the sound manager, animated background doors,
  main/new-game/loading-instructions/pause/high-score screens, Guide name entry, persistent text
  high scores, save/resume prompts, background asset loading and complete phone lifecycle state.
  The lab finishes by testing deactivate/tombstone/activate, resuming at the pause screen and
  continuing the partially entered sequence.
- `Assets/` contains 1,943 lines of code snippets and media for students to insert while following
  the lab; it has no project or entry point and is not a third product. Its eight code files are
  byte-identical to the corresponding final EX2 files.
- The six shared screen-manager sources, app manifest and three icon/background files are
  byte-identical between EX1 and EX2. The game class, gameplay, level/settings, level XML, project
  declarations and sound sets differ, while EX2 adds six screen classes and `AudioManager`.

Collapsing EX1 into EX2 would erase a documented, runnable teaching checkpoint. Conversely, carrying
both produces two mostly shared games. That is the same material scope choice as the three-stage
Honeycomb Rush training kit, not an engineering defect that the agent may settle by omission.

## Authentic content evidence

Content is not a blocker. `scripts/build-original.sh` reads every unchanged `<Compile>` declaration
from both content projects and invokes XNA 4.0's official `BuildContent` task. All four builds return
true:

| Endpoint | Platform/profile | Compiled items | Verbatim items |
|---|---|---:|---:|
| EX1 | WindowsPhone/Reach | 19 XNB | 1 level XML |
| EX1 | Windows/Reach | 19 XNB | 1 level XML |
| EX2 | WindowsPhone/Reach | 19 XNB | 1 level XML |
| EX2 | Windows/Reach | 19 XNB | 1 level XML |

Each endpoint has the exact four Quartz MS SpriteFonts, seven SoundEffects and eight textures. The
two stages intentionally use different seven-file sound sets and slightly different level XML.
The resulting 76 authentic XNBs and all hashes are retained under `xna4-build/` and `evidence/` in
the artifact root; none are checked into a guessed port before the stage decision.

This sample does **not** use `System.Xml.Serialization.XmlSerializer` and is independent of
`SAMPLES-DEC-008`. `GameplayScreen` loads the copied `LevelDefinitions.xml` with LINQ to XML, while
high scores and saved level state use line-oriented text in isolated storage.

## Windows Phone lifecycle boundary

EX2 subscribes to all four `Microsoft.Phone.Shell.PhoneApplicationService` events. Its observable
contract includes:

- `Launching` loading the persistent level;
- `Deactivated` preserving both current level and partial sequence progress;
- `Activated` restoring that state and selecting the resume/pause startup route;
- `Closing` committing or cleaning the saved game according to the user's pause-screen decision.

The local XNA Wine prefix has no Windows Phone SDK or application host. The offline Win7 VM that
successfully produced authentic SongProcessor outputs is available and network-isolated, but its
current VirtualBox configuration reports `nested-hw-virt="off"`; no authentic WP7 emulator run or
tombstone/reactivation capture was claimed in this audit. Live CNA contained no
`PhoneApplicationService`, `StartupMode` or corresponding activation event types **when this was
written**; see the 2026-09-09 re-audit below, which supersedes that sentence. A desktop-only
startup substitution, manually raised sample event, or ordinary file save presented as tombstoning
would violate the zero-workaround rule.

## Re-audit 2026-09-09: the lifecycle blocker has mostly gone, and EX1 now compiles here

### `Live CNA contains no PhoneApplicationService` is no longer true

That sentence above was accurate when it was written. Six days later, `SAMPLE-071` built the
service: `cnanext` `bbd9ef0ef` (2026-09-07), *"add the Microsoft::Phone application-lifecycle
service"*, `modules/phone`, nine tests. What EX2 asks for, measured member by member:

| EX2 uses | CNA |
|---|---|
| `PhoneApplicationService.Current` | present |
| `.State` — `ContainsKey`, indexer, `Remove`; **five** of EX2's nineteen files | present, as `StateDictionary` |
| `Launching`, `Activated`, `Deactivated`, `Closing` | present |
| `LaunchingEventArgs`, `ActivatedEventArgs`, `DeactivatedEventArgs`, `ClosingEventArgs` | present |
| `StartupMode` / `StartupMode.Launch` | **absent** |

`StartupMode` is used **once**, at `EX2/MemoryMadnessGame.cs:72`, to open `MainMenuScreen` on a fresh
launch and `PauseScreen(true)` on a resume. CNA's own design note answers it already: there is no
operating-system shell above the game, and `AttachEXT` raises `Launching` because a process that has
reached that point started without preserved state. So the honest CNA value is a constant `Launch` —
a documented property plus a two-value enum, not a subsystem. Nothing else in either endpoint needs
anything from `Microsoft.Phone`.

That does not make the tombstoning *reference* problem go away: there is still no way here to run
the original under a Windows Phone host and watch it be killed and resumed. It does change what is
being asked for, from "a lifecycle subsystem CNA does not have" to "one property, plus a decision
about how to qualify a transition this machine cannot produce".

### EX1 type-checks against the official XNA 4.0 assemblies

The retained build evidence covered **content only** — the sources had never been compiled here, so
"EX1 is completely playable" rested on the lab document's word. `scripts/compile-original-sources.sh`
now compiles both endpoints unchanged against the official XNA 4.0 assemblies in the local GAC:

| endpoint | sources | lines | result |
|---|---:|---:|---|
| EX1 | 12 | 2,667 | **clean**, `MemoryMadnessEx1.dll` `c57d6970c5bbbbd721c428833c755d213f6e3165fc035011cafab3179b4f2115` |
| EX2 | 19 | 4,455 | fails on `Microsoft.Phone` only — 5 `using` sites and the 4 event-args types, no other error |

EX2's failure is the expected one and is itself the measurement: this machine has XNA reference sets
for Windows/x86 and Xbox360 and no Windows Phone assemblies at all, so `Microsoft.Phone.Shell` has
nothing to resolve against. The absence of any *other* error is what makes the table above complete
rather than indicative.

Retained log: `evidence/original-source-compile.log`.

### `System.Xml.Linq` is not a blocker, and is not the DEC-008 question

Both endpoints read their levels at runtime with LINQ to XML —
`XDocument.Load(@"Content\Gameplay\LevelDefinitions.xml")`, then
`Descendants(XName.Get("Level"))` and `Descendants(XName.Get("Pattern"))`
(`Level.cs`, `GameplayScreen.cs` in each endpoint). Sharp Runtime has a whole `xml-linq` module:
`XDocument::Load`, `XContainer::Descendants(const XName&)`, `XName::Get`, `XElement::Attribute` and
`getValueProperty` are all present. This is `System.Xml.Linq`, not the
`System.Xml.Serialization.XmlSerializer` work `SAMPLES-DEC-008` is about.

One consequence worth noting for a port: that path is a **loose file read straight off disk**, with a
backslash separator, not a content-pipeline asset. It is the twentieth item in each configuration's
content manifest — copied, not compiled — which is why the manifest has 20 entries where 19 XNBs
exist.

### The two endpoints differ in content as well as in code

Relevant to the "one product or two" decision below: EX1 and EX2 do not share a content set. Seven
of the nineteen assets are different sounds (`BleepBlue`/`BleepGreen`/`BleepRed`/`BleepYellow`/
`DoorHit`/`Fail`/`Success` against `BlueButton`/`GreenButton`/`RedButton`/`YellowButton`/
`DefeatBuzzer`/`HighScoreScreen`/`LevelComplete`), and their `LevelDefinitions.xml` files hash
differently. Treating EX2 as "EX1 plus polish" would drop content, not only code.

`Microsoft.Xna.Framework.Media` is an unused `using` in both endpoints — nothing in either uses
`Song`, `MediaPlayer` or `MediaLibrary`.

## Owner decisions required before implementation

1. Under `SAMPLES-DEC-005`, choose either both runnable products as separate CNA sample targets, or
   the final EX2 endpoint plus retained/source-delta evidence as an explicit owner-approved scope
   decision. Do not silently call EX1 a non-product.
2. Establish a usable authentic Windows Phone lifecycle reference/qualification route, or approve a
   precise cross-platform lifecycle boundary together with the general CNA/Sharp Runtime work it
   requires. This is shared with SAMPLE-065; it is not permission for a MemoryMadness-only shim.

After those decisions, port the selected endpoint(s), retain touch-only interaction, exact XNB/XML
content, asynchronous loading, Guide name entry, persistence and lifecycle behavior, then qualify
unchanged XNA, native OPENGLES3 and real-Chrome WEBGL2 representative flows. Do not invent mouse or
keyboard gameplay, remove persistence, make loading synchronous, substitute loose assets, or reduce
the final endpoint to EX1 behavior.
