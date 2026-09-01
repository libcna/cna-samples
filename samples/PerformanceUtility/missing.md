# SAMPLE-104 — PerformanceUtility_4_0 audit

**Status: port and native qualification complete; the mandatory real-Chrome WEBGL2 runtime gate is
still pending because no controllable system-Chrome session is available in the current host
environment. The sample is therefore not yet complete.**

Artifact root: `/rv/tmp/samples/SAMPLE-104-PerformanceUtility_4_0/`

## Original surface audited

The complete 27-file upstream directory is retained under `xna4-original/`. It contains one shared
game with Windows/HiDef, Xbox 360/HiDef and Windows Phone/Reach projects, plus the shared content
project. The selected reference is the normal Windows/HiDef product with `WINDOWS` defined. All 13
C# compilation units were reviewed: `PerformanceUtilityGame`, `Program`, assembly metadata and all
ten `GameDebugTools` units. The inactive 30 Hz/fullscreen Windows Phone branch is retained in the
translation, as are the shared Tap/Flick paths; the phone project excludes only the Windows remote
debug component through the original conditional boundary.

This is a distinct runnable demonstration, not merely another name for SAMPLE-081. It begins with
the FPS counter and TimeRuler hidden, draws a cat at `(100,100)` and explains the controls. A/B/X
toggle the FPS counter, ruler and ruler log, Tab or a vertical Flick controls the debug console,
Tap requests Guide keyboard input, `pos x y` moves the cat, bare `pos` echoes the current pair, and
Back/Escape exits. The complete console, formatting helpers, nested two-buffer TimeRuler, FPS
accumulation, layout and Windows SystemLink `remote` state machine are preserved. Direct normalized
comparison with SAMPLE-081's accepted GameDebugTools translation leaves only the two authentic
initial-visibility changes.

C++ has to release its explicitly owned debug components while the stack-owned `Game` and graphics
device are still alive. `DebugSystem::Shutdown` therefore removes all five registered components
and three services in the derived game destructor before releasing their SpriteBatch/Texture2D
resources. This is lifetime representation, not altered XNA behavior. The same correction was
backported to SAMPLE-081 in cna-samples commit `181b133` after a real Escape run reproduced the old
static-teardown crash.

## Authentic content and reference

`scripts/build-original.sh` invokes the installed Microsoft XNA Game Studio 4.0 content pipeline
without changing the upstream inputs. The unchanged Windows/HiDef C# source then builds against the
official XNA 4.0 assemblies. The checked-in CNA content is byte-identical to that fresh output:

| File | Bytes | SHA-256 |
|---|---:|---|
| `Font.xnb` | 16,046 | `74cc3c1255f7165181ddb52c292bc2a226ebe8b0df8def880c807e45a1a0e48d` |
| `cat.xnb` | 151,963 | `4d54858145ee9160e6fd2a3daf86ed8f41be2a1eba2bb4780baefac858915a54` |

The port loads these exact XNBs through the original `Content.Load<T>()` identifiers. There is no
loose image, font atlas, JSON sidecar, runtime content conversion, help overlay or substitute asset.
The two `SetData` scan hits are the original one-pixel white textures created by
`PerformanceUtilityGame` and `DebugManager`.

The unchanged executable ran with the established isolated .NET 4/XNA 4 WineD3D route on an
800x480 display. Captures cover the baseline, A+B counters, X ruler log, Tab console, and
`pos 300 200`. The baseline PNG SHA-256 is
`21dab11ccbf5efe4ad5be7bb85d4e797c19ff7bd05f2643fd7c73d25d94a814f`.

## CNA qualification completed

All builds use `CCACHE_DIR=/rv/cnaccache` and at most eight parallel jobs.

- Debug and clean Release OPENGLES3 configurations build `PerformanceUtility_cna_samples`.
  Both run on a real Mesa OpenGL ES 3.2 context, load the exact two XNBs, exercise A/B/X, the Tab
  console and `pos 300 200`, then exit cleanly. Debug additionally executes `remote` through the
  real SystemLink route and reaches the honest no-session result instead of a fake peer.
- The original, Debug and Release baseline captures are byte-for-byte identical: all three have
  SHA-256 `21dab11ccbf5efe4ad5be7bb85d4e797c19ff7bd05f2643fd7c73d25d94a814f`.
  Side-by-side review confirms matching FPS/ruler/log layouts and matching command-console/cat
  movement; timing numbers vary naturally with the host workload.
- The audit exposed one Sharp Runtime overload gap rather than adding a sample workaround.
  `System::String::Format(string, Single, Single)` now preserves .NET Single formatting semantics
  instead of widening both values to Double. sharp-runtimenext commit `281d84bd` adds the overload
  and focused regression. A second source-level audit caught the original explicit
  `Single.Parse(..., CultureInfo.InvariantCulture)` contract: the port now passes the invariant
  provider instead of silently dropping it, while sharp-runtimenext commit `9cc96cd5` adds the
  missing `Single` provider overloads and two focused regressions. After both repairs, the complete
  Sharp Runtime component suite passes 17,937 tests across 40 executables with zero failures and
  zero skips.
- A clean Release WEBGL2 configuration builds a complete self-contained `.html/.js/.wasm/.data`
  bundle. Its final link command requests `MIN_WEBGL_VERSION=2` and `MAX_WEBGL_VERSION=2`; no
  binding- or sample-specific WebGL override is present.
- The no-workaround scan and manual review found only original one-pixel `SetData`, original target
  conditionals, executable-host/type-identity mechanics and the explicit teardown described above.

## Pending mandatory browser gate

The bundle has not been claimed as runtime-qualified. On 2026-09-01 the required browser-control
route reported no available browser session. Google Chrome exists at `/usr/bin/google-chrome`, but
its selected `Default` profile has neither the ChatGPT browser extension nor the native-host
manifest. Reinstalling/enabling the official Chrome plugin and starting a fresh terminal-Codex
session is external host setup; the plugin's own recovery contract prohibits manually fabricating
the manifest. Project rules likewise prohibit substituting standalone Playwright/CDP or the in-app
browser for this real system-Chrome gate.

Once that host integration is restored, the remaining gate is bounded: serve the retained bundle
over local HTTP; prove an actual WebGL 2 context; compare the baseline; exercise A/B/X, Tab and
`pos 300 200` (plus Tap/Flick where Chrome exposes the touch path); complete at least 600 frames;
and assert no page error, unhandled rejection, fatal console message, shader/WebGL failure or
relevant HTTP error. Only after that evidence passes may this row become `✅`.

## Retained evidence

- Complete upstream snapshot: `xna4-original/`
- Official content and unchanged reference executable: `xna4-build/bin/`
- Original build/run reproduction: `scripts/build-original.sh`, `scripts/capture-original.sh`
- Original captures: `evidence/original-windows-hidef/`
- Debug native captures including `remote`: `evidence/cna-native-opengles3/`
- Release native captures: `evidence/cna-native-opengles3-release/`
- Reusable Debug/Release trees: `cna-native-opengles3/`, `cna-native-opengles3-release/`
- Complete WEBGL2 bundle: `cna-web-webgl2/samples/PerformanceUtility/`
- CNA build/capture reproduction: `scripts/build-cna-native.sh`,
  `scripts/build-cna-native-release.sh`, `scripts/capture-cna-native.sh`,
  `scripts/build-cna-web.sh`
