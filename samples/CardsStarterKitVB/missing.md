# SAMPLE-121 — `CardsStarterKit_4_0_VB` audit and owner decision

## Status

Fresh source, content and reference-runtime audit is complete enough to require a representation
decision under `SAMPLES-DEC-005`. This physical directory is Microsoft's Visual Basic translation
of the same Cards Starter Kit already ported and fully qualified as C++ SAMPLE-069. No duplicate
target, alias or second copy of the game was invented without the owner's ruling.

## Current-head re-analysis — 2026-10-03

**🛑 Owner classification pending.** Owner: "ponech cancelled a analyzuj 121" cancels the
preceding SAMPLE-120, not this independent VB variant. Unlike that image pack, this is a real
**XNA 4.0 runnable Blackjack game and reusable CardsFramework**, with Windows, Xbox and Phone
projects. Its projects explicitly name `XnaFrameworkVersion=v4.0`.

Both complete source trees still match their retained snapshots: **VB 251 files / 5,377,193
bytes; C# 247 files / 5,421,790 bytes**. The 49 VB sources (7,491 lines) still correspond to the
47 C# sources (8,742 lines), plus two identical `VBCoreHelper` modules. The renewed paired audit
finds the same single type-casing difference and only two AssemblyInfo string differences.
Runtime identifiers, menu text, content contracts and inactive target branches retain their
previously audited relationship. The original HTML and complete Ms-PL remain byte-identical
between deliveries. Both asset inventories still contain 182 entries, with 181 common paths;
only two common project-artwork files differ, plus one variant-only shell image on each side.

Fresh official XNA4 Windows/HiDef content generation rebuilds all **89 XNBs byte-identically**
to the earlier VB output, the retained C# official output and SAMPLE-069's checked-in content.
Both content-project contracts, including all Compile metadata, match (89 desktop / 90 Phone).
The unchanged VB game/framework sources compile again with the genuine .NET4 VB compiler and
XNA4 assemblies. Reflection against the retained C# assemblies reproduces the same normalized
member-surface differences. Correcting the earlier description: `MathUtility` and `UIUtilty`
are VB **NotInheritable classes with implicit public constructors**, versus C# static classes;
they are not VB Modules. The two extra `VBCoreHelper` sources are the actual Modules.
No gameplay code instantiates those utility classes. Other measured differences remain private
WithEvents properties, Main visibility, `BlackJackGame` casing and the VB root-namespace identity.

Three fresh source-reference runs pass Play → betting → $25 → Deal → Stand/resolved hand →
Escape/pause → Quit/menu → Exit, with **exit 0**, seven 800×480 captures each and empty unexpected
console logs: rebuilt VB HiDef, retained C# reference, and the additional VB Reach build below.
All six pairwise comparisons of menu, after-chip and returned-menu against the VB HiDef run
have **AE=0, byte-identical decoded RGBA pixels**. Random dealt hands/results establish reachable
states and matching layout, without a random-hand pixel-equality claim. The first simultaneous
attempts did not qualify: VB failed during host/GDI font initialization; the C# capture lost its
window. Both subsequent individual runs pass, with original failure logs retained separately;
concurrency as the cause is not established. The C# reference binary was reused, not rebuilt.

### Original project metadata and reference-build limitation

The older direct-compiler harness's label "Windows/HiDef" describes its selected pipeline and
generated runtime-profile resource. The actual **Windows game and library projects declare
Reach and `VBRuntime=Embed`**; game source does not override GraphicsProfile. The corresponding
**C# Windows projects declare HiDef**; the Embed property belongs to the VB build. This is a
real project-configuration difference in addition to the source-language mechanics.
A separate Reach generation retains all original
source and project Compile contracts: 86/89 XNBs differ from HiDef only in the header profile flag;
the three SpriteFonts have different atlas packing/size. Its complete game route and all three
static states still match the HiDef VB/C# references exactly.

All three font XNB pairs also pass a complete glyph comparison through the retained CNA host
content tools: **285/285 glyphs** have byte-identical character maps, glyph/crop/kerning arrays,
font settings and RGBA pixels. Atlas widths remain 256; Reach pads heights 144→256, 196→256 and
120→128. Every atlas row within the original HiDef height is byte-identical; the difference is
the padded atlas height. All six resulting font CNBs pass the container inspector. This is a
CPU content comparison using existing tools, not a rebuilt CNA or renderer/runtime gate.

The installed original-era **vbc 10.0.30319.1** does not support `/vbruntime*`: it emits BC2007,
ignores that option and produces assemblies referencing Microsoft.VisualBasic. Thus the Reach
run is explicitly a **linked-runtime source build**, not proof of the exact upstream Embed/VS
project build. Both generations and compiler help/warnings/assembly references are retained.
No VB source, numeric conversion, gameplay or content is rewritten to bypass the limitation.
Authentic full-project embedded-runtime qualification remains open; it is a reference compiler/
build-contract limitation, not a newly demonstrated CNA gameplay/API defect.

### Product boundary and recommendation

SAMPLE-069's C++ game/framework source remains unchanged since `ef49afb` (2026-08-31); its
existing AOT/ownership mechanics remain documented in its diff.md. Its 2026-09-26 native/browser,
audio and gallery results are **retained qualifications, not fresh current-HEAD SAMPLE-121
gates**. No second C++ target, alias, implementation or new runtime tests were added here.
The source/content/member/reference evidence establishes no additional VB gameplay requirement
beyond that product; the language/project/assembly identity still needs an explicit scope ruling.

Recommend **cancel the separate VB duplicate and retain all original/reference evidence**.
Alternatively the owner can accept one explicitly shared language-neutral SAMPLE-069 product,
or authorize an independent VB-faithful C++ identity/surface with its own native/WEBGL2 gates.
Shared acceptance must explicitly address the VB namespace/API identity and Reach-versus-HiDef
project/content boundary. A separate faithful VB product must retain its Reach configuration and
original Reach content as well as the VB names; the equal glyph pixels do not make the serialized
atlas dimensions or graphics-profile metadata identical.
The separate route would reproduce the same 47 logical game/framework units and 89 assets plus
the VB namespace/casing/utility/helper boundary, adding no measured scene, rule, input or content.
No new large CNA/Sharp subsystem is established by this analysis. No pruning was requested.

Fresh evidence:
`/rv/tmp/samples/SAMPLE-121-CardsStarterKit_4_0_VB/evidence/current-head-analysis-20261003/`:
`source-inventory.json`, complete compact `vb-source-review.txt` / `cs-source-review.txt`,
`source-pair-audit.tsv`, `projects.json`, `project-source-contracts.json`, `content-contracts.json`,
`assets-and-docs-comparison.json`, full readme/licence body, `xnb-equivalence.json`, both build
logs, four fresh surface reports/diffs, `compiled-surface-comparison.json`, `binary-provenance.json`,
`profile-comparison.json`, `profile-font-equivalence.json`, retained font-tool hash, six font
container reports, compiler help and assembly-reference report, the three successful
capture directories, first-attempt logs, `runtime-comparison.json`, review and final heads.
Reproduction helpers are in `scripts/current-head-20261003/`; fresh assemblies/pipeline products
are in `xna4-build/current-head-20261003/`, with the separate Reach probe under `project-reach/`.
Earlier snapshots/products/evidence remain at their original paths.

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

- VB utility classes expose one implicit constructor for each of `MathUtility` and `UIUtilty`;
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

## Historical original source execution

`scripts/build-original.sh` maps the unchanged Windows source to the offline .NET 4 VB
compiler, builds `CardsFramework.dll` and `Blackjack.exe`, and runs the unchanged HiDef content
project through XNA 4. The direct compiler harness links the installed VB runtime while retaining
both upstream `VBCoreHelper` modules; no source file is edited. The build and all 89 content items
passed in that historical audit. Its HiDef/linked-runtime settings differ from the upstream
Reach/Embed project metadata as clarified above; it is not an exact VS project-build claim.

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

## Historical relationship to the existing C++ product

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

1. **recommended:** cancel the separate VB duplicate, retaining every source, project, reference
   product and audit record;
2. accept one shared SAMPLE-069 language-neutral C++ product, explicitly ruling on the VB identity
   and Reach/HiDef metadata/content differences before claiming shared completion;
3. authorize a separate VB-faithful C++ target/surface that preserves `BlackJackGame` casing,
   nested `Blackjack.GameStateManagement`, utility/helper APIs and Reach profile/content, resolves
   the embedded-runtime reference-build limitation and passes its own native/WEBGL2 gates.

Until that decision, copying or aliasing SAMPLE-069 as a nominal VB target would add no upstream
behavior and would obscure the only real distinction: source-language/project identity.
