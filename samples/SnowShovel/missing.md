# SAMPLE-083 — Snow Shovel audit

**Status: complete — no known behavior or content differences from the XNA 4.0 original.**

Artifact root: `/rv/tmp/samples/SAMPLE-083-SnowShovelSample_4_0/`

## Original surface audited

The complete 20-file `SnowShovelSample_4_0` directory is retained under `xna4-original/` and its
manifest and SHA-256 inventory are recorded at the artifact root. The upstream ships two project
wrappers over the same `Game.cs`, `Program.cs`, assembly metadata and content project:

- Windows/Reach defines `WINDOWS`, runs in a 480×800 window and uses keyboard, game pad and touch;
- Windows Phone/Reach defines `WINDOWS_PHONE`, selects 30 Hz/fullscreen presentation, suppresses
  keyboard state and adds the `Accelerometer::ReadingChanged` path.

The port now mirrors the original `SnowShovel::Game` type and its private nested `Snowflake`.
Both conditional products are represented in the same source as upstream. The normal native/web
product selects the original Windows behavior; a separate `-DWINDOWS_PHONE` build proves the
complete Phone-only branch compiles against CNA's real legacy sensor API.

The translation retains the 272×480 logical world, 480×800 presentation, pre-game/game/post-game
states, wave growth and time bonuses, snow bounce and tint/spin ranges, shovel acceleration and
clamping, collision scoring/sound, all original text, drop shadows and the exact input mappings.
The nested and outer `Random` instances remain independent. Random calls are sequenced explicitly
where C++ argument evaluation would otherwise lose C#'s left-to-right order: spawn X/Y, velocity
X/Y and texture index, then snowflake scale, angular velocity and R/G/B tint.

The old port's behavior changes are gone:

- no always-on desktop accelerometer wrapper;
- no invented mouse start/restart/movement path;
- no F1 overlay or runtime `help.png` load;
- no fixed viewport constants or manual `TouchPanel` dimensions;
- no manual time-formatting helper;
- no public standalone `Snowflake`, renamed game type or merged RNG stream.

## Authentic content

The unchanged XNA Game Studio 4.0 content declarations were executed through the official pipeline
assemblies for both Windows/Reach and Windows Phone/Reach. The checked-in files are the Windows
outputs used by the unchanged runnable reference and the selected CNA product:

| File | Bytes | SHA-256 |
|---|---:|---|
| `ScoreFont.xnb` | 21,678 | `58a2e9a873b99768a720abec69811792d2b6e3ce7f27043eea643c3d3aef3f08` |
| `TitleFont.xnb` | 21,678 | `4eefa823f31e700ce41ade8fc31ee4cb733bfa568eab286a75f6a904fa97a0e3` |
| `plink.xnb` | 94,345 | `e426183cb01fc864d63bbbb9cf525469616bb2b3da8d55f1181472405a01b3d5` |
| `shovel.xnb` | 21,691 | `6fdabb174c9717efbc4761ba32d6c6b00f86c331be5e840b73a014b2254e565b` |
| `snowflakes.xnb` | 82,107 | `36ceee90702a72758314e93bf7a594fbf747d8572fc65fb5049958cfd9463b98` |

`evidence/xna-content-sha256.txt` also records the five platform-specific Phone outputs. The old
DejaVu font atlases/JSON, loose PNGs and loose WAV are removed. Runtime code uses only the original
identifiers `shovel`, `snowflakes`, `TitleFont`, `ScoreFont` and `plink` through
`Content.Load<T>()`. The original `SnowShovel.htm` is byte-identical (SHA-256
`073e325f9b0d3fde69a7d9dba53263d898924bd494742e56eb9faa3b2e83509b`). Per repository policy,
the historical port-only `help.png` remains beside `CMakeLists.txt`, outside `Content`, and is
neither packaged nor loaded.

## Stale blocker checks

The prior `missing.md` reported a 1333×800 `GraphicsDevice.Viewport` during `Initialize()` and
hard-coded 480×800 as a workaround. Live CNA already resets the existing device from
`GraphicsDeviceManager` preferences and refreshes its viewport before the game callback. The
faithful query now produces a real 480×800 window and a complete correctly scaled frame in both
Debug and Release OPENGLES3. No sample constant and no new CNA change was needed.

The prior port also manually implemented `"00"`/`"00.0"`. Sharp Runtime commit `9c389f86`
provides general custom numeric-picture handling, and the focused SAMPLE-083 follow-up adds the
missing mixed `(intcs, float)` `System::String::Format` route so the original Single expression is
not widened to Double (`sharp-runtimenext 1f5bbbc2`). The port now executes the two original
composite format strings directly.
Captured original and CNA HUDs both show `00:00.0`, live tenths and the red countdown.

## Original XNA qualification

`scripts/build-original.sh` compiles the unchanged Windows sources with the in-prefix .NET 4/XNA
4 compiler and builds both content targets through `BuildContent`. `scripts/capture-original.sh`
runs the resulting `SnowShovel.exe` under the established offline Wine prefix and WineD3D on an
isolated Xvfb display.

The reference window is exactly 480×800. Captures prove pre-game instructions, Space start,
Right/Down shovel acceleration, live countdown, automatic Game Over at ten seconds and Space
restart/reset. The longer synthetic restart press intentionally reaches the original next-frame
level-trigger behavior and starts a fresh round, showing the new 9.3-second timer and reset elapsed
time. The unmodified executable exits cleanly.

## CNA qualification

- Debug OPENGLES3 builds and runs the same four-state scenario. All five XNB readers succeed, the
  real window and captures are 480×800, arrow input moves/rotates the shovel, collisions increase
  score and enter the same `SoundEffect::Play` branch, Game Over/restart work and Escape exits.
- A clean compile-only Debug build with `WINDOWS_PHONE` succeeds, including sensor construction,
  the legacy `ReadingChanged` event, `Start()`, 30 Hz/fullscreen setup and accelerometer override.
  No physical accelerometer or Windows Phone runtime is available, so this is not described as a
  real-device sensor test.
- Release OPENGLES3 builds and repeats the complete four-state runtime capture successfully.
- Release WEBGL2 builds a self-contained `.html/.js/.wasm/.data` bundle and runs in the system
  Google Chrome. The browser obtains `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, renders at 480×800,
  starts and moves the shovel through actual browser touch events, increases the score, reaches Game Over
  and restart, then completes 600 additional animation frames. There are no page exceptions,
  unhandled promise rejections, fatal console messages or relevant HTTP errors.
- The browser image gate finds meaningful rendered pixels in all five captures, the red shovel,
  cyan Game Over instructions and more than 29,000 changed pixels across each requested state
  transition. Against the nondeterministic XNA pre-game capture, the snow-free title crop is 100%
  within eight channel levels and the bottom elapsed-time crop is pixel-identical; random
  snowflake positions intentionally prevent a whole-frame deterministic score.

Build, run, browser, image, console and checksum evidence is retained under `evidence/`; reusable
commands are under `scripts/`. No CNA source change was required. Sharp Runtime gained the bounded
mixed `(intcs, float)` composite-format overload and its regression test.

## Known differences

None. Necessary C#-to-C++ ownership and list-erasure representation is documented in `diff.md`;
it does not change observable behavior.
