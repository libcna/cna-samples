# SAMPLE-125 — `GSMSample_4_0_Mango` audit and owner decision

## Current-head analysis — 2026-10-03 — 🛑 owner product/lifecycle decision

Owner: **"124 ponech cancelled analyzuj 125"**. SAMPLE-124 is separately ⛔; SAMPLE-125
is independently analyzed, not owner-cancelled. This is **XNA 4.0, C#**, with Windows/Reach,
Xbox 360/HiDef and Windows Phone/Reach projects. It is a revised Game State Management library
and demonstration application, rather than completed SAMPLE-072 unchanged.

All **46 files / 320,750 bytes** match upstream byte for byte. The full **24 C# units / 3,429
lines**, three solutions, six C# projects, content project, manifests, documentation and Ms-PL
were reviewed. Windows selects six library plus fourteen app units; Phone selects six plus ten.
All six source images/icons decode. The readme describes distinct desktop/Phone experiences,
fast app switching and tombstoning.

Fresh comparison with SAMPLE-072 confirms **14 changed mapped sources and nine new units**;
only application `AssemblyInfo.cs` is unchanged among fifteen mapped units. Seven tracked new
declarations remain absent from the current 072 C++ tree. `Button.cs` also declares
`BooleanButton`; seven tracked declarations are not a count of all added types. Only `blank`
and `gradient` match 072 among five source assets/XNBs. The other changes are the 800×480 RGB
background, Segoe UI Bold game font and 20-point menu font.

The unchanged Windows library/game and five assets rebuild with genuine .NET4 and official
XNA4 Windows/Reach. **5/5 fresh XNBs equal this sample's historical generation**, with decoded
headers/reader tables. Retained CNA host tools (hashes/timestamps recorded, not rebuilt or claimed
as current-HEAD runtime gates) produce **5/5 historically byte-identical CNBs**; five inspections
pass. The earlier 13 focused tests remain historical.

The fresh isolated Xvfb/WineD3D reference covers and visually verifies nine full **800×480**
captures: main menu, options, changed options, gameplay, movement, pause, quit confirmation,
returned main menu and exit confirmation. It exits through the menu **0**. The game log has only
two X-connection shutdown messages after stopping the owned display. Teaching-placeholder
gameplay and Phone music/SFX toggles without audio are intentional upstream demonstrations.
No native C++/WEBGL2 port, Phone runtime or Xbox runtime is qualified by this Windows run.

### Corrected dependency boundary

The old claim that CNA lacks `PhoneApplicationService`, `IsApplicationInstancePreserved` and
the state dictionary is **superseded**. CNA `db68149e3` provides a process singleton, boxed-object
dictionary and Launching/Activated/Deactivated/Closing events. `AttachEXT(Game)` bridges Game
events: Launching on attachment, suppression of initial activation, then
`ActivatedEventArgs(true)` after deactivation of the still-running process. This supports a
preserved instance; it does not establish OS process-kill/relaunch, Activated(false), restored
shell state or the original tombstoning contract.

Source persists screen type names/order/player identity into isolated `ScreenManagerState.xml`.
Restoration uses `Type.GetType(assemblyQualifiedName)`, application `IScreenFactory`,
`Activator.CreateInstance(Type)` and `Enum.Parse`. XML/storage exist. Sharp `db86514c` explicitly
excludes general reflection (AGENTS.md permanent deviations); template Activator/minimal RTTI
Type are not this runtime mechanism. A closed AOT registry and explicit player-name conversion
are feasible, but supported identities, constructor/error behavior and lifecycle mapping need
the owner-selected boundary and a documented deviation.

One source-only corner must survive that design: base `GameScreen.IsSerializable` defaults true,
while Phone compiles `LoadingScreen` with only a private parameterized constructor. If serialized
on deactivation, the original empty-constructor factory cannot restore it. This is a source
inference, **not a freshly observed Phone runtime defect**; silently skipping that screen or
marking it nonserializable changes the original.

The earlier Win7 unsupported-Phone build and 13/13 tests remain historical, not new SDK/runtime
checks. No new VM run, sample/dependency implementation, test, publication or pruning was needed.

### Owner options

1. **Separate Mango product:** retain 072 and implement the changed Windows product plus all
   library/Phone behavior; define the AOT factory and native/browser lifecycle contract. Authentic
   Phone restoration evidence remains a prerequisite for claims of Phone parity.
2. **Shared Mango upgrade:** explicitly replace the selected shared 072 generation, adding all
   Mango behavior and the same factory/lifecycle qualification; this changes an accepted product.
3. **Cancel the separate row:** retain all originals, content and evidence as archive data. This
   requires an independent owner decision.

A separate target is the clearest boundary if this generation is wanted. A desktop-only subset
cannot be reported as a completed translation of the whole physical directory.

### Fresh evidence and reproduction

Stable root: `/rv/tmp/samples/SAMPLE-125-GSMSample_4_0_Mango/`.
Dated helpers: `scripts/current-head-20261003/`; official products:
`xna4-build/current-head-20261003/windows-reach/`; new CNBs: `cna-build/current-head-20261003/`.
`evidence/current-head-analysis-20261003/` retains inventory/full source/project contracts,
readme/licence provenance, image metadata, comparison TSVs, XNB readers, tool/dependency hashes,
build/inspection logs, nine original captures, review and synchronized final heads. Use dated
`run-reference.py`, `compare_base.py` and `audit-current.py`. All earlier generations remain.
Initial helper failures (path ordering and unavailable unrtf) are recorded as setup diagnostics;
the final audit asserts all stated counts.

## Historical audit — 2026-09-02

The following original audit results/paths remain for provenance. The current dependency and
qualification claims above supersede them. Do not run the old qualification helper as a new gate.

### Historical status

The complete source, content, original-runtime and live-dependency audit is finished. This physical
directory is a materially revised Mango/Windows Phone generation of Game State Management, not a
byte-identical packaging variant of the already-complete SAMPLE-072 desktop/Xbox product. No alias,
partial merge, Phone substitute or second C++ product was invented without the owner's representation
and platform-lifecycle decision under `SAMPLES-DEC-005`.

## Complete upstream inventory

The retained upstream snapshot has 46 files / 320,750 bytes, including all solutions, projects,
documentation, license and content. Its 24 C# files contain 3,429 lines. The directory supplies a
reusable `GameStateManagement` library and a `GameStateManagementSample` application with Windows,
Xbox 360 and Windows Phone solutions/projects rather than one monolithic game project.

The Windows product compiles 20 source units: six library units and fourteen application units. The
Phone product compiles sixteen: six library units and ten application units selected through Phone
project conditions. The complete snapshot is byte-identical to
`/rv/tmp/XNAGameStudio/Samples/GSMSample_4_0_Mango/GameStateManagementSample`; the empty retained
snapshot diff proves that the reference source was not patched for the audit.

## Relation to SAMPLE-072

SAMPLE-072 comes from `GSMSample_4_0_WIN_XBOX` and has 15 C# files / 2,520 lines. All fifteen
filename-mapped sources were compared line by line with the Mango generation. Only the application
`AssemblyInfo.cs` is byte-identical; the remaining fourteen mapped files changed. Mango adds nine
source files with no counterpart in the older tree:

- library `IScreenFactory.cs`, `InputAction.cs` and its own `AssemblyInfo.cs`;
- application `Program.cs` and `ScreenFactory.cs`;
- `Button.cs`, `PhoneMainMenuScreen.cs`, `PhoneMenuScreen.cs` and `PhonePauseScreen.cs`.

The new behavior is not language scaffolding. It includes:

- reusable `InputAction` mappings for keys/buttons and held versus newly-pressed semantics;
- `GameScreen.Activate(bool instancePreserved)`, `Deactivate()`, `Unload()` and
  `IsSerializable` lifecycle contracts;
- `ScreenManager.Deactivate()` serialization of the serializable screen stack, controlling-player
  identity and assembly-qualified screen type names to isolated `ScreenManagerState.xml`;
- `ScreenManager.Activate(...)` restoration through an application-provided `IScreenFactory`;
- Phone launch, activate, deactivate and preserved-instance handling through
  `PhoneApplicationService`;
- Phone touch buttons/menu/pause screens and touch-directed gameplay movement;
- transient player/enemy position preservation in `PhoneApplicationService.Current.State`.

The existing SAMPLE-072 C++ tree contains none of the seven new runtime types `InputAction`,
`IScreenFactory`, `ScreenFactory`, `Button`, `PhoneMainMenuScreen`, `PhoneMenuScreen` and
`PhonePauseScreen`. The declaration-aware scan intentionally does not confuse the new `Button`
class with XNA's existing `Buttons` enum. SAMPLE-072 is still complete for its own older upstream
directory, but it cannot be cited as a complete translation of this Mango directory.

## Content comparison

Both generations compile five logical assets. Only `blank.png` and `gradient.png`, and their
official XNB outputs, are byte-identical. Mango deliberately changes the other three:

- `background.png` is an 800x480 RGB composition rather than SAMPLE-072's 853x480 RGBA image;
- `gamefont.spritefont` changes from Segoe UI Mono to Segoe UI;
- `menufont.spritefont` changes from size 23 to size 20.

The unchanged Mango content project completed through Microsoft's XNA 4.0 Windows/Reach pipeline,
producing five XNBs: `background` (1,536,187 bytes), `blank` (251), `gamefont` (70,830), `gradient`
(443) and `menufont` (38,062). The then-current `cna-content` converted all five to CNB with zero failures,
and every output passes `cna_tool_cnb_info`. Thirteen focused live CNA Texture2D, SpriteFont and
XNB pipeline tests pass. Thus the representation decision is not blocked by these assets.

## Authentic Windows reference

`scripts/build-original.sh` compiles the unchanged Windows configuration with Microsoft's .NET 4
C# compiler and official XNA 4.0 Windows/Reach assemblies, first as
`GameStateManagement.dll` and then as `GameStateManagementSample.exe`. The exact official content
project is processed by the retained XNA pipeline. The build and all five XNBs pass.

`scripts/capture-original.sh` runs that executable with official XNA 4.0 under isolated Xvfb/Wine,
WineD3D and software GL; it never opens a browser or window on the owner's real display. The
reference window is exactly 800x480. Automated input reaches and captures all nine representative
states:

1. main menu;
2. options menu;
3. changed options;
4. gameplay;
5. moved gameplay state;
6. pause menu;
7. quit confirmation;
8. returned main menu;
9. exit confirmation.

The game exits through its own menu with exit code zero. This proves the selected Windows product,
its changed visual assets, input actions and normal screen flow. It does not prove Windows Phone
tombstoning.

## Authentic Windows Phone boundary

The offline/headless `win7` VM was booted with all eight network adapters set to `none`. The shared
folder exposed only this audit artifact. An authentic Rebuild of the unchanged
`GameStateManagementSample (Windows Phone).sln` selected its default `Debug|Windows Phone`
configuration, but the installed XNA Game Studio reports:

> Your installation of XNA Game Studio does not support this project (XNA Platform = 'Windows
> Phone', XNA Framework Version = 'v4.0').

The unsupported solution also cannot map the content project's target platform. No Phone assembly,
XAP, emulator run, tombstoning result or touch-runtime result is claimed. The VM was shut down and
verified `poweroff`, still with `nic1` through `nic8` disconnected.

## Historical CNA and Sharp Runtime boundary

Live Sharp Runtime has LINQ-to-XML `XDocument` and isolated-storage surfaces, so XML syntax and file
storage alone are not the blocker. The audit then reported absent Phone services. That claim is
obsolete: the current analysis above distinguishes the implemented preserved-instance bridge
from the still-unqualified OS tombstoning/restoration path.

The original `ScreenFactory` additionally calls `Activator.CreateInstance(Type)` after resolving
an assembly-qualified type name saved in XML. Sharp Runtime's documented permanent deviation keeps
general .NET reflection out of scope: its `Activator` supports compile-time template construction,
not runtime creation from `System::Type`. A faithful AOT C++ product could use an explicit closed
screen-name/factory registration analogous to other approved reflection-to-AOT translations, but
that representation and the Phone lifecycle contract must be selected and documented; the existing
template `Activator` must not be presented as dynamic .NET reflection.

## Historical owner options

Choose one product boundary before implementation:

1. Upgrade the existing shared Game State Management C++ product to the Mango generation, retaining
   SAMPLE-072 as provenance while adding every Mango library/application behavior, exact changed
   content, an explicit closed AOT screen factory and an approved native/browser lifecycle mapping.
2. Keep SAMPLE-072 intact and create a separate Mango target preserving its 800x480 Windows product,
   all unique source behavior and a deliberately specified Phone lifecycle/touch/tombstoning
   equivalent. Decide whether the Phone product is a separate target when an authentic Phone
   reference environment becomes available.
3. Retain this physical directory as distinct Mango/Phone reference/support data while SAMPLE-072
   remains the selected portable product, explicitly accepting an evidence-backed non-port for this
   row.

Do not merge only the easy Windows changes, discard the Phone branches, replace tombstoning with a
sample-local file hack or call SAMPLE-072 an alias. Any reusable platform lifecycle gap belongs in
CNA/Sharp Runtime, while the closed AOT factory declaration belongs to the selected sample product
only after the owner approves that boundary.

## Reproduction and evidence

Artifact root:

`/rv/tmp/samples/SAMPLE-125-GSMSample_4_0_Mango/`

Important retained material:

- `xna4-original/GameStateManagementSample/` — exact complete upstream snapshot;
- `scripts/build-original.sh` — unchanged Windows library/game and official XNA content build;
- `scripts/capture-original.sh` — isolated nine-state original execution;
- `scripts/compare_base.py` — complete source/content/XNB/project comparison with SAMPLE-072;
- `scripts/qualify.sh` and `scripts/audit.sh` — reproducible focused qualification and assertions;
- `evidence/xna4-original-windows-reach/` — nine screenshots, hashes, geometry and exit result;
- `evidence/base-source-comparison.tsv`, `mango-unique-sources.tsv`,
  `base-content-comparison.tsv` and `base-xnb-comparison.tsv` — exact delta evidence;
- `evidence/existing-port-unique-type-scan.tsv` — declaration-aware SAMPLE-072 port check;
- `evidence/build-original.log` and `cna-xnb-transcode.log` — official/CNA content builds;
- `evidence/cna-focused-content-tests.log` — 13/13 focused tests;
- `evidence/win7-phone-msbuild.log` and `win7-reference-boundary.txt` — authentic unsupported-Phone
  diagnostic and final offline VM state.

Historical qualification command (old paths/worker cap, not the fresh workflow):

```bash
/rv/tmp/samples/SAMPLE-125-GSMSample_4_0_Mango/scripts/qualify.sh
```

The retained scripts cap relevant compiler/runtime worker counts at eight for this session.
