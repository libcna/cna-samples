# SAMPLE-083 — Snow Shovel audit

## Current-head analysis before requalification — 2026-09-27

**Status: current-head requalification pending.** The physical 20-file upstream directory still
matches `xna4-original/` byte for byte, and each of the five checked-in Windows/Reach XNBs is
byte-identical to the retained official XNA 4.0 output. The selected Windows product is a
480×800 arcade game with pre-game, ten-second game, and post-game states: Space or touch starts
and restarts, arrow keys/gamepad/touch steer the shovel, caught snowflakes score and play `plink`,
and Escape or gamepad Back exits. The parallel Phone project shares the source and adds the
accelerometer and 30 Hz/fullscreen branch. There is a keyboard/gamepad route for the actions, so
the owner-requested touch-only mouse opt-in does not apply to the selected Windows product.

The checked-in game source has not changed since the historical port; a focused scan found the
original `Content.Load` calls, normal keyboard/gamepad/touch paths, the conditional Phone sensor,
and only CNA's required runtime type-name marker. No sample-side renderer, loose-content, F1 or
mouse workaround was found. This analysis does **not** reassert the old runtime qualification on
current CNA `next d1dde5d73` and Sharp Runtime `next 9e58c955`: the retained native Release
executable is from 2026-09-09 and its RUNPATH names retired `openeggbert/cnanext`, while the
2026-09-01 WEBGL2 JS contains 35 `PThread` markers even though the source has no threading
requirement. Build helpers still use the retired `openeggbert` checkout and omit the current
`CCACHE_BASEDIR=/rv` setting. Both CMake trees were pruned; the original XNA executable and
historical captures remain. No `SnowShovel` gallery page or bundle exists yet.

Five upstream package files are absent beside the port: the Microsoft Permissive License, game
icon, thumbnail and two Phone manifests. Restore the original files without placing them in
runtime `Content`. For completion, repair the retained helpers for the active checkout and shared
ccache, rebuild the unchanged XNA original and both CNA targets, compare the original/native
pre-game and interaction states, check score plus audio and clean exit, and run a nonthreaded
ordinary-HTTP WEBGL2 bundle in system Chrome through start, movement, Game Over and restart with
600 further frames and error checks. Then publish and test the exact bundle with a genuine game
screenshot. The previous Phone conditional compile should be repeated on the active CNA head.
The current inventory and exact hashes are in
`/rv/tmp/samples/SAMPLE-083-SnowShovelSample_4_0/evidence/current-head-analysis-20260927/inventory.json`.

## Historical completed-port audit

The historical qualification recorded no known behavior or content differences from the XNA 4.0
original; the current-head gates listed above are pending.

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

---

## Re-audited 2026-09-09

Verified independently: all three original C# units have counterparts, all five checked-in XNBs are
byte-identical to the official pipeline output, and the source carries no desktop sensor path, no
mouse path and no `F1` overlay. The port is 635 lines against the original's 667.

The two crops this document names were re-measured and both hold: the **bottom elapsed-time band is
bit-exact**, RMSE `0 (0)`, and the **title band is `0.000195`** — a couple of antialiased pixels,
inside the eight levels claimed. A whole-frame score against the reference is 0.11–0.16 on every
frame and every product, which is the snow and nothing else.

Two further measurements, beyond what this document claims:

- **Only 4.8–6.0 % of the frame's pixels differ at all.** The rest — title, timer, score labels,
  the three lines of instruction text, the shovel and the elapsed-time line — is the same picture.
- **The shovel, which is at a fixed position, matches where it is not snowed on.** Of the 4,368
  distinctly red pixels in the XNA pre-game capture, 731 differ in CNA's, and **671 of those 731
  are covered by a snowflake** in the CNA frame. Sixty pixels remain, 1.4 % of the shovel, at the
  edges where a flake's alpha blends into the boundary.

So the nondeterminism is confined to the snow, exactly as documented, and the deterministic content
agrees.

**Documentation.** The headers carry 11 `@brief` at 7 % comment density, below the campaign's 28 %
median. Not unique to this sample — 24 of the 99 samples are in the same position, recorded as one
deferred item in `plan.md`.

Nothing needed correcting.
