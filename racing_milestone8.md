# Racing Milestone 8 — XACT, screens and persistence

## Result

Milestone 8 completed on 2026-09-03. The CNA port now follows the original desktop
launch-to-race-to-menu lifecycle using only the canonical XNA 4 source and its
authentic content-pipeline products. It loads the supplied XACT banks, traverses
the complete LIFO screen stack, persists settings/highscores/replays and handles a
real race outcome without a sample-side framework workaround.

## Frozen revisions

- CNA: `056e57d478f8e6accfa9124337803e735b39f1e4`
- cna-samples pre-Milestone-8 revision: `08514c53f36046dfe2573ed4ed68430caf6f2981`
- sharp-runtime: `6baf0925e0bf7805a363836deed90543d26f6d28`
- meta-gl: `20c8b2dc5bb80e32706784066db9fd9e15b3f46a`
- EasyGL: `deda7a426c3c166c0e03a4790f1ede610e2e46fb`
- renderer/profile: CNA `OPENGL33`, HiDef, Mesa OpenGL 4.6 core

## Screen and loading closure

The original Loading, Splash, MainMenu, CarSelection, TrackSelection, Game,
Highscores, Options and Help classes are present behind the source LIFO screen
stack. The staged loader exposes the exact `Models...`, `Landscape...`,
`Textures...` and `All systems go!` status sequence. XNA created those resources
from a worker; CNA graphics resources are owner-thread-affine, so stages advance
cooperatively from the game thread while retaining their observable order and
one-second ready delay.

The desktop input route preserves the original keyboard, mouse and player-one
gamepad mappings, including text entry and the source OEM punctuation mapping.
Menus use the authentic background/button/header/help/options/color/cursor atlases,
`CarSelectionPlate.xnb`, selection shadows, `LineRendering.xnb` highscore
separators, menu post-processing and the three `pokal` trophy XNBs. Text and cursor
flush after the post-process as in the original source.

## Audio and persistence closure

`RacingGameManager.xgs`, `Wave Bank.xwb` and `Sound Bank.xsb` load directly through
CNA's XACT-shaped API: four categories, eight variables, 28 wave entries and 27
cues. Screen clicks, highlights, countdown, checkpoints, crashes, victory/loss,
menu/game music and five continuous/transitioning gear cues retain the original
selection, throttle-dependent volume, pitch and cooldown logic. The authentic
banks remain the only audio source.

`GameSettings` serializes through sharp-runtime's compile-time `XmlSerializer`
customization point and CNA storage. The XML keeps the original root, schema
namespaces and property element names. Defaults, dirty-state transitions,
minimum-graphics behavior and the original consume-on-save-failure rule are
preserved. Highscore parsing/ranking and replay replacement use the source formats;
asynchronous saves are joined before replacement and shutdown to keep ownership
safe.

## Fidelity details

Options initialize resolution/fullscreen from the live graphics device and apply
only changed display state. `HighDetail` controls the original road technique,
guard-holder/column population, random object density, shadow-receiver distance
and shadow-map size. Menu previews use the generated replay pose and player camera,
and the splash draws the source world/shadow composition without menu post effects.
On an empty screen stack the original total-crash/stop-music exit behavior runs
once.

The end-to-end control probe enters through Loading and all selection screens,
drives real Advanced-track physics past a completed lap to a natural Game Over,
accepts the result and unwinds Game, TrackSelection and CarSelection back to
MainMenu. It verifies the real highscore submission, outcome cue and stopped gear
loop. A separate real-GPU harness renders all three rank trophies and proves their
outputs are visible and mutually distinct; no forced victory state exists in the
product.

## Qualification

All builds used at most eight parallel jobs.

- settings/XML/storage probe: PASS in Debug and ASan/UBSan;
- loading/splash/menu/car/track/race/XACT screen-flow probe: PASS in both builds;
- highscores/options/help/LineRendering/cursor persistence probe: PASS in both;
- natural race-outcome and return-to-menu probe: PASS in both;
- cumulative real-OPENGL33 harness: 148/148 in Debug and ASan/UBSan;
- 420-frame complete scene: PASS in Debug and ASan/UBSan with 53 authentic model
  XNBs, 1,252 seeded objects, full post/HUD/shadow/replay behavior;
- Debug/ASan captures are pairwise bit-identical: screen flow
  `025c275c700ccffbf33ab255ead2ec04cfd6231970db69bb6e6ee6661c5187ba`,
  menu tour `a802bde6b16a64935437f0f7b052b22f66c84183316eacb046b422d7ff7d683d`,
  race return `b514e6ae02e4ab83bdebd31dbc65147e7324e80b8b635fc7e2bda330fea572d7`,
  420-frame scene
  `84bbaab739eb5fa8dbe9d144dac19957e8370785b1706db695e53b4b11ce4768`,
  HUD `c1859ce2fdeb5fc41f637b9b659197be288f563ce09ae96e2ecb8f318ed58364`;
- `git diff --check`: PASS.

The cumulative qualification script now builds and runs the four Milestone 8
probes and stores their logs/captures under `evidence/cna-opengl33/milestone8/`.

## Upstream impact

No CNA, sharp-runtime, meta-gl or EasyGL source change was required during this
milestone. Racing consumes the already-landed sharp-runtime XML serialization and
CNA XACT/storage/effect/model functionality at the frozen revisions above.

## Next milestone

Milestone 9 owns feature-complete Linux `OPENGL33` fidelity, performance, leak,
device-loss and release-gate evidence. Later milestones retain Windows, Android and
Web qualification; they are not implied by this desktop completion.
