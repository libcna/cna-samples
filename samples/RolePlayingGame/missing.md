# SAMPLE-070 — Role Playing Game

## Status

The `SAMPLES-DEC-008` blocker is closed. `System::Xml::Serialization::XmlSerializer`
exists in `sharp-runtimenext` (`modules/xml-serialization`), and this port uses it for the
save/load routes rather than a handwritten RPG-specific parser.

Every item the 2026-08-31 audit listed under *"Existing port deviations that remain to be
removed"* has been removed. What follows records what that took and what is still open.

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

Every game and data class of the first two projects now has a counterpart under `src/`; a
basename sweep over the original tree finds no class without one. The third project is the
build-time content pipeline, which CNA's own pipeline replaces — the port consumes the XNBs it
produced rather than reimplementing the writers.

## Content

`Content/` carries the 1,004 official XNB assets produced by the unchanged content projects
through the XNA 4.0 toolchain, plus the compiled XACT banks:

```text
RPGAudio.xgs       472 bytes
Sound Bank.xsb   1,312 bytes
Wave Bank.xwb 8,610,184 bytes
```

Nothing is parsed from the 281 source XML files at runtime and no loose `.wav` stands in for a
cue: `AudioManager` is the original XACT component and CNA's `AudioEngine`/`WaveBank`/`SoundBank`
load the banks above (27 cues).

## What was restored

- **Save/load.** `SaveSession`, `LoadSession`, `DeleteSaveGame`, `RefreshSaveGameDescriptions`
  and the storage region of `Session.cs`, with `PartySaveData`, `PlayerSaveData` and
  `SaveGameDescription`, over CNA's `StorageDevice`/`StorageContainer` and `Guide`.
- **Combat.** `Combatant`, `CombatantPlayer`, `CombatantMonster`, `ArtificialIntelligence`,
  `CombatAction` and its five concrete actions, and the real `CombatEngine` — turn order, the
  delay state machine, targeting with adjacent targets, rising damage/healing numbers, selection
  rings, flee odds and the victory/loss/fled endings.
- **Screens.** `ListScreen<T>` and the six screens built on it, plus `QuestDetailsScreen`,
  `StatisticsScreen`, `PlayerSelectionScreen`, `MessageBoxScreen`, `SaveLoadScreen`,
  `ControlsScreen` and `HelpScreen`; and the eight screens that had been standing in as text
  substitutes (`DialogueScreen`, the `NpcScreen<T>` family, `ChestScreen`, `InnScreen`,
  `LevelUpScreen`, `RewardsScreen`, `GameOverScreen`, `LoadingScreen`).
- **HUD and menus.** The texture-backed `Hud` with its combat action menu, and `MainMenuScreen`
  with the plank art, description panel, Save/Load entries and exit confirmation.

## Still open

- The native `OPENGLES3` and real-browser `WEBGL2` qualification runs have not been performed in
  this session; the owner asked that the game not be launched on this machine's displays. The
  native Release target builds clean and the earlier smoke run in this session reached the
  textured main menu and loaded all three XACT banks.
- The original Windows/HiDef executable still cannot be captured unchanged: it stops at the
  retired `GamerServicesComponent`/Games for Windows LIVE initialization boundary under Wine even
  with the official XNA Live Proxy and the GFWL redistributable installed. That failure is
  preserved under `evidence/xna4-original-windows-hidef-live-failure/`, and the reference
  captures come from the diagnostic host in `xna4-diag/` (one commented-out
  `Components.Add(new GamerServicesComponent(this));`), under
  `evidence/xna4-diagnostic-host-windows-hidef/`. That is labelled diagnostic evidence and is not
  represented as an unchanged original-runtime pass.

## Deviations

Recorded in `diff.md`.
