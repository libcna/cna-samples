# SAMPLE-070 — Role Playing Game audit and blocker

## Status

`SAMPLE-070` is not a faithful XNA 4.0 port. The fresh audit on 2026-08-31
reopened the historical implementation and marks the sample blocked under
`SAMPLES-DEC-008` until the owner chooses a shared
`System.Xml.Serialization.XmlSerializer` direction.

No sample workaround was added during this audit. In particular, the existing
handwritten XML parser is not accepted as the implementation of XNA content or
save-game serialization.

## Authoritative source and measured scope

The complete source is preserved at:

```text
/rv/tmp/samples/SAMPLE-070-RolePlayingGame_4_0_Win_Xbox/
  xna4-original/RolePlayingGame_4_0_Win_Xbox/
```

The source snapshot contains 1,191 files and has the sorted aggregate SHA-256:

```text
82f4da98bc3f7b2d9e7da1a7795fe15b35ae23bff02255e6c1753300ee7c24a4
```

The three original C# projects contain 137 source files and 32,744 lines:

| Project | C# lines |
|---|---:|
| `RolePlayingGame` | 22,016 |
| `RolePlayingGameData` | 8,793 |
| `RolePlayingGameProcessors` | 1,935 |

The current C++ sample contains only 72 source units and 6,351 lines. A
basename comparison alone finds 27 completely absent game classes, before
counting partial method bodies and simplified behavior.

## Original build and runtime evidence

The unchanged Windows/HiDef game, data assembly and all 39 custom content
processor/writer sources compile through the local XNA 4.0 toolchain. The
unchanged content projects build all 1,004 ordinary XNB assets. The original
XACT project also compiles successfully to:

```text
RPGAudio.xgs       472 bytes
Sound Bank.xsb   1,312 bytes
Wave Bank.xwb 8,610,184 bytes
```

This disproves both historical pipeline blockers. The generated XNB object
graphs and XACT banks are the assets a faithful port must consume; direct
runtime parsing of the 281 source XML files and loose-WAV `SoundEffect`
substitution are forbidden sample workarounds.

The exact unchanged executable reaches the retired
`GamerServicesComponent`/Games for Windows LIVE initialization boundary in
Wine even with the official XNA Live Proxy and GFWL redistributable installed.
That failure is preserved under:

```text
evidence/xna4-original-windows-hidef-live-failure/
```

For visual reference only, `xna4-diag/RolePlayingGame.cs` differs from the
original by commenting out the single
`Components.Add(new GamerServicesComponent(this));` statement. No other game
source or content was changed. This diagnostic host reaches and captures:

- the textured main menu and quest details;
- the quest log and overworld gameplay;
- the full statistics/equipment screen;
- the session menu with New, Save, Load, Controls, Help and Exit;
- Controls, Help and the exit-confirmation dialog.

The successful reference is under:

```text
evidence/xna4-diagnostic-host-windows-hidef/
```

It is labelled diagnostic evidence and is not represented as an unchanged
original-runtime pass.

## Confirmed framework boundary: `XmlSerializer`

Live CNA already implements `StorageDevice`, `StorageContainer`, `Guide` and
`GamerServicesComponent`. Therefore the old claim that save/load could be
dropped because CNA lacked storage is false.

The reachable original save/load implementation in `Session/Session.cs`
constructs `System.Xml.Serialization.XmlSerializer` at 20 call sites (ten
serialize and ten deserialize) for ten object-graph routes:

- `PlayerPosition`;
- `List<WorldEntry<Chest>>` for map and quest state;
- `List<WorldEntry<FixedCombat>>` for map and quest state;
- `List<WorldEntry<Player>>`;
- `List<ModifiedChestEntry>` for map and quest state;
- `PartySaveData`, including `PlayerSaveData`;
- `SaveGameDescription` discovery/readback.

The serializers are used in both directions by `LoadSession`, `SaveSession`
and `RefreshSaveGameDescriptions`. `MainMenuScreen` exposes Load, while the
in-session menu exposes Save, so these are observed reachable features rather
than dormant source.

Live `sharp-runtimenext` at `4a49afb0cfe6a41e6e0af0bb62dc5175976731bb`
has no `System::Xml::Serialization::XmlSerializer`. Implementing the .NET
reflection-driven contract in C++ requires a reusable member-description and
construction policy, exactly the unresolved architecture already identified
by SAMPLE-014 and SAMPLE-066. A handwritten RPG-specific save parser would
repeat the forbidden workaround and is not authorized.

This sample is therefore attached to `SAMPLES-DEC-008`. The owner must choose
between a reusable Sharp Runtime serializer or an explicit, documented policy
for sample-specific serializers before this port can satisfy the completion
rules.

## Existing port deviations that remain to be removed

The serializer decision is the point at which autonomous work stops; it is not
the only defect in the old port. Once unblocked, the port still needs a
complete reimplementation pass:

- remove `src/Xml/XmlNode.hpp` and `src/Data/ContentLoader.hpp`, then load the
  exact processor-produced XNB object graphs through `ContentManager`;
- replace `AudioManager.hpp`'s loose `SoundEffect`/WAV emulation with the
  original `AudioEngine`, `WaveBank`, `SoundBank` and cue behavior;
- restore the absent save data classes, `SaveLoadScreen` and
  `MessageBoxScreen` rather than dropping save/load and confirmation;
- restore the absent combatants, AI and `Combat/Actions` hierarchy; the
  current roughly 270-line Attack/Defend/Flee text resolver is not the
  original animated combat engine and omits spells/items;
- restore all reachable list, inventory, equipment, spellbook, statistics,
  player-selection, store-buy/store-sell, controls and help screens;
- restore the original texture-backed HUD, menus and secondary-screen chrome
  instead of plain text panels;
- perform the required line-by-line source audit and native OPENGLES3 plus
  real-browser WEBGL2 qualification after those changes.

Representative classes absent from the current port include
`ArtificialIntelligence`, `CombatAction`, `Combatant`, `CombatantPlayer`,
`CombatantMonster`, all five concrete combat actions, `PartySaveData`,
`PlayerSaveData`, `SaveGameDescription`, `SaveLoadScreen`, `StatisticsScreen`,
`InventoryScreen`, `EquipmentScreen`, `SpellbookScreen`,
`PlayerSelectionScreen`, `StoreBuyScreen`, `StoreSellScreen`,
`ControlsScreen` and `HelpScreen`.

## Qualification result

The audit and original-reference build/capture gates pass. Port qualification
does not pass and was intentionally not claimed: the current C++ sample retains
multiple prohibited workarounds and cannot implement its reachable save/load
contract without the unresolved shared serializer decision. No CNA or Sharp
Runtime files were modified for SAMPLE-070.
