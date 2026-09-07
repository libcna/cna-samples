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

## Qualification

The threaded `WEBGL2` bundle builds and passes a headless-Chrome gate: `crossOriginIsolated`,
a real `WebGL 2.0 (OpenGL ES 3.0 Chromium)` context, 600 post-interaction `requestAnimationFrame`
callbacks, and zero exceptions, unhandled rejections and HTTP errors. The gate drives the game by
keyboard, which is the only input the original has, and captures the main menu, the quest details
and quest log a new session opens, the map after both are dismissed, `StatisticsScreen` on the
character-management key, the `QuestNpcScreen` the party leader walks into, and the in-session
menu.

An earlier version of this gate looked like it had found a defect -- pressing Space, which
`GameplayScreen` binds to character management, left the frame unchanged -- and the cause turned
out to be the gate, not the port. `scripts/probe-space.sh` settles it: it delivers Space four ways
(CDP `rawKeyDown`, CDP `keyDown` with `text`, and a page-dispatched `KeyboardEvent` on the window
and on the canvas) and every one of them opens `StatisticsScreen`, fully populated -- name, class,
level, HP/MP, the four modifiers and the weapon/armour ranges. What the earlier gate had actually
done was walk first: the party leader's first step south reaches Shed-darr the Wise, the quest
destination, so `QuestNpcScreen` was on top and swallowed the Space. Its frames were byte-identical
because that dialogue is static, which is what gave the false reading away -- the map animates, so
two captures of it can never match. The gate now presses Space while the gameplay screen is the top
one and captures the NPC dialogue as its own step. The probe and its five frames are kept under
`evidence/space-key-probe/`.

The other thing that run taught the gate: `Page.captureScreenshot` returns the last composited
surface, and under `--use-angle=swiftshader` on three cores that surface can be a second or more
behind the game, so a capture could show the state before the key rather than after it. `capture()`
now takes two screenshots and keeps the second, and every interaction gets five to six seconds to
land.

`scripts/capture-web.sh` runs the gate. Chrome is `--headless=new`, so it never puts a window on a
display, and both gate scripts pin the browser and the driver to three cores with `taskset` --
`--use-angle=swiftshader` rasterizes WebGL on the CPU and takes every core it can find otherwise.
`scripts/capture-web-firefox.sh` and `scripts/firefox-smoke.mjs` carry the same walk over WebDriver
BiDi for Firefox, which needs a private Xvfb because a headless Firefox has no GPU-backed WebGL 2
here.

## The native run, and the save path the web target cannot reach

`scripts/capture-native.sh` drives the native `OPENGLES3` Release build through the same walk the
web gate takes, and then one step further: it saves a game. `scripts/capture-native-load.sh`
starts a second process and loads it back. That pair is the only place the persistence work is
actually exercised end to end, because Emscripten has no `StorageDevice` for the sample to write
through -- the browser gates can reach every screen but the save.

The saved session is written to `<XDG_DATA_HOME>/game/RolePlayingGame/AllPlayers/`, as
`SaveGame1.xml` beside `SaveGameDescription1.xml`, which is the original's layout. Its content is
the state the captures show rather than a plausible-looking document: `Maps/Map001` at tile
`(9,10)` facing `South`, `currentQuestIndex` 1 with `currentQuestStage` `InProgress` after the
first quest completed, and a `PartySaveData` holding Kolatt at `characterLevel` 2 with
`experience` 40, `partyGold` 10 and the two equipment asset names. Loading it back in a fresh
process brings the party up at level 2 with 40/150 experience and the restored weapon and armour
values, not at a new game's level 1 -- so the ten `XmlSerializer` routes round-trip through
`StorageDevice`/`StorageContainer` rather than merely writing something.

Both scripts drive the game by comparing frames, not by sleeping. The first attempt reused the web
gate's fixed timings and did not survive the move to native: the native build loads its content
several times faster, so the same Escape presses walked through more popups than they did in the
browser and the run ended up in the quest log while it believed it was in the save menu. Every
press now has to change the picture or the run aborts and captures what it actually found.

Two environment facts are worth keeping, because both cost a run here:

- **`SDL_VIDEODRIVER=x11` is the setting that keeps the window off the owner's screen, and
  unsetting `WAYLAND_DISPLAY` does nothing at all.** This was measured here rather than assumed,
  because the first version of this note asserted the opposite. Four runs of this sample's own
  binary on a Wayland host, counting X11 windows on the Xvfb and on `:0`:

  | `WAYLAND_DISPLAY` | `SDL_VIDEODRIVER` | window on the Xvfb |
  |---|---|---|
  | set | `x11` | yes |
  | set | unset | no |
  | unset | unset | no |
  | unset | `x11` | yes |

  `SDL_VIDEODRIVER=x11` wins over a set `WAYLAND_DISPLAY`, and clearing `WAYLAND_DISPLAY` on its
  own changes nothing, because SDL3's wayland backend connects to the default `wayland-0` socket
  in `XDG_RUNTIME_DIR` whether or not the variable names it. In both driverless rows the game did
  not fail -- it came up on the owner's real compositor, which is what the earlier runs in this
  session did. The renderer log tells the two apart for free: the real GPU reports MSAA up to 8x,
  llvmpipe on the Xvfb reports 4x.
- **A rootless Xwayland cannot be screen-captured by the X11 tools.** On `:0` here, `import
  -window root`, per-window `import`/`xwd` and `ffmpeg -f x11grab` all fail, because there is no
  composited root pixmap and each client is its own wayland surface. An Xvfb root is a real
  pixmap, so the same capture works there. That, and not only politeness, is why the gate runs on
  a private display.

The Firefox gate (`scripts/capture-web-firefox.sh`, `scripts/firefox-smoke.mjs`) passes on Firefox
140 ESR over WebDriver BiDi: cross-origin isolated, WebGL 2, 600 post-interaction rAF callbacks,
nine distinct frame hashes, and `moduleAbort` null with no page errors. That last part is the
result worth recording -- this Firefox has no `Atomics.waitAsync`, which is what the 6.0.3
emscripten mailbox assertion needed to avoid aborting a threaded bundle, and the 6.0.9 toolchain
this bundle is built with carries the upstream fix.

## Still open

- Nothing on this sample. The three items this file listed as outstanding on 2026-09-07 -- the
  interactive native run, its save/load round trip, and the Firefox gate -- are all done and their
  evidence is under `evidence/cna-native-opengles3-qualified/` and
  `evidence/cna-web-webgl2-firefox/`. Neither SAMPLE-067 defect can reach this sample: it starts
  no background loading thread (zero `Thread`/`Task` uses in the original, zero `std::thread` in
  the port), and the bundle is built with emsdk 6.0.9.
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
