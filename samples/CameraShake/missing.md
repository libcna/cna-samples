# CameraShake — SAMPLE-030 audit record

Upstream: `CameraShake_4_0`, ported against the unchanged XNA 4.0 sources snapshotted at
`/rv/tmp/samples/SAMPLE-030-CameraShake_4_0/xna4-original`, per-file SHA-256 in
`evidence/xna4-original-sha256.txt`.

## 1. What was ported

The whole sample, 630 lines of C# across 5 files, as `.hpp`/`.cpp` pairs mirroring the
original's own layout. The previous port was a single header plus `Program.cpp` and had no
`VibrationManager` or `Camera` file at all.

| Original | Port |
|---|---|
| `Camera.cs` | `src/Camera.{hpp,cpp}` |
| `VibrationManager.cs` | `src/VibrationManager.{hpp,cpp}` |
| `CameraShakeGame.cs` | `src/CameraShakeGame.{hpp,cpp}` |
| `Program.cs` | `src/Program.cpp` |
| `Properties/AssemblyInfo.cs` | `src/Properties/AssemblyInfo.cpp` |

## 2. What the previous port claimed, and what is actually true

Eight deviations were recorded. **All eight are gone**, and four of the records had already
diagnosed themselves as porting shortcuts rather than framework limits.

| Old claim | Finding |
|---|---|
| "VibrationManager omitted … `GameComponent`/`Game.Components` not yet in CNA" | **False.** Both exist. The whole manager is ported — the four per-player settings, the linear decay, pause/resume/cancel, and `GamePad::SetVibration`. |
| "Touch input removed — phone-specific" | **False.** `TouchPanel::setEnabledGesturesProperty(Tap \| DoubleTap)` and the gesture drain loop are both ported; tap gives a short shake, double tap a long one. |
| "GamePad input not implemented (keyboard only)" | The record itself says "not a CNA framework limitation". Restored: `GamePad::GetState`, the A/X press edges and `Buttons.Back` to exit, alongside the keyboard. |
| "Instruction text uses a fixed offset instead of `Viewport.TitleSafeArea`" | The record itself says CNA implements it. Restored, including the one-pixel black drop shadow the original draws underneath. |
| "Model format converted from FBX/X to `.model.json`" via `tools/fbx_ascii2model.py` | Gone. `tank.fbx` goes through `FbxImporter`/`ModelProcessor` and `Ground.x` through `XImporter`/`ModelProcessor`, and the port loads the resulting `Model` XNBs. |
| "Textures not assigned to model meshes — `.model.json` cannot specify them" | Gone with the format. The pipeline builds `engine_diff_tex.tga` and `turret_alt_diff_tex.tga` as model dependencies and the mesh materials carry them; the tank renders textured. |
| "Ground uses checker texture but renders untextured" | Gone. `Checker.bmp` is built as a dependency and the ground renders as the tiled blue-and-black checker, with `SamplerState::LinearWrap` as the original sets it. |
| "3D scene renders as white stripe (all CNA backends) … not yet root-caused" | **Does not reproduce.** See §4. |

## 3. Content

All **6** XNBs are byte-identical to this sample's own official pipeline output for the
Windows target (`cmp`), hashes for both platforms in `evidence/content-sha256.txt`. Three
of them — `Checker_0`, `engine_diff_tex_0`, `turret_alt_diff_tex_0` — are textures the
`ModelProcessor` pulls in itself; they are not listed in the content project.

`tank.fbx` needed `Microsoft.Xna.Framework.Content.Pipeline.FBXImporter.dll` in the runner's
`PipelineAssemblies`; the DLL was already being staged, but only the texture, `.x` and
effect importers were being declared.

## 4. The "white stripe", and what happened to it

The previous record's last entry described the entire 3D scene collapsing to a white stripe
on **every** CNA backend, blamed tentatively on near-plane clipping of `w<0` vertices, and
left explicitly not root-caused.

It does not happen. The faithful port renders the scene correctly the first time it was
run, and the idle frame matches the XNA original closely (§5).

What changed is what the port loads. The old record's arithmetic — "the ground corner
vertex at (6554,0,6554) … x+z=13108 exceeds the threshold ≈3000" — is computed on
*unscaled* coordinates, and the game draws the ground with `Matrix.CreateScale(.1f)`, which
puts that corner at 655 units with the camera at 1000 and a 10–10000 frustum: comfortably
inside. Those coordinates came from the hand-converted `ground.model.json`, which this port
no longer uses. No CNA change was needed, and none was made.

## 5. Verification

Both builds are EasyGL: native `OPENGLES3` and web `WEBGL2` under Emscripten. Both windows
are titled `CameraShake`, from the ported `AssemblyInfo.cs`.

**Unlike the two samples before it, this one is fully deterministic while it is not
shaking**: the camera sits at a fixed (1000,1000,1000) looking at the origin, neither model
moves, and `Camera::Update` touches nothing until `Shake()` is called. The idle frame is
therefore directly comparable, pixel for pixel (`evidence/comparison.txt`):

| Measure | Result |
|---|---|
| pixels within 8 levels of the original | **341425 of 384000 (88.9 %)** |
| pixels exactly equal | 38855 (10.1 %) |
| worst single-channel delta | 93 |
| clusters of >40-level difference, ≥30 px | **0** |

Zero clusters is the number that matters: the differences are scattered single pixels along
texture and geometry edges — the shading and filtering noise of two entirely different
graphics stacks on a mip-mapped textured scene — not a structural disagreement.

The shake itself uses `Random` and cannot be compared frame to frame, but its endpoints can:
in **both** engines the shaken frame differs from the idle one and the frame two seconds
later is **byte-identical** to it, so the camera returns exactly where it started.

The browser gate (`scripts/capture-web.sh`, `scripts/chrome-smoke.mjs`) asserts what the old
white-stripe failure would have broken: the scene covers 383317 of 384000 pixels with
near-white below 5 %, and the ground's lower half means (36, 56, 78) — blue-dominant and
dark, i.e. the checker really is textured. It also asserts that pressing A moves the camera
and that letting the shake finish returns the frame to exactly the idle one, an 800×480
WebGL2 canvas, `CNA: graphics renderer: WEBGL2`, `document.title == "CameraShake"`, and no
rejections, runtime exceptions, HTTP errors or fatal console messages. Full record in
`evidence/cna-web-webgl2/browser-result.json`.

## 6. C++ mapping notes — not deviations

- C#'s `VibrationSettings` is a class, so `Dictionary<PlayerIndex, VibrationSettings>` holds
  references and every write through the indexer mutates the shared object. The port stores
  `std::shared_ptr<VibrationSettings>`, which reproduces that; a by-value `TValue` would
  not, because sharp-runtime's `Dictionary` indexer hands back a .NET-style proxy rather
  than a mutable reference.
- `for (PlayerIndex p = One; p <= Four; p++)` becomes a loop over an explicit four-element
  array of the enum values.
- The manager clears its own static `instance` in its destructor. C# leaves that reference
  standing for the process's lifetime; C++ objects really are destroyed, and without this
  the "only one manager" rule would refuse a second game in the same process.

## 7. Deviations

None.
