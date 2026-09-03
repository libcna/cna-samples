# Racing Milestone 6 — gameplay correctness

## Result

Milestone 6 completed on 2026-09-03. The product owns the original best/current
replay lifecycle, and the unchanged XNA 4 gameplay sources and C++ translation
agree through a deterministic complete three-lap race. No GLB, converted content,
sample-local CNA workaround or rewritten gameplay algorithm is present.

## Frozen revisions

- CNA: `aa27e6a6a61fb296403950fa66cec96b5d058562`
- cna-samples starting revision: `1b127f5a91c1586c7fbe35979ee99742c519cca3`
- sharp-runtime: `6baf0925e0bf7805a363836deed90543d26f6d28`
- FNA: `b35512475ed7980169574d2c40927381c1764d5a`
- renderer/profile: CNA `OPENGL33`, HiDef, Mesa OpenGL 4.5 core

## Replay lifecycle

`Replay` preserves the original 0.2-second transform interval, interpolation,
three track-specific filenames, storage/title fallback, default track generation,
deep clone and little-endian binary representation. The original constructor's
never-updated `replayFileFound` local remains explicit: successful saved/title data
is still followed by later fallback paths.

The Track oracle compiles unchanged original `Replay.cs`. CNA and FNA agree on
79/79 exact records, including generated beginner/advanced/expert ghosts with
385/760/1135 matrices, checkpoint schedules, boundary/interpolated playback,
empty recording, append, clone and the 144-byte two-matrix saved wire fixture.

`RacingGameManager` owns best/new replays, records live matrices, compares
checkpoint times, records completed lap times, replaces a faster best replay and
saves a deep clone asynchronously. The 420-frame product probe proves a
385-matrix beginner ghost, live interpolation and current recording at the
original cadence.

## Complete race trace

The physics oracle links unchanged original `BasePlayer.cs`, `CarPhysics.cs`,
`ChaseCamera.cs` and `Player.cs`. A scripted track locator changes only the
environmental segment returned to both implementations; all checkpoint, replay,
lap, outcome and UI decisions execute in the original/translated gameplay code.

The exact outcome is:

```text
laps=2 started=3 checkpoints=0 replayMatrices=0 highscores=3
bestReplacements=2 bestLapBits=3e828f5c bestTimeBits=437f0000
bestCheckpoints=3 bestMatrices=1 fadeups=9
betterSounds=3 worseSounds=3 gameOver=1 victory=1 text=5
victorySounds=1 loseSounds=0 gearStops=1
```

The visible lap value is 2 because the original victory branch deliberately
removes the extra lap created by completion. The best lap is 0.255 seconds in this
compressed deterministic trace and the best display time is 255 milliseconds.

This trace exposed one real translation omission: the C++ checkpoint branch made
the correct comparison and overlay but did not emit the original
`CheckpointBetter`/`CheckpointWorse` sound selection. `CarPhysicsEnvironment` now
has a typed checkpoint event and the product owner records it for the later XACT
consumer. Audio playback itself remains in Milestone 8.

## Qualification

The cumulative entry point
`samples/RacingGameHarness/scripts/qualify-milestone1.sh` passed with at most eight
parallel build jobs:

- physics/player/camera comparison: 753 records in Debug and ASan/UBSan; all 681
  non-camera records exact, 72 camera frames bounded to the existing 16 ULP;
- Track/Replay comparison: 79/79 exact records in Debug and ASan/UBSan;
- drivable product scene: PASS in Debug and ASan/UBSan;
- real OPENGL33 harness: 107/107 in Debug and ASan/UBSan;
- FNA/CNA static image: RMSE `0.002230`, 99.0503% of channels within 2;
- LeakSanitizer: unchanged 100,956 bytes / 449 allocations rooted wholly in
  external `libGLX_mesa` frames;
- `git diff --check`: pass.

Evidence is retained under
`/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/evidence/`, especially
`physics-oracle/`, `fna-track-oracle/`, `cna-opengl33/milestone5/` and
`cna-opengl33/milestone4/`.

## Next milestone

Milestone 7 now owns complete visible world/rendering parity: remaining original
models/materials, combination population, visible ghost, shadows, lens flare,
post-processing, brake tracks and HUD. Milestone 8 owns screens, XACT and persistent
launch/race/return lifecycle integration.
