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
- The original loading screen schedules graphics-resource construction on a worker
  thread. CNA graphics devices and their GL contexts are owner-thread-affine, so
  the C++ screen advances the same `Models...`, `Landscape...`, `Textures...` and
  `All systems go!` stages cooperatively on the game thread. Resource order and the
  visible one-second ready delay remain unchanged; no second content path exists.
- The rendering half of C# `Model.RenderCar` is held by internal `CarRenderer`
  while common visibility/material policy remains in `Graphics.Model`. It consumes
  the same XNB model and Effect objects, with unchanged parameters and two-pass
  ordering; it is not an alternate rendering path.
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
- The replay ghost uses the same `CarRenderer` boundary as the player
  car. It binds the authentic XNA 4 `LightingShader` `ShadowCar` technique and the
  same model hierarchy; this preserves the original effect, part ordering, replay
  pre-orientation and wheel animation until the complete `Graphics.Model` owner is
  translated in Milestone 7.
- `ScreenshotCapturer` enumerates and creates `Screenshots/...` below the same
  CNA `StorageContainer`. The original enumerates the relative name through
  `System.IO.File` but writes it through `StorageContainer`; using one logical
  storage root is the lossless cross-platform representation of that relative
  path and prevents a desktop-current-directory/storage mismatch.
- `Helpers.Log` uses sharp-runtime's `System.IO.IsolatedStorage` implementation.
  It explicitly flushes each entry because the current C++ `StreamWriter` surface
  has no `AutoFlush` property; the resulting append, timestamp and 2 MiB rotation
  behavior is unchanged.
- `LineManager3D.Render` receives world/view/projection matrices explicitly instead
  of reading `BaseGame` process statics. Both `AddLine` overloads, the 4096-line
  bound, retained-array rebuild rule, authentic `LineRendering3D` technique and
  per-frame reset remain unchanged.
- For the owner-approved Android and Web qualification plans, `MobileInput` composes the
  unchanged desktop `Input` provider with CNA
  `TouchPanel` and optional CNA `Accelerometer` state. It does not pretend that a
  touch screen is a connected gamepad. Menu contacts populate the original
  pointer/click fields; race contacts populate additional logical values whose
  zero defaults leave every desktop branch and trace unchanged.
- `CarControlState` exposes mobile steering, throttle, brake, handbrake and camera
  values because the C# sample predates touch devices. `CarPhysics` applies them in
  the same steering, acceleration, braking and camera formulas used by the original
  device branches; there is no mobile physics implementation or tuned vehicle
  model.
- `MobileControlMapper` owns safe-area layout, stable per-finger roles, handedness,
  sensitivity and optional tilt filtering outside simulation. Android/Web selection
  of that provider, Android landscape orientation and Android CNA `DisplayInfo`
  safe-area polling are the only platform-gated game integration points; the mapper
  and logical action state are ordinary portable C++. The extension is selected by
  default for Android and Emscripten products and does not change desktop input or
  gameplay. Web uses the full backbuffer safe area and leaves tilt disabled.
  `DisplayInfo` reports window-client coordinates, so the portable layout helper maps
  both axes independently into the logical backbuffer before deriving hit rectangles.
- The Android provider enables the existing optional tilt preference by default.
  Its first valid sample establishes neutral on every race entry or lifecycle
  resume; leaving the race stops the sensor and clears that calibration. Android
  reverses the platform landscape axis, selects 1.5x response sensitivity and uses
  the shared filter's faster response. A steering contact overrides tilt for its
  lifetime, preserving touch as an immediate fallback without changing the shared
  physics formulas.
- SDL also exposes a phone finger as a mouse event by default. While a real
  `TouchPanel` contact owns the current or immediately preceding race frame,
  `MobileInput` suppresses only the duplicate mouse-driving snapshot and clears
  its smoothing history. Menu clicks, genuine mouse use outside those frames and
  all desktop behavior remain available; the shared vehicle physics contains no
  Android special case.
- Touches already active when the race begins are ignored until their release, and
  the first race frame suppresses mouse driving even if SDL delivered the GO-touch
  release between game screens. A new in-race contact is accepted normally.
- `UIRenderer` creates one game-owned one-pixel `Texture2D` for translucent vector
  control shapes and labels them with the already loaded authentic bitmap font.
  This avoids new replacement art and keeps the overlay outside the authentic
  post-process scene, while its sprite count and safe-area geometry remain directly
  testable.
- The owner-requested Web qualification retains input transitions across fixed
  `Update` catch-up until the next `Draw`. The original calls `Input.Update` from
  each game update but consumes `JustPressed` properties from screen `Render`
  methods; several browser updates before one draw can therefore overwrite an
  otherwise valid edge. `ControlFrame` accumulates only those one-shot values and
  typed text. Held controls, axes and pointer positions still come from the latest
  update, the latch clears immediately after rendering, and no Web-specific
  gameplay branch or CNA input bypass is introduced.
- Web deployment stages unchanged XNB/XGS/XSB/XWB files across the original
  Models, Landscape and Textures loading phases. A platform-boundary
  `ContentDelivery` provider only reports whether the requested virtual-filesystem
  group is ready; the manager then runs its existing load code. The package audit
  excludes XACT wave intermediates already compiled into the authentic XWB and
  the two Xbox-only UI textures not selected by this platform. This is an
  owner-approved delivery difference, not a content-quality or gameplay fork.
- Browser autoplay policy requires a trusted user gesture before audio playback.
  The Web shell therefore completes its initial runtime and storage setup, shows
  `Start race`, and invokes the unchanged `main` entry point from that click.
  Native targets still enter `main` immediately, and no screen, input, XACT cue
  or simulation branch is browser-specific.
- The owner-requested Web presentation starts windowed even when the original
  `GameSettings.Fullscreen` default or a persisted desktop preference is true.
  Browser fullscreen is instead entered from a sample-shell `Fullscreen` button,
  which supplies the fresh user gesture required by the browser. This display-only
  Emscripten branch is limited to initial `GraphicsDeviceManager` configuration;
  Linux, Android and the other native targets continue to apply the original
  setting unchanged, and there is no Web rendering or gameplay substitute.
- On Web, `PersistentStorage` mounts no filesystem itself; it verifies the shell's
  synchronized IDBFS mount and sets the standard `XDG_DATA_HOME` root before
  `RacingGameManager` loads settings. All existing CNA `StorageContainer` paths,
  original XML serialization and asynchronous replay/highscore owners remain
  unchanged. The provider is an empty platform boundary on native targets.
- The owner-requested `RACING_GAME_TURBO` CMake option is an explicit alternative
  gameplay configuration and defaults to `OFF`. Defining it doubles the original
  `DefaultMaxSpeed` and `DefaultMaxAccelerationPerSec`; the corresponding
  `MaxPossibleSpeed` normalization ceiling and positive `MaxAcceleration` clamp
  are doubled as well so they cannot truncate the requested twofold behavior.
  Original per-car performance multipliers, braking/reverse acceleration, mass,
  steering, rendering, input and assets are unchanged. Builds without the option
  retain the source values and remain the fidelity reference.
