# SAMPLE-121 — `CardsStarterKit_4_0_VB` audit and owner decision

## Status

Fresh source, content and reference-runtime audit is complete enough to require a representation
decision under `SAMPLES-DEC-005`. This physical directory is Microsoft's Visual Basic translation
of the same Cards Starter Kit already ported and fully qualified as C++ SAMPLE-069. No duplicate
target, alias or second copy of the game was invented without the owner's ruling.

## Complete source comparison

The complete retained Visual Basic directory contains 251 files. Its game and reusable cards
framework contain 49 `.vb` files / 7,491 lines, compared with 47 `.cs` files / 8,742 lines in
`CardsStarterKit_4_0`. Every one of the 47 C# sources has one corresponding VB source. The two
additional files are identical `VBCoreHelper.vb` modules, one in each assembly; their `Fix`
overloads implement the truncation used where the C# source has direct numeric casts.

The reproducible paired-source audit strips comments/regions without discarding strings and
compares declared types plus every string literal:

- all 47 source pairs are present;
- all non-AssemblyInfo string-literal multisets are identical;
- both AssemblyInfo pairs differ only in language/project metadata, copyright year, culture and
  GUID;
- the only declared-type spelling difference is VB `BlackJackGame` versus C# `BlackjackGame`.

A second audit compiles the unchanged sources and reflects their declared type/member surfaces.
After normalizing the VB project's root-namespace effect, all game-facing constructors, methods,
properties and events match. The complete difference is compiler/language mechanics:

- a VB `Module` exposes one implicit constructor for each of `MathUtility` and `UIUtilty`;
- the two VB `WithEvents` fields in `BlackJackGame` expose private compiler properties;
- VB emits `Program.Main` as public while C# emits it as private.

The VB game project has `RootNamespace=Blackjack`, so its `Namespace GameStateManagement` sources
compile as `Blackjack.GameStateManagement`; the C# sources and the existing C++ port expose the
namespace globally as `GameStateManagement`. The code imports and uses the resulting namespace
consistently. This is a real language/project identity difference but it creates no distinct game
behavior. A separate language-faithful C++ product would have to decide whether to reproduce that
nested identity instead of silently aliasing the existing port.

## Content and documentation comparison

Both variants contain 182 PNG/WAV/SpriteFont files. There are 181 common relative paths. Every
runtime content payload is byte-identical. Only project/shell artwork differs:

- both variants have different `CardsGame/CardsGame/Background.png` and `GameThumbnail.png`;
- VB alone has `PhoneGameThumb.png`;
- C# alone has `CardsFramework/Background.png`.

The normalized compile-item contracts are identical in both content projects: 89 items in
`BlackjackContentHiDef.contentproj` and 90 in the Phone `BlackjackContent.contentproj`. The HTML
documentation is also byte-identical, with SHA-256
`4a17267f1a9a34242a5f4431d716c5e97d8b402426cafb9100bb2ebdeaa63289`.

The official Microsoft XNA 4.0 Windows/HiDef pipeline rebuilt all 89 selected items from the
unchanged VB snapshot. Its complete XNB tree is byte-for-byte identical, including names and
payloads, to the retained official output used by SAMPLE-069. Thus a second C++ content tree would
only duplicate the same 89 files.

## Authentic original execution

`scripts/build-original.sh` maps the unchanged Windows project settings to the offline .NET 4 VB
compiler, builds `CardsFramework.dll` and `Blackjack.exe`, and runs the unchanged HiDef content
project through XNA 4. The direct compiler harness links the installed VB runtime while retaining
both upstream `VBCoreHelper` modules; no source file is edited. The build and all 89 content items
pass reproducibly.

`scripts/capture-original.sh` runs that executable through WineD3D on isolated Xvfb display
`:181`, never on the real desktop. The automated run passes:

```text
main menu -> Play -> betting -> $25 chip -> Deal -> Stand/result
          -> Escape/pause -> Quit to main menu -> Exit
```

The game exits normally. `unexpected-console.log` is empty; `console.log` retains only Wine helper
messages caused by the script intentionally shutting down Xvfb after the game exits. All seven
captures are 800x480. Two deterministic states are byte-identical to the separately executed C#
reference:

```text
after $25 chip   1eac921d193f21e381b4882740f74e47f8dce63b2e5fe8271cebd69446a752ff
returned menu    bcd132133f744197a7b69ffc3c164cf47565e8894d65cc6cf1c9ac62672df313
```

The dealt cards and result are intentionally randomized, so those captures prove reachable state
and clean interaction rather than deterministic pixels.

## Relationship to the existing C++ product

SAMPLE-069 already contains the complete 47-file logical game/framework translation, the exact 89
XNBs, all inactive Phone/Xbox branches, rules, AI, audio, screen-stack persistence and C++
language mechanics. It passed Debug and Release OPENGLES3 plus a real-Chrome WEBGL2 run through
the same bet/deal/stand/result/pause/menu path, including 600 browser animation frames and clean
audio/content loading. This audit found no VB-only gameplay, content identifier, conditional
branch, algorithm or visual state missing from that port.

No CNA or Sharp Runtime defect was exposed, no source change was made in either dependency and no
sample workaround was added. An independent native/browser gate for SAMPLE-121 would require first
choosing whether a second product is wanted; running the already-qualified SAMPLE-069 binary under
a second name would not test any additional upstream behavior.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-121-CardsStarterKit_4_0_VB/`.

- `xna4-original/` is the complete byte-for-byte 251-file upstream snapshot;
- `xna4-build/windows-hidef/` contains the unchanged VB assemblies, XNA references and all 89
  official XNBs;
- `evidence/build-original.log`, `original-binaries.sha256` and `official-xnb.sha256` retain build
  and provenance evidence;
- `evidence/source-pair-audit.tsv`, four `*-surface.tsv` files and two surface diffs retain the
  source/member comparison;
- `evidence/variant-audit.tsv` and `audit-summary.tsv` cover inventory, both content projects,
  XNB equality, documentation and deterministic visual equality;
- `evidence/xna4-original-windows-hidef/` contains the seven captures, result, Xvfb log and the
  empty unexpected-console log;
- `scripts/build-original.sh`, `capture-original.sh` and `audit.sh` reproduce the qualification
  offline.

## Owner decision required

Choose one:

1. accept that the already-complete C++ SAMPLE-069 is the faithful language-neutral product for
   both C# and VB deliveries, retaining this audit as proof rather than duplicating it;
2. classify the VB directory as retained language/reference support data for the same product;
3. authorize a second C++ target/source surface, defining whether it must reproduce VB's
   `BlackJackGame` casing, nested `Blackjack.GameStateManagement` identity and VB compiler-helper
   surface despite identical runtime behavior and content.

Until that decision, copying or aliasing SAMPLE-069 as a nominal VB target would add no upstream
behavior and would obscure the only real distinction: source-language/project identity.
