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
cumulative harness is 105/105 and the physics/player oracle is now 753 records.

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

Milestone 7 is current. The generated best replay is drawn with the authentic
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
targets; no `Color` substitution or sample fallback remains. The current cumulative
CPU harness is 105/105; the previous 107/107 ledger value was stale. City-plane
ground fill is now restored from the first source-ordered hotel/building position,
the original two-triangle geometry/tiling and authentic
`CityGround.xnb`/`CityGroundNormal.xnb`. It submits once per frame and the Debug and
ASan/UBSan captures are bit-identical
(`07411bf5df7c4ea55dd17f71c9e8ea49c2f72d49dd717ddb47c900cdd01bb917`). Lens
flare, post-processing, brake tracks and HUD remain open.
Screen/XACT/persistence lifecycle integration remains in Milestone 8.
