# Racing Milestone 5 — drivable car and desktop controls

## Result

Milestone 5 completed on 2026-09-03. `RacingGame_cna_samples` is the first runnable
product target: exact car/player/camera logic drives a real generated track and
renders the authentic XNA 4 car, road, landscape and sky through OPENGL33. No GLB,
glTF, loose asset conversion, alternate shader or sample-local runtime workaround is
present.

## Frozen revisions

- CNA: `aa27e6a6a61fb296403950fa66cec96b5d058562`
- cna-samples starting revision: `c25c045ff99e8ecbea54b5e030e60a34ad1c41b9`
- sharp-runtime: `6baf0925e0bf7805a363836deed90543d26f6d28`
- renderer/profile: CNA `OPENGL33`, HiDef, Mesa OpenGL 4.5 core

## Product integration

The concrete `RacingGameManager` owns the `GraphicsDeviceManager`, content, static
scene, player, car renderer, desktop control source, camera matrix, checkpoint and
replay state, default highscore table and pending UI/audio events. Physics calls the
actual `Track` for segment localization and coordinate frames. The desktop provider
preserves the original keyboard/mouse/gamepad mapping, mouse half-delta smoothing,
wheel delta and active in-race cursor recentering.

`CarRenderer` uses `Models/Car.xnb` and `Textures/RacerCar.xnb` from the unchanged
XNA Game Studio build. It preserves processor-selected techniques, embedded normal
and cube textures, original ambient/diffuse parameters, absolute bones, wheel signs
and solid-then-reflection/glass pass order. All 12 mesh parts are submitted.

Track now computes the original 500m checkpoint segment list. FNA and CNA agree:

```text
TrackBeginner: 197,398,585,783,975,1172
TrackAdvanced: 197,395,588,787,984,1178,1376,1621,1812,2007
TrackExpert:   198,395,591,788,984,1196,1390,1584,1781,1974,2169,
               2366,2627,2821,3017,3214,3411,3607,3802,3995,4192
```

## Regression evidence

The scene probe supplies an accelerating logical control source while exercising the
same game, player, renderer and public CNA APIs as the product. At 60Hz it covers the
original five-second countdown followed by two seconds of acceleration:

```text
updates=420 draws=420 distance=14.150156 carParts=12
=== Racing Drivable Scene: PASS ===
```

Debug and ASan/UBSan captures are byte-identical with SHA-256
`c845541c86c4b0124b94b9cef1203ccb1da19085cdc58db714e6ac133dfff216`.
Both visibly contain the chase-camera car, wheels, glass/reflections, complete road,
rails, landscape and sky.

The cumulative gate also passed:

- physics/player/camera FNA comparison: 739 records, non-camera exact, camera at
  most 16 ULP;
- Track FNA comparison: 70 exact records including three checkpoint records;
- OPENGL33 harness: 107/107 Debug and 107/107 ASan/UBSan;
- FNA/CNA static scene: RMSE `0.002230`, 99.0503% of channels within 2;
- LSan: only the pre-classified 100,956 bytes / 449 allocations rooted entirely in
  external `libGLX_mesa` frames;
- `git diff --check`: pass.

## Remaining scope

Milestone 6 owns persistent settings/highscores, complete replay load/save and ghost
interpolation, full lap/race completion and end-to-end deterministic race traces.
Milestone 7 owns all decorative objects and final render composition; later
milestones own UI, audio and platform completion.
