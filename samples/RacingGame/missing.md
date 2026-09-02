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

## Milestone 5 progress

Milestone 5 is in progress. The original `Vector3Helper` and
`SpringPhysicsObject` calculations are translated without altered formulas. A native
CNA probe is compared against a FNA executable which compiles those two unchanged
original C# source files directly. The vector and spring-state hashes agree
bit-for-bit in both Debug and ASan/UBSan builds. This oracle is now part of the
cumulative qualification entry point; the prior 102/102 static-world harness and
FNA image comparisons remain green.

The car/player state, logical input snapshot, wheel hierarchy and chase camera are
still open. Deterministic checkpoint gameplay is scheduled for Milestone 6;
the complete auto-generated decorative population (palms, lanterns and signs) and
all combination models remain part of the complete-world Milestone 7. Later
milestones remain open exactly as listed in `plan_racing.md`.
