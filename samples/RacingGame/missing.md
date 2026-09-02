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

The raw-data/CPU-geometry slice is implemented and independently reproducible:

- `TrackData`, all three `.Track` files, width/road helpers and neutral objects;
- `TrackLine` spline, looping, landscape correction, orientation, road width, UV and
  helper-range generation;
- all ten `TrackCombiModels` XML files through sharp-runtime's compile-time
  `XmlSerializer<List<CombiObject>>` route;
- the complete 257x257 landscape vertex and 393,216-index generation;
- the complete road-surface, road-back and tunnel vertex/index generation for all
  three tracks;
- the original runtime `TangentVertex` 44-byte layout, kept separate from XNB model
  vertex declarations.

The FNA oracle and CNA probe agree bit-for-bit on all aggregate hashes for 66,049
landscape vertices, every landscape index, all generated fields of all three tracks,
all road/back/tunnel vertices and indices, all helper ranges, neutral-object matrices
and all ten combinations. A subtle original C# behavior is intentionally preserved:
`preUpVectors[0].Normalize()` operates on the temporary value returned by
`List<Vector3>`'s indexer, so it does not normalize the stored vector.

Current reusable builds and evidence live under:

```text
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/cna-native-opengl33-m4/
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/fna-track-oracle-current/
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/diagnostics/
```

Milestone 4 still requires the guard-rail and column geometry, the representative
authentic model placements, and the first structurally/visually compared static
scene. Later milestones remain open exactly as listed in `plan_racing.md`.
