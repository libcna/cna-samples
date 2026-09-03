# Racing Game — active fidelity ledger

`SAMPLE-152` is active and governed by [`../../plan_racing.md`](../../plan_racing.md).
It is not complete. The canonical implementation and content source is the unchanged
XNA 4 sample retained under:

```text
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/xna4-original/
```

The authentic XNA Game Studio build is retained under
`evidence/xna4-authentic-build/Debug`; models and effects are loaded from those
unchanged XNB products. No GLB, glTF, modern material or loose-content substitute is
part of this port.

## Milestone 4 status

Milestone 4 is complete and independently reproducible:

- `TrackData`, all three `.Track` files, width/road helpers and neutral objects;
- `TrackLine` spline, looping, landscape correction, orientation, road width, UV and
  helper-range generation;
- all ten `TrackCombiModels` XML files through sharp-runtime's compile-time
  `XmlSerializer<List<CombiObject>>` route;
- the complete 257x257 landscape vertex and 393,216-index generation;
- the complete road-surface, road-back and tunnel vertex/index generation for all
  three tracks;
- both guard-rail meshes and their holder transforms, plus all track-column meshes
  and `RoadColumnSegment` positions;
- RAII GPU buffers for the complete qualified geometry and authentic XNA XNB loads
  for the sky, terrain, road, tunnel, rail and column materials;
- a rendered `TrackBeginner` scene with sky, terrain, road/back/tunnel, both rails,
  columns and authentic `GuardRailHolder`, `RoadColumnSegment` and `Banner6` model
  submissions;
- the original runtime `TangentVertex` 44-byte layout, kept separate from XNB model
  vertex declarations.

The FNA oracle and CNA probe agree bit-for-bit on all aggregate hashes for 66,049
landscape vertices, every landscape index, all generated fields of all three tracks,
all road/back/tunnel, guard-rail and column vertices and indices, every guard-rail
holder and column-segment placement, all helper ranges, neutral-object matrices and
all ten combinations. A subtle original C# behavior is intentionally preserved:
`preUpVectors[0].Normalize()` operates on the temporary value returned by
`List<Vector3>`'s indexer, so it does not normalize the stored vector.

The FNA/OpenGL and CNA/OPENGL33 base-scene captures use the same original C#
geometry, authentic textures and sky model, camera, original light direction and
material parameters. Their normalized RMSE is `0.002230`; `99.0503%` of channels
are within two 8-bit values. The final CNA capture additionally proves the three
representative custom-processor models. The cumulative harness is 102/102 PASS in
both Debug and ASan/UBSan.

Current reusable builds and evidence live under:

```text
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/cna-native-opengl33/
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/evidence/fna-static-scene-oracle/
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/evidence/cna-opengl33/milestone4/
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/diagnostics/
```

## Milestone 5 status

Milestone 5 is complete. `RacingGame_cna_samples` runs a concrete environment which
connects the exact `Player`/`CarPhysics`/`ChaseCamera` port to generated `Track` and
the original desktop keyboard, smoothed mouse and player-one gamepad mapping. Track
also retains the original 500m checkpoint segment generation; the FNA/CNA Track
oracle is 70/70 exact over all three tracks.

`CarRenderer` loads the authentic six-mesh `Models/Car.xnb`, preserves the original
processor-selected effects/material values, animates four wheel meshes and draws
solid parts before reflection/glass. A 420-frame OPENGL33 probe in Debug and
ASan/UBSan completes every update/draw/present, submits all 12 model parts, advances
the car 14.150156m after the authentic countdown and writes identical final captures
(`c845541c86c4b0124b94b9cef1203ccb1da19085cdc58db714e6ac133dfff216`). The
then-cumulative harness was 113/113 and the physics/player oracle is now 753 records.

Milestone 6 is complete. The deterministic race trace drives the unchanged original
C# and translated C++ logic through three laps and proves checkpoint ordering,
faster/slower feedback, two best-replay replacements, lap times, victory, outcome
audio events and all result text. Its 753 FNA/CNA records are exact outside the
previously measured 16-ULP camera boundary. The port now also emits the original
checkpoint comparison sound event that the earlier C++ translation had omitted.

The replay data model itself is now complete and connected. An oracle compiling
unchanged original `Replay.cs` agrees with CNA on 79/79 cumulative Track records,
including all three default ghosts, playback boundaries/interpolation, append,
deep clone and exact 144-byte binary output. The preserved source quirk whereby
`replayFileFound` is never set is explicit in the port. The 420-frame product probe
also records the current lap and interpolates the generated best replay. See
[`../../racing_milestone6.md`](../../racing_milestone6.md) for the frozen outcome.

Milestone 7 is complete. The generated best replay is drawn with the authentic
`Shaders/LightingShader.xnb` `ShadowCar` technique, the original replay
pre-orientation and current player wheel angle. The complete 53-model original
landscape catalog, all authored objects, all ten combination files and the full
source-ordered procedural population now render through the authentic processed
model/effect graph. The deterministic seed-152 Beginner probe resolves 1,252 final
objects, submits 295 visible parts and produces bit-identical Debug and ASan/UBSan
captures (`2d36d296a81d43437a33565b8514d52903fb730f293d7aa31be6fb454c77af77`)
over 420 update/draw cycles.

The source shadow sequence is also connected end to end. Authentic
`Shaders/ShadowMap.xnb` renders the original Rgba64 caster and receiver targets;
authentic `Shaders/PostScreenShadowBlur.xnb` executes its two authored passes and
overlays the result. Track road/tunnels/rails, nearby objects and the car produce
22 caster and 22 receiver submissions. The 420-frame Debug and ASan/UBSan product
runs are clean and bit-identical (`a00eb9c2c9dbaa79a494aa71bcea9093f508a195c93aa2b330514f7f850466dd`).
This required a general CNA/meta-gl fix for truthful desktop RGBA16 UNORM render
targets; no `Color` substitution or sample fallback remains. The then-cumulative
OPENGL33 harness was 113/113. City-plane
ground fill is now restored from the first source-ordered hotel/building position,
the original two-triangle geometry/tiling and authentic
`CityGround.xnb`/`CityGroundNormal.xnb`. It submits once per frame and the Debug and
ASan/UBSan captures are bit-identical
(`07411bf5df7c4ea55dd17f71c9e8ea49c2f72d49dd717ddb47c900cdd01bb917`).

The product environment also routes major-brake events into the source
`Landscape.AddBrakeTrack` behavior: identical distance/overlap/cap rules, six
authored tangent vertices per accepted mark, alpha blending and authentic
`Textures/track.xnb` through `LightingShader.xnb` `Diffuse20`. The deterministic
probe brakes after accelerating, retains 18 vertices, submits six triangles and
finishes bit-identically in Debug and ASan/UBSan
(`4cd857708ea238880a84aea1fe6129b0f080352a31b14cf312fce571915e4615`).
The complete source `LensFlare.cs` path now loads the seven authentic texture XNBs,
preserves all 17 authored records and the exact projection, border fade, smoothed
intensity and additive SpriteBatch composition, and observes the original ten-frame
tunnel gate. A dedicated real-OPENGL33 camera submits all 17 layers and changes the
backbuffer; origin/rotation, tunnel suppression and idempotent disposal are covered
in Debug and ASan/UBSan. The 420-frame product probes are clean and bit-identical at
`936dcb0a510eeff0053264c10dbaccf962a39960b07bf31e1e43a049531a3d5c`; the
normal Beginner start camera correctly finishes with the directional sun outside
the view. The original full/quarter `RenderToTexture` chain and authentic
`PostScreenGlow.xnb` five-pass composition now surround the complete game scene,
including the source alpha-write blend, speed-dependent radial blur and
`ScreenBorderFadeout.xnb`. The authentic four-pass `PostScreenMenu.xnb` path is
also integration-tested ahead of the screen stack. The pre-HUD Debug and ASan/UBSan
harnesses passed 133/133; both 420-frame product runs passed with bit-identical SHA-256
`7c725e0859e94dd906d84eb3b34948042890534fb7834aaf4b3b515c59445c89`.
The cumulative Debug and ASan/UBSan qualification passes, including both CPU
oracles and the bounded FNA static-geometry comparison.
An FNA run with the persisted post-screen setting explicitly enabled is retained in
`evidence/fna-postprocess-oracle`; the earlier sharp reference frames had the same
setting disabled.

The original desktop in-race HUD is now connected after the post-screen and lens
flare stages. It loads only authentic `Textures/Ingame.xnb` and
`Textures/GameFont.xnb`, retains the source rectangles and coordinate spaces for
lap/current/best/top-five panels, speed, gear and tachometer, and renders the
original rising checkpoint/lap overlays. The focused real-OPENGL33 probe checks the
1024x512 `SurfaceFormat.Color` atlas, its `(0,0,0,154)` translucent panel texel,
the resulting `(79,79,79,255)` blended pixel, exact sprite/text/glyph submissions
and overlay expiry. Debug, ASan/UBSan and classified-LSan HUD captures are
byte-identical (`c1859ce2fdeb5fc41f637b9b659197be288f563ce09ae96e2ecb8f318ed58364`).
The cumulative harness was 146/146 in both builds; both 420-frame product probes
pass with 14 atlas sprites and 67 glyphs on the final frame and have identical
captures (`a38811ade71f9460abd32d1129877b98cc3c94585ecc86ff763d5ddceb2bfa1c`).
No CNA or sharp-runtime fix was required for this HUD step; the focused pixel test
caught and removed a C++ port ordering error between the original additive and
alpha SpriteBatch begins. Milestone 7 is complete.

## Milestone 8 status

Milestone 8 is complete. The port now preserves the original LIFO loading, splash,
main menu, car selection, track selection, race, highscores, options and help
screens. The loading states retain the source-visible `Models...`, `Landscape...`,
`Textures...` and `All systems go!` progression; CNA's thread-affine GPU resources
are created cooperatively on the owner thread instead of the original worker.
Menus render through the authentic atlases, post-process, car-selection plate and
shadows, `LineRendering.xnb`, mouse cursor and three trophy XNBs.

The supplied `RacingGameManager.xgs`, `Wave Bank.xwb` and `Sound Bank.xsb` load
through CNA's XACT API. All 27 named cues remain available, including screen/menu,
checkpoint, crash, victory/loss, music and the original five-gear transition and
pitch/volume behavior. Missing audio hardware follows the original silent-audio
path rather than substituting assets.

`GameSettings` uses sharp-runtime's compile-time `XmlSerializer` support and CNA
storage while preserving the original element names, schema declarations,
defaults, dirty flag and save-failure behavior. Highscores retain the source text
format and ranking rules; settings and replay saves use bounded asynchronous work
whose lifetime is joined during replacement and shutdown.

Focused Debug and ASan/UBSan probes cover settings round trips, every screen,
authentic XACT initialization/cues, low/high-detail choices and a natural Advanced
race outcome returning through Track and Car selection to MainMenu. The GPU harness
is 148/148 and renders all three authentic rank trophies as distinct visible
outputs. The complete 420-frame scene remains deterministic and sanitizer-clean.
## Platform qualification status

Milestone 9 Linux `OPENGL33` is complete; its fidelity, performance, leak,
device-loss and release-gate evidence is frozen in `racing_milestone9.md`.
Milestone 10 Windows remains incomplete, but the owner restored `/dev/vboxdrv` and
the saved `win7` VM is ready for the remaining runtime qualification recorded in
`racing_milestone10.md`.

Milestone 11 Android now has an SDL3/Gradle shell, offline `x86_64` and
`arm64-v8a` `OPENGLES3` builds, CNA touch/optional-tilt input, a safe-area overlay
and focused native qualification. It is still missing the exit evidence that only
representative physical devices can provide:

- one complete touch-only race and ergonomic multi-touch check;
- real GPU format/performance, peak memory, load-time and thermal measurements;
- pause/background/resume, surface/context recreation and process-death recovery;
- audible XACT/music/engine/UI verification;
- settings, highscores/replay storage and Bluetooth-gamepad coexistence;
- exposed and persisted touch/tilt preferences;
- a measured mobile quality preset if the authentic asset residency requires one.

Milestone 12 Web has a real-browser baseline and progressive delivery in progress.
The shared C++ port builds with Emscripten pthreads, Asyncify and `WEBGL2`. A 35 MiB
bootstrap and three existing-load-phase packages reuse 326 authentic runtime files
byte-for-byte; versioned IndexedDB caching was verified both cold and warm. A
scripted Chrome session traversed splash, main menu, car and track selection, then
drove the Advanced race with no browser, HTTP or WebGL errors. Hosted-network and
peak-residency budgets, audio unlock, persistent-storage synchronization,
context-loss recovery and the desktop/mobile browser matrix remain open. The intentional
cross-platform input-lifetime adjustment is recorded in [`diff.md`](diff.md); see
[`racing_milestone12.md`](../../racing_milestone12.md) for the browser evidence.
