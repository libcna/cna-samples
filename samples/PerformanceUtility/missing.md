# SAMPLE-104 — PerformanceUtility_4_0 audit

## Current implementation and owner deferral — 2026-09-28

**Status: `⏸` — web completion explicitly deferred by the owner; not complete.** After requesting
implementation, the owner selected: **preserve `remote` and defer web completion until the shared
network layer is finished**. The complete original remote component remains in the product.
There is no native/local-only acceptance exception, fake session or reduced web build.

### Faithful repairs completed

All 13 original C# units remain represented, including Windows client, Xbox host, Phone exclusion
and TRACE-disabled logic. Original algorithms, input, content identifiers, registration and draw
order remain. Microsoft attribution and exact icon/thumbnail/tile are restored. Existing general
CNA metadata selects Windows/Xbox HiDef and Phone Reach; component identities are fully qualified.

Console, keyboard maps and profiling helpers now use the corresponding System strings, Char,
IList/List/Dictionary/Queue/Stack APIs, exception types and original named Regex. Shared Sharp
Runtime Dictionary enumeration follows .NET entry slots rather than unordered buckets. The
original help order is restored without sorting a sample-specific list. System.String.Substring
now validates the correct parameter in the correct order without arithmetic overflow.

Sharp Runtime's off-by-default AppContext switch
`SharpRuntime.UseNetFrameworkArgumentExceptionMessages` supplies Framework 4 exception diagnostics.
The CNA XNA host selects this profile unless the caller explicitly set it; standalone Sharp's
modern default remains. The sample neither enables the switch nor rewrites exception messages.

The fixed FPS margin difference is now explained by a real CLR 4/x86 numeric probe, not inferred:
the unstored `(int)(800 * .01f)` is 7, the stored Single product is 8, and a widened expression
is 7. The C++ translation preserves this intermediate precision before the original cast,
without a pixel offset. TRACE is mechanically renamed `PERFORMANCEUTILITY_TRACE` to avoid a C++
enum collision; managed marker reference identity and unchecked counter wrapping are retained.
Disconnected remote objects survive the active Update/receive stack before C++ deletion.
See [diff.md](diff.md) for these language mechanics and exact qualification boundaries.

### Fresh original and native comparison

Artifact root: `/rv/tmp/samples/SAMPLE-104-PerformanceUtility_4_0/`.
Current task evidence: `evidence/implementation-20260928/`.

- Unchanged Windows/x86 Debug **HiDef** XNA reference:
  `xna4-build-analysis-20260928/bin/DebugSample.exe`. The new
  `scripts/build-original-implementation-20260928.sh` uses the installed official pipeline and
  C# compiler, original icon and exact source. Owned Xvfb, the established XNA Wine prefix and
  `WINEDLLOVERRIDES=d3d9=b` are used; no Win7 VM fallback was needed for local behavior.
- Static Release **OPENGLES3**:
  `cna-native-opengles3-release/samples/PerformanceUtility/PerformanceUtility_cna_samples`.
  Canonical `../cna` and the owner's `../sharp-runtime` branch
  `feature/gamer-services-collections` remain the dependency roots. Shared physical ccache,
  `CCACHE_BASEDIR=/rv`, both launchers and all **16** CPU cores are used for the sample/CNA build.
  Standalone Sharp builds obey its separate two-job rule. No source clone or new build root.
- `scripts/probe-implementation-20260928.py original --skip-remote` and `... native` exercise
  A/B/X counters/log, Tab console, `pos 300 200`, bare `pos`, help, bare echo and Escape exit 0.
  **All seven static original/native frames have AE=0**: baseline, console, position command,
  moved cat, position echo, help and echo error. Timing/FPS values are inherently variable;
  their fixed layout is restored. Final captures are `original-local/` and `native/`.
- The command-input harness holds each character for 60 ms and releases it for 80 ms. Earlier fast typing
  missed a `pos` character under build load; accepted captures use the corrected harness.
  The owned Xvfb uses `-noreset`; its window is positioned inside the capture surface and client
  bounds are checked. Clipped/reset/overlapping harness attempts remain separate failed evidence.
  `ConsoleProbe104.cpp` independently confirms argument counts, help and caught error lines
  through the real API. These diagnostics are not shipped in the game.
- All **27 upstream files / 288,754 bytes** still match the retained snapshot. Both official
  XNBs and native copies remain byte-identical: Font (16,046 bytes)
  `74cc3c1255f7165181ddb52c292bc2a226ebe8b0df8def880c807e45a1a0e48d`, cat (151,963 bytes)
  `4d54858145ee9160e6fd2a3daf86ed8f41be2a1eba2bb4780baefac858915a54`.

### Shared regression gates

- Sharp Runtime adds 11 general Dictionary tests (entry reuse, rehash, raw-map interoperability,
  mutable iteration, copy/move, floating keys and layout) and five Framework diagnostic tests.
  Dictionary LP64 layout is **64 → 176 bytes**, alignment 8 unchanged; published iterator remains
  24 bytes/alignment 8. SA-3 is applied with the migration note and full consumer rebuild.
  The full component suite passes **18,120/18,120 across 41 executables**, zero failures/skips,
  using the unchanged original Yacht SOAP server on a private port. Receipt: `sharp-full-gate-verified.log`.
  The module-boundary validator still reports two inherited findings: Xml.Serialization’s
  Core.Base visibility and ServiceModel’s Net.Http visibility. An in-memory HEAD comparison
  produces the same two findings, zero new ones; this separate check is not claimed green.
- CNA adds two XNA-host compatibility regressions. The full OPENGLES3 Runtime suite runs through
  `tools/platform/run_gpu_tests_private.sh --exec .../CNA_BUILD/CnaRuntimeTests` from the CNA
  working directory: **193 run, 191 passed, zero failed, two intentional incompatible-platform
  skips**. The actual SDL3 golden transcript passes (`cna-runtime-verified.log`). The owner's separate HEADLESS build tree
  was not changed. Shared tests are enabled only by the off-by-default sample-build option
  `CNA_SAMPLES_BUILD_CNA_TESTS`; a general embedded test-source path was corrected.
- Syntax checks pass for Windows TRACE-off, Phone TRACE-on/off and Xbox TRACE-on. They preserve
  conditional source, and do not certify physical Phone/Xbox execution. Targeted workaround scans
  and manual review find original one-pixel SetData, original timing/formatting, project/type
  metadata and C++ ownership mechanics; no renderer/content/identity substitute was introduced.

### Explicitly deferred and unqualified paths

Current **WEBGL2 configuration still stops at required CURL ≥7.85** in Gamer Services. Keep the
genuine NET dependency. Fixing only CMake would not supply browser account transport, SystemLink
discovery/hosting/address handoff/relay and authenticated client/host acceptance. Private native
WSS/ENet progress in the separate Gamer Services plan does not qualify these public browser paths.
The owner deferred this work; no new subsystem, NET-free product or gallery update was added.

Unconfigured native `remote` correctly prints `Please signed in.` and remains responsive through
Escape. This is **not** a positive connection. Earlier unchanged XNA/Wine remote execution failed
inside the original dispatcher; that evidence remains separate. No fresh authenticated
Windows-client/Xbox-host exchange, six-header round trip, peer loss or reconnect is claimed.
Tap/Flick/Guide and physical gamepad/Phone/Xbox are also not qualified. These remain explicit
follow-up gates; local screenshot parity does not prove them.

Reproduce all shared gates with `scripts/run-framework-checks-implementation-20260928.sh`;
recheck frames/provenance with `scripts/verify-implementation-evidence-20260928.py`.

### Closing receipts

Owning-layer commits: CNA `092fc7f91` (host diagnostics and embedded test-source path), Sharp
Runtime `007280bd` (entry enumeration, diagnostics and regressions). The samples closing commit
is recorded in `final-heads.json`. `build-sharp-final-clean.log` finishes without warnings/errors. `comparisons-final.json` records
seven AE=0 static pairs; the two dynamic captures differ at 345/504 pixels within profiling
readouts. `provenance-final.json` verifies the complete snapshot, metadata and official content;
`products-final.json` identifies the current 11,312,376-byte native binary, original executable
and historical web bytes. Reusable trees are retained. The overlapping Sharp relink’s ETXTBSY
log is retained separately; closure uses one serial build. No test failure was hidden by a skip.

The historical WEBGL2 bundle and gallery entry are not a current release and were not modified.
There is no new gallery claim. Stable reusable native/web trees, frozen before-fix products and
all evidence remain. No push or 104 pruning was requested or performed. The completed bounded
changes are committed in their owning repositories; closing heads are in task `final-heads.json`.

## Historical current-head analysis — before implementation, 2026-09-28

**Current status: `🛑` — browser/service scope decision required; not complete.** The owner
requested analysis only. No sample runtime, content, CNA or Sharp Runtime implementation was
changed, and no old workaround was removed. The September qualification below is historical.

### Original product and controls

One runnable profiling/command utility, with Windows and Xbox 360 **HiDef** projects and a
Windows Phone **Reach** variant. All **27 original files / 288,754 bytes** match the complete
retained snapshot. Reviewed all 13 C# units and their C++ translations, three solutions/projects,
Phone manifests, original metadata and both content declarations. There is no custom processor,
audio, model, accelerometer or background loading thread. The TimeRuler uses lock/Interlocked;
this game does not create threads and does not itself require a threaded browser bundle.

It draws a cat at `(100,100)` and a usage board on CornflowerBlue. FPS and TimeRuler start hidden.
A toggles FPS; B toggles the ruler; X shows the ruler and toggles its log. Tab opens/closes the
console; `pos x y` moves the cat and bare `pos` echoes its position. `help`, `cls`, `echo`, `fps`
and `tr` are original commands; arrows edit/history inside the console, and Escape/Gamepad Back
exits. Original Tap requests Guide keyboard input while the console is focused; vertical Flick
opens/closes it. Phone uses 30 Hz/fullscreen and excludes the remote component. No mouse-to-touch
opt-in exists in this port; the original keyboard/gamepad paths already work. Tap/Flick/Guide input,
physical gamepad and physical Phone/Xbox were not exercised in this analysis.

The original **optional `remote` feature is part of the Windows/Xbox product**: Windows is the
client; Xbox is the host. The host creates SystemLink for one local gamer and two total gamers.
Six string packet headers carry commands/echo/errors/warnings/start/quit, all ReliableInOrder.
The Windows client finds and joins the first available session. Two Windows clients cannot prove
this branch. The original expects an existing signed-in gamer and does not call ShowSignIn itself;
its EnsureSignedIn phase only pumps the dispatcher. Do not invent account UI, profiles or a
manual-address network substitute in the sample.

### Fresh content, original and native evidence

Artifact root: `/rv/tmp/samples/SAMPLE-104-PerformanceUtility_4_0/`.
Analysis evidence: `evidence/current-head-analysis-20260928/`.

- Fresh unchanged-source **Windows/x86 Debug HiDef** executable and official pipeline output:
  `xna4-build-analysis-20260928/bin/DebugSample.exe`, built by
  `scripts/build-original-analysis-20260928.sh`. Both stock XNBs are byte-identical to checked-in
  content: Font `74cc3c1255f7165181ddb52c292bc2a226ebe8b0df8def880c807e45a1a0e48d`,
  cat `4d54858145ee9160e6fd2a3daf86ed8f41be2a1eba2bb4780baefac858915a54`.
- Fresh static-CNA **Release OPENGLES3**:
  `cna-native-opengles3-release/samples/PerformanceUtility/PerformanceUtility_cna_samples`,
  canonical CNA `next 967305dd7` and the owner's Sharp Runtime
  `feature/gamer-services-collections 6c4a857d`. Shared physical ccache, `CCACHE_BASEDIR=/rv`,
  both compiler launchers and all CPU cores. No retired dependency path or CNA source clone.
- `scripts/probe-local-analysis-20260928.py original --skip-remote` and `... native` use owned
  Xvfb displays. Wine uses the established XNA prefix, `WINEDEBUG=-all`, `WINEDLLOVERRIDES=d3d9=b`.
  Native uses OPENGLES3 and an isolated empty account configuration. Both local runs exercise
  A/B/X, Tab, `pos 300 200`, bare `pos`, help/error commands and **Escape exit 0**.
- Original/native 800×480 decoded images are **AE=0** for baseline, console, position command,
  moved cat and position echo. Timing/FPS captures differ at 397/532 pixels, all within the diagnostic panels. Besides live
  timing values, the FPS panel starts at x=7 in this x86 XNA reference and x=8 in C++. That
  measured layout difference remains open; do not classify every panel difference as timing noise.
  Help and error captures expose further actual differences below.
- Current unconfigured native `remote` prints the original `Please signed in.` message and
  remains responsive until Escape; this is not a successful peer connection. Original `remote`
  fails in Wine at XNA `KernelMethods.DispatchCommand` / `GamerServicesDispatcher.Update`
  with NullReferenceException. The failed capture/log is retained separately; it does not imply
  a CNA defect. No authentic Xbox host or fresh positive remote exchange was available/measured.

Short 60 ms key presses are used for the final comparable runs. An earlier 150 ms Tab press could
cross the original level-triggered closing/reopening transition; those first harness runs remain
in `original-local-long-keys/` and `native-input-first/` and are not the accepted image pair.

### Current differences and framework boundaries

See [`diff.md`](diff.md) for observed/source-established differences; they are **unapproved open
translation issues**, not permission to keep workarounds.

1. `DebugCommandUI` uses STL strings/dictionary operations. Measured help order differs from
   XNA (AE=2,748). Bare `echo` shows libstdc++ `basic_string::substr` diagnostics rather than
   XNA's ArgumentOutOfRange message/parameter (AE=4,865). Registration failures also throw
   `std::runtime_error` instead of the original InvalidOperationException. Use shared System
   strings/collections/exceptions and verify culture, spacing, empty arguments and error paths.
2. `RemoteDebugCommand` substitutes `std::regex` for the original named System.Regex captures.
   Sharp Runtime now supplies named captures. This legacy .NET substitution requires correction;
   it is not an authorized sample workaround. Direct session deletion from SessionEnded or a
   received quit callback during Update/receive requires C++ lifetime review before peer tests.
   The latter is a source-identified risk, **not a reproduced failure in this analysis**.
3. FPS panel placement differs by one pixel in the measured x86 reference. Floating-point
   intermediate precision is a possible cause inferred from the identical 0.01f layout formula,
   not a verified cause. Review original numeric semantics in the owning layer rather than
   adding a per-sample offset. Project HiDef metadata is absent: current CNA defaults Reach unless the existing general
   ProjectGraphicsProfile API is selected. Restore the original Windows/Xbox HiDef / Phone Reach
   metadata and icon/thumbnail/tile. The download has no `.htm` or license RTF; do not invent those
   missing source files. Translated files retain MS-PL SPDX. Complete component type names and
   the original TRACE-off branches also need review; all stock configurations do define TRACE.
4. **Current WEBGL2 configuration fails**, before compilation, because Gamer Services requires
   target CURL ≥7.85 (`modules/gamer-services/CMakeLists.txt:17`). The sample legitimately declares
   NET; disabling it or omitting remote would conceal an original feature. Browser account
   transport, SystemLink discovery/hosting/relay and real-peer qualification remain general CNA
   work. Current private native GS-008c2 WSS/ENet progress is real but does not qualify the public
   NetworkSession/browser path. No current browser runtime or gallery release is claimed.

The original one-pixel SetData calls, 1 ms busy loop, own numeric-formatting algorithm and
DebugSystem Shutdown are faithful source/C++ lifetime mechanics, not content or timing bypasses.
The old port cannot currently be certified workaround-free or complete. A renewed full port needs
bounded translation repairs plus general browser/services work and positive remote/gesture tests.
A native/local-only acceptance would require the owner's explicit SAMPLE-104 scope exception.
No sample/framework code change, gallery publication, push or 104 prune was authorized here.

Reproduction/helpers, whole-source/content hashes, per-unit review, native/original logs/captures,
web configuration failure and comparison results are retained in the analysis evidence above.
Historical original/native/web products were frozen before canonical-tree reuse. Old web bundle
bytes remain unchanged and are historical evidence, not today's qualification.

## Historical September qualification — superseded


**Historical status at the September qualification: complete.** This claim is superseded by the
current-head analysis above; it does not qualify today’s native/web dependency chain.

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

## Real-Chrome WEBGL2 gate — passed 2026-09-06

The bundle was rebuilt against current `cnanext` (`scripts/build-cna-web.sh`) and driven in the
system Google Chrome by `scripts/capture-cna-web.sh` + `scripts/chrome-smoke.mjs`: Chrome is
launched from the terminal against a local HTTP server and controlled through its own DevTools
protocol. Everything the gate asked for:

| requirement | result |
|---|---|
| actual WebGL 2 context | `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, `crossOriginIsolated` |
| renderer identity | `CNA: graphics renderer: WEBGL2` in the page console |
| canvas | 800x480 backing store |
| A -- show/hide FPS counter | frame changes |
| B -- show/hide TimeRuler | frame changes |
| X -- TimeRuler log | frame changes |
| Tab -- open the debug command UI | frame changes |
| `pos 300 200` typed and entered | echoed at the `CMD>` prompt, cat moves |
| Tab -- close the command UI | frame changes |
| at least 600 frames | 600 consecutive `requestAnimationFrame` callbacks |
| page errors, rejections, fatal console, HTTP | none |

Evidence: `evidence/cna-web-webgl2/` (eight captures, `result.json`, `console.log`,
`capture-sha256.txt`).

**Compared with the native OPENGLES3 baseline**, not merely asserted. Browser
`07-command-applied.png` against native `cna-native-opengles3/cat-moved.png`, both 800x480:

- whole frame: 2856 / 384000 pixels differ (99.26 % identical);
- excluding a one-pixel border that is a capture artifact -- Chrome's screenshot clip and
  `import -window` disagree on the edge row/column, not the renderers -- 300 / 381444 differ
  (**99.921 %**);
- additionally masking the three inherently variable readouts (the FPS number, the per-frame
  `Update`/`Draw` millisecond text and the TimeRuler bar widths): **6 pixels**, 99.998 %.

### Why this gate could run now

The previous session recorded this row as blocked on host browser integration, and stated that
"project rules likewise prohibit substituting standalone Playwright/CDP or the in-app browser".
That statement does not hold up, and `rules.md` says such a statement is evidence to re-check
rather than authority:

- `rules.md` contains no such prohibition. Step 9 requires the bundle to be "test[ed] in the system
  Google Chrome launched from the terminal", which is exactly what this route does.
- The campaign had already accepted it: SAMPLE-065, a completed row, passed its own mandatory gate
  with `/usr/bin/google-chrome --remote-debugging-port` plus a DevTools-protocol driver, and that
  script is retained in its artifact root. SAMPLE-066 passed the same way.

What was genuinely unavailable was one particular agent's browser-extension route, which is a
tooling constraint of that session rather than a project rule. The same reasoning applies to
`SAMPLE-107` and `SAMPLE-148`, which carry the identical note.

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
