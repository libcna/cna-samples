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
- The car-only transform loop from C# `Model.RenderCar` is held by
  `CarModelHierarchy`. It stores the same one-time absolute-bone snapshot and emits
  mesh/world pose pairs for the eventual renderer; wheel recognition, ordering,
  alternating rotation signs and matrix multiplication order are unchanged.
- C++ `Input` owns the original static helper's prior mouse state and smoothing
  accumulator as an ordinary game-owned object. Its update order, half-delta filter,
  wheel delta and in-race cursor recentering are unchanged; `ControlSource` permits
  replacement only for deterministic qualification.
- C++ `RacingGameManager` represents the original process-static environment as
  explicit owned state and dependency interfaces. The product executable uses the
  real desktop provider; the scene probe injects a logical snapshot after its CNA
  device mapping has already been qualified against unchanged C#.
- The rendering half of C# `Model.RenderCar` is provisionally held by internal
  `CarRenderer` until the complete original `Graphics.Model` is translated in
  Milestone 7. It consumes the same XNB model and Effect objects, with unchanged
  parameters and two-pass ordering; it is not an alternate rendering path.
- C++ `Replay` receives the current top lap time and content directory from the
  game-owned environment instead of reading the original process-static
  `Highscores`/`Directories` classes. Its generation, interpolation and binary
  representation are unchanged. The original static `ManualResetEvent` around
  storage is represented by a process-wide mutex, while `RacingGameManager`
  retains the original background-save behavior through an owned future.
- The original constructor's never-updated `replayFileFound` local is preserved
  deliberately: even a successfully read replay is followed by the title/default
  paths. This is an observable old source quirk, not a new CNA behavior.
- Checkpoint comparison audio is emitted through a typed `CheckpointSoundType`
  environment event. The original calls process-static `Sound.Play`; the C++ owner
  records the same better/worse selection for the later XACT consumer without
  coupling deterministic car physics to the audio backend.
