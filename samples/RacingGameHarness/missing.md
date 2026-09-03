# Racing Game diagnostic harness

This is the permanent non-game diagnostic target required by `plan_racing.md`. It is
not the Racing port. The product lives separately in `samples/RacingGame`; the
cumulative qualification script builds and runs both targets.

The harness exercises a real `Game` loop and SDL/OpenGL window on an isolated Xvfb display. Its
qualification script proves:

- `Initialize` / `LoadContent` / `Update` / `Draw` / `UnloadContent` and graphics-device events;
- deterministic clear, backbuffer readback, multiple successful presents and a retained capture;
- resize, fullscreen round-trip, viewport and presentation state;
- real X11 events reaching CNA's public `Keyboard` and `Mouse` snapshots;
- `RenderTarget2D` bind/clear/unbind/readback;
- independent upload/readback of all six `TextureCube` faces;
- byte-exact upload/readback and GPU consumption of the original Racing
  `Position + TextureCoordinate + Normal + Tangent` 44-byte vertex layout;
- Debug and ASan/UBSan execution on the exact `OPENGL33` renderer, with backend/profile evidence.

With `--content-root`, it also loads the authentic XNA Game Studio 4.0 outputs for
`Models/Car`, `Models/Windmill`, `Models/AlphaDeadTree` and `Models/Cube` through
public `ContentManager::Load<Model>`. It validates all 17 parts, keeps the processed
56-byte tangent layout separate from the runtime 44-byte `TangentVertex` layout,
selects the original processor-authored techniques and asserts meaningful pixels for
all four models.

With `--effect-evidence`, the harness additionally loads the authentic
`Shaders/NormalMapping` and `Shaders/PostScreenShadowBlur` XNBs. It proves all 14
normal-mapping techniques, `Specular20/P0`, authored defaults, texture parameters,
independent Effect cloning, exact runtime tangent-layout consumption, and the
original horizontal/vertical fullscreen blur passes with render-target readback.
The shared XNA/FNA oracle and comparator are under `oracle/xna4/` and `scripts/`.
The complete cumulative Debug and ASan/UBSan qualification is **107/107 PASS**.
It also runs the CPU Track and physics/camera oracles. The latter compiles unchanged
original `BasePlayer.cs`, `CarPhysics.cs`, `ChaseCamera.cs`, `Player.cs`,
`SpringPhysicsObject.cs` and `Vector3Helper.cs`: 667 non-camera records are bit
exact (including the complete Player outcome probe), while 72 frames x 44 camera
values stay within the measured 16-ULP native/JIT float boundary. The cumulative
oracle is 739 records.

Five of the 107 real-GL assertions consume the authentic `Models/Car.xnb` through
`CarModelHierarchy` and prove all six mesh poses, the original four-wheel sequence,
zero/animated transforms and unchanged glass/body transforms.

Milestone 5 also adds `RacingGameDrivableSceneProbe`, which runs the real product
scene for 420 deterministic updates in Debug and ASan/UBSan. It verifies repeated
present, post-countdown acceleration, all 12 authentic car part submissions and a
complete 1280x720 GPU capture. Evidence is under
`evidence/cna-opengl33/milestone5/`.

Artifact root:
`/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/`. Milestone 2 evidence is
under `evidence/cna-opengl33/milestone2/`; the exact authentic build is under
`evidence/xna4-authentic-build/`. Milestone 3 CNA evidence is under
`evidence/cna-opengl33/milestone3/`, with XNA and FNA oracles under
`evidence/xna4-authentic-effect-oracle/` and
`evidence/fna-authentic-effect-oracle/`.

No host desktop window is opened: `scripts/qualify-milestone1.sh` always runs the executable under
its own Xvfb server and injects input only into that virtual display.
