# SAMPLE-125 — `GSMSample_4_0_Mango` audit and owner decision

## Status

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
(443) and `menufont` (38,062). Current `cna-content` converted all five to CNB with zero failures,
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

## Live CNA and Sharp Runtime boundary

Live Sharp Runtime has LINQ-to-XML `XDocument` and isolated-storage surfaces, so XML syntax and file
storage alone are not the blocker. CNA/Sharp Runtime do not expose the Windows Phone
`PhoneApplicationService`, `IsApplicationInstancePreserved`, shell state dictionary or matching
launch/deactivate/tombstone event source.

The original `ScreenFactory` additionally calls `Activator.CreateInstance(Type)` after resolving
an assembly-qualified type name saved in XML. Sharp Runtime's documented permanent deviation keeps
general .NET reflection out of scope: its `Activator` supports compile-time template construction,
not runtime creation from `System::Type`. A faithful AOT C++ product could use an explicit closed
screen-name/factory registration analogous to other approved reflection-to-AOT translations, but
that representation and the Phone lifecycle contract must be selected and documented; the existing
template `Activator` must not be presented as dynamic .NET reflection.

## Owner choice required

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

Run the retained qualification with:

```bash
/rv/tmp/samples/SAMPLE-125-GSMSample_4_0_Mango/scripts/qualify.sh
```

The retained scripts cap relevant compiler/runtime worker counts at eight for this session.
