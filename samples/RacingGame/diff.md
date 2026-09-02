# Racing Game intentional C++ mechanics

The original XNA game remains the behavioral authority. This file records only
language mechanics that the C# source cannot need.

- `TrackData` explicitly registers its serializable members and the XNA
  `Vector3`/`Matrix` value shapes with sharp-runtime's compile-time
  `XmlSerializer` customization points. .NET discovers the same public
  properties through reflection; reflection is deliberately unavailable in
  sharp-runtime. The registration changes no element name, order, value or
  wire representation.
- `TangentVertex` is a trivially-copyable 44-byte C++ struct with a static
  `VertexDeclaration`. It does not inherit CNA's polymorphic `IVertexType`,
  because that would insert a vtable pointer into the bytes uploaded to the GPU.
  Its four fields and offsets remain exactly the original
  `Position@0, TextureCoordinate@12, Normal@20, Tangent@32` layout.
- The pure CPU mesh construction that lives inside C# `Track` is held by the
  internal C++ `TrackGeometry` value. The eventual `Track` owner consumes these
  same arrays for GPU buffers; extracting them makes all road/back/tunnel bytes
  independently testable and does not change an index, vertex, draw range or
  content route.
- The CPU portions of C# `GuardRail` and `TrackColumns` are likewise held in
  internal `GuardRailGeometry` and `TrackColumnsGeometry` values. They preserve
  the original mesh bytes and model-placement transforms while allowing the
  eventual rendering owners to acquire GPU buffers through normal CNA RAII.
- C++ `CarPhysics` receives one game-owned `CarControlState` and a
  `CarPhysicsEnvironment` reference. The original reads the same values and invokes
  the same side effects through static `Input`, `BaseGame` and `RacingGameManager`
  members. The explicit C++ dependencies preserve every input mapping, calculation
  and call site while making a frame immutable and deterministic; they do not add a
  second physics path or bypass CNA input APIs.
- C++ `ChaseCamera` overrides protected camera hooks on `CarPhysics` so camera
  requests apply to the owning object. The original reaches that same object through
  the process-static `RacingGameManager.Player`. Its original static X/Y/Z-axis
  properties are instance getters in C++ because the view matrix is explicitly
  game-owned; the extracted columns and all camera calculations are unchanged.
- C++ `Player` emits rank queries, centered result text and outcome audio through a
  `PlayerEnvironment` derived from the same game-owned physics environment. The
  original uses static `Highscores`, `TextureFont`, `Sound` and `BaseGame` owners;
  message contents, coordinates, colors, lap formatting and branch order remain the
  same.
