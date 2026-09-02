# SAMPLE-143 — Role Playing Game 4.0 Phone audit and blockers

## Status

`SAMPLE-143` is a complete and materially distinct Windows Phone 7 edition of
the Role Playing Game sample. It is not represented by the existing desktop
`samples/RolePlayingGame` target, and it is not a faithful port yet.

The audit stops under `SAMPLES-DEC-002`, `SAMPLES-DEC-005` and
`SAMPLES-DEC-008`. No phone-flavoured reduced game, loose-content substitute,
handwritten save serializer or asset replacement was added.

## Authoritative source and measured delta

The exact 1,191-file, 53,021,685-byte source snapshot is preserved at:

```text
/rv/tmp/samples/SAMPLE-143-RolePlayingGame_4_0_Phone/
  xna4-original/RolePlayingGame_4_0_Phone/
```

Its sorted aggregate SHA-256 is:

```text
4357821594df4c5c0398073165f3c271cd1b79cf03485b3238aa55a4a6d18dab
```

The Phone delivery has 140 C# units and 34,064 source lines. A path-by-path
comparison with the complete desktop/Xbox `SAMPLE-070` source finds only 64
identical C# files, 73 changed shared files and three Phone-only files:

- `ExtensionMethods.cs`, which resolves gesture direction;
- `GameScreens/SwitchMapScreen.cs`, which performs a draw-before-load map
  transition and preserves/restores map state around content unload/reload;
- `ScaledVector2.cs`, which establishes the Phone coordinate and draw scaling.

The source delta is 3,152 added and 1,830 removed lines. Important product
differences include:

- a fixed 800x480 fullscreen Reach presentation and 30 Hz target timestep;
- Tap, VerticalDrag, HorizontalDrag and Flick input plus Phone Back/Start
  handling across 21 logical actions;
- a second `StaticContent` manager and explicit unload/reload map transition;
- isolated application storage plus an in-memory map cache;
- direct `SoundEffect`/`SoundEffectInstance` playback of 27 WAV assets instead
  of the desktop XACT graph.

There is no actual Activated/Deactivated or tombstoning hook in this delivery;
the storage and map-cache code must not be misreported as a complete Phone
lifecycle implementation.

The solution contains Phone game, Phone data, Windows data, processor and
Phone content projects. A stale `RolePlayingGameWindows.csproj` is excluded
from the solution and references a nonexistent content project, so it is not a
second runnable Windows product.

## Content is a separate Phone generation

The Phone content project has 1,032 items:

| Processor | Items |
|---|---:|
| `TextureProcessor` | 714 |
| sample `PassThroughProcessor` object graphs | 281 |
| `SoundEffectProcessor` | 27 |
| `FontDescriptionProcessor` | 10 |

Against the desktop generation, 992 shared content files differ and only 30
are byte-identical. All 281 paired XML documents preserve their element
structure, but 100 contain Phone-specific values. The measured changes include
148 frame dimensions, 100 source offsets, 16 tile sizes and three music cue
names.

Of 714 paired raster assets, only two retain both the same dimensions and the
same decoded pixels; the other 712 are genuinely resized or redrawn. Phone
images contain 28,439,220 pixels and 19,073,105 encoded bytes versus
128,734,757 pixels and 64,744,146 bytes in the desktop generation. The most
common conversion is 485 sprites from 64x64 to 30x30. A contact sheet covering
all 715 Phone raster assets was generated and inspected; the character,
animation, equipment, map, effect and UI sets are coherent rather than damaged
or placeholder data.

This evidence rules out silently pointing the desktop port at one allegedly
duplicate content tree.

## Authentic XNA reference result

The owner's offline Win7 SP1 VM was booted with every VirtualBox network
adapter disabled. It has Visual Studio 2010 SP1 and XNA Game Studio 4.0 desktop
targets, but no Windows Phone SDK targets. The unchanged solution therefore
fails honestly in `Microsoft.Xna.GameStudio.targets` because platform
`Windows Phone`, framework `v4.0` is unsupported. The VM was shut down normally
and remains powered off.

A credential-free custom host around the official XNA pipeline assemblies was
then used inside that same offline VM. The full unmodified 1,032-item Phone
content graph reaches `Fonts/DebugFont.spritefont` and fails because its
`Arial Narrow` family is unavailable. That proprietary font is not
installed even on the reference Win7 image, and the audit did not substitute a
lookalike font or edit the asset.

A clearly labelled diagnostic build omitted only that single SpriteFont item.
It produced 1,031 of the remaining 1,031 assets successfully:

```text
files:                 1,031
bytes:           145,652,005
platform headers:      XNBm:1,031
manifest SHA-256: b29fe970d3120a5f39dd9ca5b74cc5b2f87dd87c31753d549d86827649bd3406
```

`XNBm` proves that these are Windows Phone products rather than desktop XNBs.
The missing `DebugFont.xnb`, unavailable Phone SDK/runtime reference, and
Phone-versus-desktop product representation require an explicit owner scope
decision under `SAMPLES-DEC-002`/`005`.

## CNA content boundary

The current CNA `cna-content` tool processed the authentic Phone XNB tree
with eight workers. It built exactly the 750 stock products (714 textures,
nine available fonts and 27 sounds) and rejected exactly the 281 sample-owned
custom object graphs:

```text
Built: 750  Skipped: 0  Failed: 281
CNB files:            750
CNB bytes:    145,835,810
manifest SHA-256: 97973b3699a3431b83b6400d94a794289e7366bda999c84801d012e8e089b6ce
```

The failures cover the complete closed RPG reader set: Armor, FixedCombat,
Chest, Weapon, QuestNpc, Spell, Monster, Map, Item, Quest, Store,
CharacterClass, Player, Inn, QuestLine and GameStartDescription. This is not a
claim that all such XNBs require a new generic CNA pipeline. The accepted
runtime-sample policy permits pregenerated exact XNBs, but the complete C++
product must register and implement the matching closed AOT readers. The
existing desktop port instead reads all 281 source XML files at runtime through
`src/Xml/XmlNode.hpp` and `src/Data/ContentLoader.hpp`; SAMPLE-070 already
classifies that implementation as a forbidden sample workaround.

Focused CNA content/runtime tests pass 156/156 using SDL's offscreen platform
and a real Mesa OpenGL ES 3.2 context. No CNA source change was needed by this
audit.

## Shared `XmlSerializer` blocker

The Phone `Session.cs` contains 34 live `new XmlSerializer(...)` call sites,
up from 20 in the desktop generation. It serializes and deserializes the same
core save graphs (`PlayerPosition`, world-entry collections, modified chests,
party/player data and save descriptions) and additionally duplicates key graph
routes for the Phone map cache used by `SwitchMapScreen`.

The existing desktop port omits that behavior. Adding another RPG-specific
serializer would violate the no-workaround rule, so this sample joins
`SAMPLES-DEC-008`. The owner has explicitly kept that decision deferred and is
separately managing possible Sharp Runtime XML work; this audit neither changes
nor pre-approves that branch.

## Owner decisions and work after unblocking

The owner must choose one Phone product boundary:

1. a separate faithful Phone-generation target with its exact 800x480,
   gestures, scaling, map-switch, content and audio semantics;
2. an explicitly unified Role Playing Game product that still exposes and
   qualifies a complete Phone generation rather than erasing the delta; or
3. an evidence-backed historical Phone non-port.

If a runtime product is selected, the owner must also provide/authorize the
exact `Arial Narrow` input or explicitly approve a documented DebugFont asset
change, and resolve `SAMPLES-DEC-008`. The implementation must then remove the
existing XML loader workaround, consume the authentic object graphs through
closed readers, restore all missing desktop foundations identified by
SAMPLE-070, add the Phone-only behavior above and pass native OPENGLES3 plus
real-browser WEBGL2 qualification.

## Evidence

Reproducible scripts and retained products are under the artifact root. Key
records include:

- `evidence/comparison-summary.txt` and the source/content delta tables;
- `evidence/content-analysis.txt` and `evidence/asset-preview.png`;
- `evidence/build-win7-phone-solution.log`;
- `evidence/build-win7-content-full.log`;
- `evidence/build-win7-content-minus-debug-font.log`;
- `evidence/xnb-manifest.tsv`, `cnb-manifest.tsv` and
  `cna-failure-readers.tsv`;
- `evidence/cna-phone-transcode.log` and
  `evidence/cna-focused-content-tests.log`;
- `evidence/build-summary.txt` and `evidence/win7-reference.txt`.

Port qualification is intentionally not claimed. No CNA or Sharp Runtime file
was modified for SAMPLE-143.
