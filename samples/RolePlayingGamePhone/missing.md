# SAMPLE-143 — Role Playing Game 4.0 Phone audit and blockers

## Status

**Owner cancelled 2026-10-03: "oznac 143 cancellled a analyzuj 144".**
SAMPLE-143 remains an evidence-backed historical Phone variant; no CNA port,
merged generation or DebugFont/reference work will be produced. Preserve the
originals, official content products and analysis; no cleanup was requested.

`SAMPLE-143` is a complete and materially distinct Windows Phone 7 edition of
the Role Playing Game sample. It is not represented by the existing desktop
`samples/RolePlayingGame` target.

The owner authorized a detailed reassessment on 2026-10-03. The earlier
`SAMPLES-DEC-008` serializer blocker is now resolved by the shared
Sharp Runtime serializer and the completed SAMPLE-070 implementation. The
owner then selected an evidence-backed historical non-port. The authentic
Phone reference route and unavailable Arial Narrow input for `DebugFont`
therefore remain recorded external inputs rather than pending work. No
phone-flavoured reduced game, loose-content substitute, handwritten save
serializer or asset replacement was added.

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
product must register and implement the matching closed AOT readers.

That work is no longer an unimplemented subsystem. Completed SAMPLE-070 now
loads its authentic custom XNB graphs through 37 closed reader classes and no
longer parses the 281 source XML files at runtime. A fresh comparison of the
two original data projects finds the same 37 reader classes in both
generations: 34 are identical after whitespace/comment normalization. The
three changed readers retain the same serialized field order and only adjust
Phone post-load behavior:

- `AnimatingSpriteReader` loads the same texture and fields, while Phone draw
  code applies `ScaledVector2.DrawFactor`;
- `CharacterReader` applies the Phone scale factor to its two source-offset
  corrections;
- `MapReader` delegates fixed-combat post-load setup to a helper but reads the
  same graph.

The inspected Phone `Maps/Map015.xnb` reader table also records the same
`RolePlayingGameDataWindows` assembly and reader names used by SAMPLE-070.
Therefore the 281 standalone `cna-content` failures are a tool-without-sample-
registration diagnostic, not evidence that a new generic content subsystem is
required. A Phone product still has to adapt the three reader behaviors and
prove every graph at runtime.

Focused CNA content/runtime tests pass 156/156 using SDL's offscreen platform
and a real Mesa OpenGL ES 3.2 context. No CNA source change was needed by this
audit.

## Shared `XmlSerializer` foundation

The Phone `Session.cs` contains 34 live `new XmlSerializer(...)` call sites,
up from 20 in the desktop generation. It serializes and deserializes the same
core save graphs (`PlayerPosition`, world-entry collections, modified chests,
party/player data and save descriptions) and additionally duplicates key graph
routes for the Phone map cache used by `SwitchMapScreen`.

SAMPLE-070 now implements the same save graphs through Sharp Runtime's shared
`XmlSerializer`, including explicit C++ reflection metadata and verified
save/load. The Phone map cache repeats those graph routes in memory, so it adds
wiring and qualification work rather than a new serialization design.

Current Sharp Runtime also supplies `IsolatedStorageFile`,
`IsolatedStorageFileStream` and browser IDBFS persistence. The Phone port must
still exercise fresh-process native save/load, browser reload persistence and
map-cache unload/reload, but `SAMPLES-DEC-008` is no longer a scope blocker for
this sample.

## Current runtime readiness — 2026-10-03

Inspection at CNA `db68149e3` and Sharp Runtime `db86514c` found the defining
Phone dependencies already present:

- `TouchPanel` exposes Tap, HorizontalDrag, VerticalDrag and Flick queues, with
  tested SDL gesture delivery through the public queue used by both targets;
- `ContentManager.Unload()` exists for the Phone map transition;
- direct `SoundEffect` and looped `SoundEffectInstance` playback are covered by
  the shared audio implementation;
- Sharp Runtime supplies application isolated storage, XML, Unicode memory
  streams and the harmless C++ counterpart of `GC.Collect()`.

The original has no Activated/Deactivated handlers or tombstone restoration,
so a new Phone lifecycle service is not required to reproduce this delivery.
No new large CNA or Sharp Runtime subsystem has been identified. These are
source/test inspections, not a runtime qualification of SAMPLE-143.

## Implementation scope and recommendation

The safest faithful representation is a separate `RolePlayingGamePhone`
target. A merged switch inside SAMPLE-070 would have to select between two
large content generations and two input/layout/audio behaviors, making both
products harder to audit.

SAMPLE-070 provides a strong base: 103 C++ source units / 18,393 lines, the
closed content readers, XML metadata, complete combat/screens and native/web
qualification already exist. The Phone merge must nevertheless review 73
changed C# units and add the three Phone-only units. The original delta is
3,152 added and 1,830 removed lines; just eight files (`Session`,
`AudioManager`, `InputManager`, `Hud`, `ListScreen`, `TileEngine`,
`CombatEngine` and `StatisticsScreen`) contain 52.5% of the changed-line
churn. Much of the rest is systematic Phone scaling/layout work, but it occurs
across gameplay and data draw paths and cannot be replaced by globally scaling
the final frame.

The port is therefore medium-to-large sample work, not framework research.
The hard completion gates are the exact `DebugFont`, a labelled original
Phone reference (or an owner ruling on the unavailable SDK), all 1,032 content
loads, touch paths, map unload/reload, save/load, direct audio, combat and the
full native OPENGLES3 plus real-browser WEBGL2 walk. The uncompressed authentic
Phone content is about 146 MB, so web bundle size is also materially larger
than SAMPLE-070's approximately 65 MB content bundle.

## Historical options superseded by cancellation

The owner selected option 3 on 2026-10-03. The alternatives below are retained
only to explain the measured scope; they are no longer pending work.

The audited options were:

1. a separate faithful Phone-generation target with its exact 800x480,
   gestures, scaling, map-switch, content and audio semantics;
2. an explicitly unified Role Playing Game product that still exposes and
   qualifies a complete Phone generation rather than erasing the delta; or
3. an evidence-backed historical Phone non-port.

Had a runtime product been selected, it would also have required either the
exact `Arial Narrow` input or explicit approval of a documented DebugFont
asset change. Its implementation would have branched from the completed SAMPLE-070
foundation, consume the authentic Phone object graphs through its adapted
closed readers, add the Phone-only behavior above and pass native OPENGLES3
plus real-browser WEBGL2 qualification.

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
