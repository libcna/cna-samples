# Racing Milestone 4 — static track scene

## Result

Milestone 4 is complete on 2026-09-03. CNA now renders the first representative
Racing scene from the original XNA 4 raw track/landscape inputs and authentic XNA
Game Studio 4.0 XNB products. No GLB, glTF, modern material, loose texture, shader
rewrite or sample-local content decoder is present.

The cumulative public harness is 102/102 PASS in Debug and ASan/UBSan. The
model-free static capture is structurally the same as the authoritative FNA/OpenGL
capture: normalized RMSE `0.002230`, `99.0503%` of RGB channels within two 8-bit
values and maximum channel delta 19.

## Frozen revisions and environment

- CNA: `af0f4f1964ed12aef1b63d5083e7293c49c3a393`
- cna-samples Milestone 4 starting revision: `96e2bc3`
- cna-samples pre-closure revision: `b98d121c1379473478c5fd2e0d894acf64d8a3a2`
- sharp-runtime: `6baf0925e0bf7805a363836deed90543d26f6d28`
- FNA managed source/assembly: `b35512475ed7980169574d2c40927381c1764d5a`
- FNA3D: `32401479a3ab5bd6b2e7f786e87bf4166aa03b0f`
- modern Racing behavior oracle: `d8092633e4e43e014ff168d8e913a9373538b851`
- CNA renderer: `OPENGL33`, HiDef, Mesa llvmpipe OpenGL 4.5 core
- FNA renderer: FNA3D OpenGL, Mesa llvmpipe OpenGL 4.5 compatibility,
  MojoShader `glsl120`

## Original data and CPU structure

The port reads the unchanged `TrackBeginner.Track`, `TrackAdvanced.Track`,
`TrackExpert.Track`, ten `.CombiModel` files and `LandscapeHeights.data`. Its
44-byte runtime `TangentVertex` remains deliberately separate from the authentic
processor-authored 56-byte XNB model vertex layout.

The authoritative C# oracle and C++ probe agree bit-for-bit across 64 record groups:

- 66,049 landscape vertices and 393,216 indices;
- every generated field for all three closed track splines;
- road, road-back and tunnel vertices/indices;
- both guard-rail meshes and all holder transforms;
- every column mesh and `RoadColumnSegment` position;
- all road-helper ranges, neutral-object matrices and ten combinations.

For `TrackBeginner`, the visible scene specifically retains 6,660 road vertices,
11,339 vertices per guard rail, 224 left holders, 231 right holders and 101 column
positions. Re-running the original Track oracle after adding the renderable scene
still reports:

```text
RACING_TRACK_ORACLE_COMPARISON=PASS records=64 COLUMN=3 COMBI=10
FIELDHASH=3 GUARD=6 HELPER=32 LANDSCAPE=1 PHASEHASH=3 ROAD=3 TRACK=3
```

## GPU scene

`StaticTrackScene` owns the original CPU objects and RAII vertex/index buffers for
terrain, road, road back, tunnels, both rails and columns. It loads unchanged XNB
identifiers for:

- `Shaders/NormalMapping`, `Shaders/LandscapeNormalMapping` and
  `Shaders/PreScreenSkyCubeMapping`;
- the original landscape/detail, road, road-back, tunnel, guard-rail and cement
  textures/normal maps;
- `Textures/SkyCubeMap` and the stock-processor `Models/Cube`.

Technique selection, material colors, specular power, sky color and the normalized
original gameplay light `(8500,-7250,15000)` follow the original sources. The
standard final capture additionally submits authentic custom-processor XNB models
`GuardRailHolder`, `RoadColumnSegment` and `Banner6` using qualified placements.

## Independent FNA image oracle

The FNA project reuses the linked original `TangentVertex`, `TrackData`, `TrackLine`,
`TrackVertex` and `TrackCombiModels` sources plus the already-qualified oracle
landscape/geometry builders. It loads the authentic textures, cube map and Cube XNB.
As in Milestone 3, only FNA's renderer-side Effect bytes come from the pinned modern
repository because unpatched FNA MojoShader cannot parse an unused legacy shader in
the authentic full Effect graph. Those `.efb` files are behavior-oracle inputs only;
they are not copied into or loaded by CNA.

The comparison intentionally captures CNA immediately before the three custom model
submissions. This gives FNA and CNA identical scene contents while the final CNA
capture separately proves the representative authentic models. The committed
comparator enforces RMSE at most `0.01` and at least 97% of channels within two
values. Frozen output is substantially tighter:

```text
RACING_STATIC_SCENE_COMPARISON=PASS width=320 height=180
rmse=0.002230 channelsWithin2=0.990503 maxChannelDelta=19
```

PPM SHA-256 values:

- CNA Debug/ASan/LSan base scene:
  `70d110a08e2a080d1e7a30c5a883677a22412b53b01230825dcebd7b110cbb80`
- FNA base scene:
  `3af1c2f8369bbbdc3d1630a293deb548252c4b1be95ffc8a4b6db9b15d731916`

## Qualification and remaining scope

`samples/RacingGameHarness/scripts/qualify-milestone1.sh` is the cumulative entry
point and caps compilation at eight jobs. It completed:

- Debug OPENGL33 build/run with real synthetic X11 keyboard/mouse input: 102/102;
- ASan/UBSan build/run with leak detection disabled: 102/102, no finding;
- LSan classification: 102/102, only the already-known 100,956 bytes rooted wholly
  in external Mesa `libGLX_mesa` frames;
- FNA/OpenGL static-scene build/run: `RESULT PASS`;
- FNA/CNA Debug and ASan image comparisons: PASS;
- `git diff --check`: PASS before commit.

Evidence roots:

- `evidence/cna-opengl33/milestone4/`
- `evidence/fna-static-scene-oracle/`
- `diagnostics/` for the exact CPU oracle reports.

No CNA or sharp-runtime change was required for this final GPU slice. The earlier
general fixes used by Milestone 4 remain independently committed upstream.
Milestone 5 may now begin with car/player state, wheel hierarchy, chase camera and a
logical desktop input snapshot. Full decorative auto-population and combination
model rendering remain scheduled for Milestone 7, while checkpoint gameplay belongs
to Milestone 6.
