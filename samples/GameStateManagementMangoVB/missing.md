# SAMPLE-126 — `GSMSample_4_0_Mango_VB` audit and owner decision

## Status

The complete Visual Basic source, project, content, compiled-surface and reference-runtime audit is
finished. This directory is Microsoft's VB generation of the C# Mango product audited as
SAMPLE-125, but it is not behaviorally or platform-metadata identical. No alias, duplicate C++
target or partial merge was added before the owner chooses a shared versus VB-faithful product
boundary under `SAMPLES-DEC-005`. It also inherits SAMPLE-125's unresolved Phone lifecycle and
closed AOT screen-factory boundary.

## Complete inventory and pairing

The retained directory has 46 files / 313,396 bytes. Its 24 `.vb` files contain 2,948 lines,
compared with 24 `.cs` files / 3,429 lines in SAMPLE-125. All 24 logical files pair one-to-one after
normalizing `.vb`/`.cs` and `My Project`/`Properties`; every pair declares the same type set. The
only string-literal differences are:

- the two assemblies' language-specific GUIDs and the application's 2010/2011 copyright metadata;
- `MessageBoxScreen` spelling the same two newline-prefixed prompts with `vbLf` plus plain literals
  instead of C# `\n` literals.

All six VB project files have C# counterparts with identical normalized compile-item sets. The
three library projects each compile six units; the application projects select ten Phone units or
fourteen Windows/Xbox units exactly as in SAMPLE-125. This proves complete product decomposition,
not complete semantic identity.

## Measured language and behavior differences

The exact Win7 `/vbruntime*` assembly and the C# assembly were inspected through the same reflection
and IL-behavior probes. The reusable library has ten real setter-visibility differences: VB makes
`ControllingPlayer`, `EnabledGestures`, `IsExiting`, `IsPopup`, `IsSerializable`, `ScreenManager`,
`ScreenState`, `TransitionOffTime`, `TransitionOnTime` and `TransitionPosition` publicly settable;
the C# generation uses internal, protected or protected-internal setters.

Two differences affect compiled behavior or reachable output rather than only visibility:

- `OptionsMenuScreen` concatenates its enum with VB `&`. The exact VB assembly reports
  `Preferred ungulate: 1` initially and `Preferred ungulate: 2` after selection, whereas C# reports
  `Dromedary` and `Llama`. The independent reflection probe and OCR of the actual changed-options
  frames both prove this reachable distinction.
- C# `GameScreen.EnabledGestures` has a 34-byte setter that, when the screen is active, calls
  `TouchPanel.set_EnabledGestures`. VB generated a nine-byte auto-property setter with no calls.
  The shipped Phone menu sets the property in its constructor before activation, so the ordinary
  shipped menu path does not prove a missed gesture; nevertheless, the reusable library contract
  is observably different for an active screen.

The application surface also adds a private VB `Program` constructor, exposes `Main` publicly
instead of privately and calls the menu position property `_Position` instead of `Position`.
Several VB drawing-coordinate conversions use `CInt`, whose rounding semantics are not identical to
C# truncating casts; the separate target must not silently normalize such language behavior without
the owner's ruling. Compiler-generated backing-field/event names were measured but are not treated
as game-facing differences.

## Phone and shell metadata differences

The VB project is explicitly a Windows Phone OS 7.1 deployment while the C# manifest says 7.0. The
VB manifest carries fourteen capabilities versus nine and uniquely requests identity user/device,
camera, contacts and appointments. It also uses the template title/token `WindowsPhoneGame1`, a
different ProductID and a different 64x64 `GameThumbnail.png`; C# uses
`GameStateManagementSample`. `Background.png`, `Game.ico` and `AppManifest.xml` remain identical.

These are real installation/shell identity differences even though the game source is otherwise
paired. A shared portable C++ product has to state that these retired Phone deployment details are
reference metadata; a separate VB-faithful product must deliberately preserve or map them.

## Content provenance

All six files in `Content/`, including the content project itself, are byte-identical to SAMPLE-125.
The same-host official XNA 4 Windows/Reach pipeline therefore produces a byte-identical five-XNB
tree for C# and VB. The exact Win7 project build also produces all five assets; only
`gamefont.xnb` differs from the Wine-host build because Win7 rasterizes its installed Segoe UI font
differently (136,366 versus 70,830 bytes). The other four XNBs are byte-identical across hosts.
This is an environment/font result, not a language asset difference.

Current `cna-content` successfully converts all five exact Win7 XNBs, including the larger
SpriteFont, and every CNB passes `cna_tool_cnb_info`. Thirteen focused live CNA Texture2D,
SpriteFont and XNB pipeline tests pass. Content is not the representation blocker.

## Authentic original builds and execution

The offline/headless Win7 VM built the unchanged `GameStateManagementSample (Windows).sln` as
`Debug|x86` through authentic MSBuild/XNA Game Studio with `/m:8`. The log proves the project-owned
`/vbruntime*` embedding mode, successful library/game/content builds, zero warnings and zero errors.
The exported `GameStateManagement.dll`, `GameStateManagementSample.exe`, PDBs and all five XNBs are
retained.

That exact Win7 output was then run with official XNA 4 under isolated Xvfb/Wine. Automated input
captures main menu, options, changed options, gameplay, moved gameplay, pause, quit confirmation,
returned main menu and exit confirmation; the 800x480 game exits through its own menu with code
zero. A separately reproducible local compiler build passes the same nine-state sequence and
exposes the same enum/setter behavior. No browser or window was opened on the owner's real display.

The same offline VM selected the exact `Debug|Windows Phone` solution. With `/m:1` it reports the
authoritative diagnostic that this installation of XNA Game Studio does not support Windows Phone
XNA v4.0 projects; the content target consequently has no Phone platform mapping. No XAP, Phone
assembly, emulator, touch or tombstoning execution is claimed. The VM ended `poweroff` with
`nic1` through `nic8` still `none`.

## CNA/Sharp Runtime and existing-port boundary

SAMPLE-126 contains the same Mango screen-stack serialization, runtime type restoration,
`PhoneApplicationService`, touch and tombstoning requirements documented for SAMPLE-125. Live
Sharp Runtime's general reflection remains intentionally out of scope, and live CNA/Sharp Runtime
does not provide the retired Phone lifecycle/shell state source. A closed compile-time screen
factory could be a faithful AOT representation only after that product boundary is approved.

The complete SAMPLE-072 C++ port represents the older desktop/Xbox generation. It lacks all seven
new Mango runtime types already measured by SAMPLE-125 and therefore cannot be relabelled as this VB
product. No sample-local lifecycle, XML, gesture or type-reflection workaround is acceptable.

## Owner choice required

Choose one boundary together with SAMPLE-125:

1. Treat the C# and VB Mango directories as one shared language-neutral C++ product, select and
   document the canonical behavior where the official translations disagree, and retain this VB
   source/build/manifest as reference support data. The shared product still needs the complete
   Mango lifecycle and closed AOT screen-factory decision.
2. Create a distinct VB-faithful target. Preserve its numeric enum display, public setter surface,
   `_Position` identity, conversion semantics and Phone 7.1 shell metadata deliberately, while also
   implementing every shared Mango/Phone behavior rather than copying only the Windows menu.
3. Retain this physical directory as a separately audited language/platform reference and accept an
   evidence-backed non-port, while any approved C# Mango product proceeds under SAMPLE-125.

Do not call the variants identical, silently fix Microsoft's VB translation to the C# behavior,
or duplicate the existing older Game State Management port without this ruling.

## Reproduction and evidence

Artifact root:

`/rv/tmp/samples/SAMPLE-126-GSMSample_4_0_Mango_VB/`

Important retained material:

- `xna4-original/GameStateManagementSample/` — exact complete VB snapshot;
- `scripts/win7-build-original.cmd` and `win7-build-phone.cmd` — authentic offline MSBuild paths;
- `xna4-build/win7-windows/` — exact exported DLL/EXE/PDB/content product;
- `scripts/build-original.sh` — reproducible local compiler/content build (the host compiler uses
  `/vbruntime+`; the authoritative Win7 output proves exact `/vbruntime*`);
- `scripts/capture-original.sh` — isolated nine-state original execution;
- `scripts/compare_sources.py`, `compare_variants.py`, `AssemblySurfaceDump.cs` and
  `VariantBehaviorProbe.cs` — pair, project, API and behavior evidence;
- `evidence/win7-windows-msbuild.log`, `win7-phone-msbuild.log` and
  `win7-reference-boundary.txt` — exact build and final VM boundary;
- `evidence/xna4-original-win7-windows/` and `xna4-original-windows-reach/` — both nine-state runs;
- `evidence/source-pair-audit.tsv`, `library-api.diff`, `game-api.diff`,
  `cs-behavior-probe.txt`, `vb-behavior-probe.txt` and options OCR — exact divergences;
- `evidence/variant-audit.tsv` — complete project/content/artwork/capture comparison;
- `evidence/cna-xnb-transcode.log` and `cna-focused-content-tests.log` — 5/5 conversion and 13/13
  focused tests.

Re-run the host-side qualification with:

```bash
/rv/tmp/samples/SAMPLE-126-GSMSample_4_0_Mango_VB/scripts/qualify.sh
```

All compilation/content worker settings in these scripts are capped at eight for this session.
