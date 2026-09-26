# SAMPLE-067 — Catapult Wars audit and qualification

## Current-head requalification — 2026-09-26

**Status: `✅` for the automated native/browser gates on cna-samples `develop`
`c868b9a`, CNA `next` `cefe6c83b`, SharpRuntime `next` `d86adb65`.**
The owner's separate request to test the game in person (2026-09-07) is
still outstanding. Do not interpret this automated result as that verdict,
and do not apply an artifact prune without a new owner instruction.

The unchanged 225-file upstream snapshot was compared again with the physical
source; `diff -qr` is empty. The selected runnable endpoint is the final EX2
PolishAndMenus End Phone/Reach game, not an intermediate training stage. Its
21 C# units compiled against XNA 4.0 with the separately labelled
Phone-haptics diagnostic shim. The original content project rebuilt all
33 XNBs plus its animation XML copy item; all 34 files match the repository
`Content/` and the rebuilt native product byte for byte. There is no local
Windows Phone host, so this remains a source/content and behavior audit,
not a live visual-original screenshot comparison. Source and content parity
are logged in `evidence/requal-20260926/source-content-parity.txt`.

New Release OPENGLES3 and Release WEBGL2 builds use the active repositories,
not the retired `openeggbert` checkout. The native executable finds a
colocated `libcna.so` through `$ORIGIN`; the web bundle uses pthreads,
offscreen framebuffer, Asyncify and Emscripten 6.0.9. Its 9.1 MB wasm has no
debug sections. The `.data` SHA-256 remains
`a5939b26ff7322047b94401095ed4a3777465a57e286db2083391519b43049ca`.
Build logs and reproducible helpers are under the external artifact root
`/rv/tmp/samples/SAMPLE-067-CatapultWars_4_0/`.

The current native capture traversed main menu → instructions/background
load → full gameplay → mouse-as-touch `FreeDrag` aim → `DragComplete` fire →
pause → Quit Game → main menu → clean Exit (process 0). The aiming and fired
frames show the state change; `06-pause.png` shows the real pause choices.
The Xvfb automated gate uses the already owner-approved CNA mouse-to-touch
extension for normal left-button input. An **external diagnostic**
`sdl-escape-probe.c` exposes a held right button as the Phone Back/Escape
keyboard snapshot because synthetic X11 key events do not update SDL's
`GetKeyboardState` under this Xvfb. It is in the artifact `scripts/` and is
not part of the sample or shipped binary. A repeat of the **entire clean
native path**, recorded from a private PulseAudio sink, produced 69.10 s of
44.1 kHz stereo PCM (mean −34.8 dB, peak −1.1 dB) and exited with code 0.
Evidence: `evidence/requal-20260926/native-final/` and
`native-audio-green/`.

The new threaded bundle passed **real Chrome** in two input modes. Mouse
input (no browser touch emulation) opened the instructions, completed the
background load, aimed and fired, then ran 600 more animation callbacks;
the browser reported `crossOriginIsolated=true`, WebGL 2, 800×480 canvas,
zero exceptions/rejections/HTTP failures. A separate real browser-touch run
reached the same gameplay, fired, displayed the actual pause menu, returned
to the title and selected its Exit choice; 600 frames and the same error
checks passed. **Firefox 140 ESR**, where the old Emscripten 6.0.3 mailbox
bug was exposed by absent `Atomics.waitAsync`, independently reached
gameplay through the loading thread, aimed/fired and ran 600 callbacks with
`moduleAbort=null` and no page error. Native, Chrome and Firefox gameplay
frames pass the three-patch sky/mountain/WIND pixel gate calibrated on the
historical black-texture failure. Browser audio is audible too: 88.79 s of
44.1 kHz stereo PCM, mean −27.2 dB, peak −1.1 dB. Evidence:
`evidence/requal-20260926/{web-direct,web-touch,web-firefox,web-audio}/`.

The gallery now includes Catapult Wars as its 64th card, with a current live
gameplay screenshot, detail page and exact copies of the four rebuilt bundle
files. The scoped `coi-sw.js`/`launch.html` supply isolation on ordinary
static HTTP for this genuinely threaded game. A separate real-Chrome run of
the **staged gallery bytes**, served from the gallery root, passed menu,
instructions, gameplay, aim/fire, 600 frames, WebGL 2, pixel gate and all
four asset HTTP 200 checks with no runtime failure. The first gallery probe
failed because its test-only CDP `ignoreCache` reload bypassed the service
worker; the test now performs a normal reload in static-hosting mode, as the
previous threaded gallery gates do. Evidence:
`evidence/requal-20260926/web-gallery/`.

The source port did not change in this pass. There is no new sample-side
workaround or outstanding CNA/SharpRuntime fix for 67. The one deliberate
input extension remains the owner's mouse-to-touch opt-in recorded in
`diff.md`; the game screens continue to consume the original touch gestures.
The fresh original-build helper only gained `cp --remove-destination` to
cope with the historical artifact's hardlinked copy of its XML file.

## Pre-implementation current-head analysis — 2026-09-26

**Status at the time: `🔎` pending requalification on cna-samples `develop` `8b951bf`,
CNA `next` `cefe6c83b`, and SharpRuntime `next` `d86adb65`.** This is a
read-only source/artifact audit. It did not rebuild or run 67, change sample
or dependency code, or supersede the historical passing gates below.
Artifact root: `/rv/tmp/samples/SAMPLE-067-CatapultWars_4_0/`.

The 225-file physical `CatapultWars_4_0` matches `xna4-original/` without
any difference. It is a Phone training kit with EX1 and EX2 Start/End stages;
the selected runnable product remains the **final EX2 PolishAndMenus End**
Phone/Reach endpoint, with 21 game C# units. The retained official pipeline
output `xna4-build/Content-phone/`, the repository `Content/`, and deployed
native `Content/` match across all 34 files: 33 XNBs and one animation XML.
The earlier unchanged C# build used a labelled haptics-only diagnostic shim;
there is no local Phone application host, so the old build is source evidence,
not a live visual-original comparison. The repository retains the original
tutorial, license and historical help image in the required location.

The retained native product is dated 2026-09-09, contains no colocated
`libcna.so` and still has a `RUNPATH` into the retired
`openeggbert/cnanext` checkout. The retained WEBGL2 bundle is dated
2026-09-06 and uses pthreads/offscreen framebuffer for the source's real
background loading thread; its 115,598,872-byte wasm contains debug/name
sections. The installed Emscripten is now 6.0.9, but those products do not
prove behavior against the active CNA/SharpRuntime heads. The generated
artifact `MANIFEST.md` gives retired `openeggbert` restore paths, and
`scripts/capture-native.sh` names the same checkout's SDL headers. Refresh
the build and capture helpers before the current-head gates.

The current source still calls ordinary `Content.Load<T>()`, keeps the
background `System::Threading::Thread`, gesture input, audio manager,
animation XML and screen persistence. A targeted scan found no `NOXNA`,
`__EMSCRIPTEN__` gameplay branch or replacement content loader. The
`CNAEXT` hits are the documented C++ type/screen registry mechanics and the
single owner-requested `TouchPanel` mouse-to-touch opt-in in `diff.md`;
no new sample workaround or framework defect is proven by this static pass.
The historical 20 post-fix Chrome/Firefox pixel gates below are valuable
evidence for the worker texture race, but they predate the current heads.

There is no `CatapultWars` card, detail page or bundle in the gallery.
Because the original uses a loading thread, a refreshed web bundle must
keep Emscripten threads and be tested on static hosting with a scoped
COOP/COEP service-worker launcher, as established for later threaded samples.
The older evidence has no captured audible output despite six official
SoundEffect XNBs and the game's active `AudioManager`; capture native and
browser audio. Exercise real menu and drag/fire input, background-loaded
textures, pause and clean exit on current Release OPENGLES3 and in real
Chrome; repeat the relevant Firefox gate because that browser exposed the
earlier Emscripten bug. Preserve the owner-approved mouse input and seek the
owner's outstanding hands-on verdict separately. Until these checks pass,
the historical `✅` does not establish current-head completion.

## Historical qualification and owner test request — 2026-09-06/07

## Owner test pending

Asked by the owner on 2026-09-07: the owner will test this sample in person when at the computer.
Every automated gate recorded below passes -- native Debug/Release OPENGLES3, real-Chrome WEBGL2
(touch and mouse) and real-Firefox WEBGL2, twenty post-fix browser runs with per-pixel checks --
but the owner's own hands-on verdict is deliberately still outstanding. Until it is given, do not run
`tools/prune-completed-sample.sh --apply` on this artifact root, and do not read the `✅` row as
that confirmation.

Two things changed underneath the build the owner last saw: the machine's emsdk went 6.0.3 -> 6.0.9, and
`cnanext cde325ecd` restored the renderer thread-context lease on Emscripten. Both are described
below, with the evidence.

## Fixed: Firefox aborted when the background loading thread started

Reported by the owner on 2026-09-06 and reproduced here, deterministically, in Firefox 140.10.1esr.
Two separate causes; both are now closed, and neither was a sample or a CNA defect in the end.

**First cause -- the pthread pool (fixed 2026-09-06, `cna-samples d97510a`).** The console showed
*"Tried to spawn a new thread, but the thread pool is exhausted"* several times. Sharp Runtime
preallocates one Emscripten worker by default (`SHARP_RUNTIME_EMSCRIPTEN_PTHREAD_POOL_SIZE`), and
`InstructionsScreen::HandleInput` starts a loading thread for **every** `Tap` in the frame's
gesture queue -- faithfully; the original C# does the same, with no `break` and with `isLoading`
set inside the loop. A pool of one cannot serve that, and Emscripten can only grow the pool by
returning to the event loop, which a blocking `Game::Run()` under Asyncify does not do in time.
`cna-samples` sets the pool to 8 for its threaded web builds.

**Second cause -- an Emscripten 6.0.3 regression.** After the pool fix Firefox still died with a
bare `Aborted(Assertion failed)` on the first asset the background thread touched, while Chrome
played the same bundle through. The stack named the site exactly: `assert` at glue line 588,
called from `onmessage` at 1595, i.e. the main thread's `worker.onmessage` relay in
`loadWasmModuleToWorker`. The chain, each link measured rather than assumed:

1. **Firefox ESR 140 has no `Atomics.waitAsync`.** Probed directly in this browser on a
   COOP/COEP page: `typeof Atomics.waitAsync === "undefined"` on the main thread *and* in a
   worker (caniuse: Firefox 145+, Chrome 90+, Safari 16.4+). Emscripten therefore sets
   `waitAsyncPolyfilled`, `_emscripten_thread_mailbox_await` registers nothing, and the main
   thread's `waiting_async` stays 0.
2. **Every GL call from a loader pthread is proxied to the main thread.** With
   `-sOFFSCREEN_FRAMEBUFFER=1`, `system/lib/gl/webgl1.c` routes each call through
   `emscripten_sync_run_in_main_runtime_thread`, which posts to the main thread's mailbox. So a
   `Texture2D` upload on the loading thread is exactly what triggers the notification.
3. **`emscripten_thread_mailbox_send` then takes the postMessage route** (`thread_mailbox.c`:
   `if (thread->waiting_async) __builtin_wasm_memory_atomic_notify(...) else
   _emscripten_notify_mailbox_postmessage(...)`), sending `{targetThread: <main>, cmd: 4}`.
4. **Emscripten 6.0.3's main-thread handler asserts on that message** instead of processing it:
   `if (d.targetThread) { assert(d.targetThread != _pthread_self()); ... }`. The message is
   addressed to the main thread itself, so the assertion fails and the module aborts. In a build
   without ASSERTIONS the same message would be dropped and the loader thread would hang instead.

The regression is upstream `41656d690b` (#27018, 2026-05-28) and the fix is upstream `8d056a6d50`
("[pthreads] Fix relaying of mailbox notifications to main thread", #27336, 2026-07-14), whose
message describes this exact scenario including Firefox. Release 6.0.3 was tagged 2026-07-13 --
one day before the fix -- so it is the last release carrying the bug; 6.0.4 onwards restores
`if (d.targetThread && d.targetThread != _pthread_self())` and falls through to `checkMailbox()`.

**Resolution, on the owner's decision of 2026-09-06: upgrade the toolchain.** `~/emsdk` was moved
from 6.0.3 (`db04e88`) to **6.0.9** (`5eb0bde`, tool
`releases-f04ea239d533260dd1db760dd2d668d5f9a88d6b`) with `git pull && ./emsdk install 6.0.9 &&
./emsdk activate 6.0.9`. Neither `cnanext`, `sharp-runtimenext` nor this sample was changed for
it: the defect was in the toolchain, so a workaround in any of the three would have been exactly
what the zero-workaround policy forbids.

Because the toolchain changed, the artifact was rebuilt from scratch rather than relinked --
`.sdl-prebuilt-emscripten-pthreads` (built 2026-08-30 with 6.0.3) was deleted and regenerated, and
the whole `cna-web-webgl2` tree was reconfigured from a preload cache reproducing its previous
configuration exactly. Configure 2 m 6 s, build 3 m 30 s at `-j16`. The preloaded content is
unchanged and provably so: `CatapultWars_cna_samples.data` still hashes
`a5939b26ff7322047b94401095ed4a3777465a57e286db2083391519b43049ca`.

| | before (6.0.3) | after (6.0.9) |
|---|---|---|
| `…_cna_samples.js` | `62d3d226d8c3c95c…` | `4e58dd1706137baf…` |
| `…_cna_samples.wasm` | `35e86f53573b37dc…` | `f87fbb535ffc9eaa…` |
| `…_cna_samples.data` | `a5939b26ff732204…` | `a5939b26ff732204…` (identical) |
| glue relay line | `if (d.targetThread) { assert(…) }` | `if (d.targetThread && d.targetThread != _pthread_self())` |

**Firefox evidence.** `scripts/capture-web-firefox.sh` + `scripts/firefox-mouse-smoke.mjs` are new
and are the Firefox counterpart of the Chrome mouse gate: headed Firefox on a private Xvfb display
(`:137`), driven over **WebDriver BiDi** -- Firefox 140 starts BiDi rather than CDP, and its CDP
shim has no `Input.dispatchMouseEvent` at all, so `input.performActions` produces the pointer
input, `log.entryAdded` carries the console and every uncaught error, and
`browsingContext.captureScreenshot` takes the frames. The gate also installs `Module.onAbort`, so
a bare `Aborted(...)` cannot pass unnoticed.

Five Firefox runs on the rebuilt bundle (`evidence/cna-web-webgl2-firefox`,
`…-firefox-run1..run3`): every one reaches gameplay through a real pointer, aims with `FreeDrag`,
fires with `DragComplete`, and renders 600 further frames. `moduleAbort: null`, `pageErrors: []`,
`fatal: []`, `crossOriginIsolated: true`, WebGL 2 confirmed, canvas 800x480, and
`atomicsWaitAsync: "undefined"` recorded in every result -- that is, the passing runs are on
precisely the browser configuration that used to abort. The Chrome gates were re-run on the same
rebuilt bundle and still pass: the mouse gate four times (`evidence/cna-web-webgl2-mouse`,
`…-mouse-run1..run3`) and the original touch gate once
(`evidence/cna-web-webgl2-qualified`), with the identical menu screenshot hashes it produced
before the upgrade (`da2357d8…`, `1404bb21…`).

The pre-upgrade artifacts are kept for comparison in `evidence/emscripten-609-upgrade/` (the
6.0.3 glue, its relay lines and the bundle hashes) and
`evidence/pre-emsdk609-cna-web-webgl2-{mouse,qualified}/`.

## Fixed: assets loaded on the background thread were intermittently empty

Found on 2026-09-06 immediately after the abort above stopped hiding it -- in Firefox the game had
never reached this screen before, so it was newly *visible*, not newly introduced. Fixed the same
day in `cnanext cde325ecd` on the owner's decision.

**What was seen.** On the gameplay screen, a subset of the textures `GameplayScreen::LoadAssets`
loads on the background thread rendered pure black, with nothing reported anywhere: no GL error,
no exception, all 33 `Loading asset` lines present, the functional gate green. The affected set
varied between runs -- one Firefox run lost `sky`, `mountain` and `HUDFont`; the Chrome touch run
lost `gameplay_screen`, both clouds, `sky`, `mountain` and `HUDFont` but kept the HUD frames and
both catapults.

**Why.** The renderer's thread-context lease
(`IGraphicsRenderer::AcquireThreadContextLeaseEXT`) is what serializes a content load against a
frame: `GraphicsDeviceManager::BeginDraw` holds one for a whole frame and `ContentReader` holds one
for a whole asset. `EasyGLRenderer`'s implementation returned **null under `__EMSCRIPTEN__`**,
because the web has no thread-affine context ownership to hand over -- true of the binding half of
the contract, wrong about the exclusion half. Meanwhile `-sOFFSCREEN_FRAMEBUFFER=1` proxies *each
individual GL call* to the browser thread, where every thread's calls run against one shared
context, so a loader's `glBindTexture`/`glTexImage2D` pair could be split by the frame's own binds
and the upload landed on whatever the frame had bound.

**The fix** makes the lease body common to both platforms and keeps only the binding handover
native-only. The main thread waits on the same recursive mutex it already used natively; that is
safe on the web because Emscripten's main-thread `emscripten_futex_wait` keeps servicing the proxy
queue while it spins, so the loader's proxied GL calls still complete and release the lease. A
background load and a frame now exclude each other on the web exactly as they do natively.

**Evidence, by pixel rather than by assertion.** The functional gate cannot see this defect, so
each run's gameplay frame is sampled at three patches that must not be black (`sky`, the mountain
peak, the `HUDFont` "WIND" caption):

| | runs | defective |
|---|---|---|
| Firefox 140 ESR, before | 5 | **2** |
| Chrome headless, before | 5 | **1** |
| Firefox 140 ESR, after | 10 | **0** |
| Chrome headless, after | 10 | **0** |

All 20 post-fix runs also pass the full functional gate (menu, instructions, gameplay, `FreeDrag`
aiming, `DragComplete` fire, 600 further frames, no abort, no page error, no HTTP error). Under an
unchanged 30% failure rate, 20 clean runs would have probability ~0.08%. Evidence:
`evidence/postfix-firefox-run1..10/` and `evidence/postfix-chrome-run1..10/`; the pre-fix runs are
kept in `evidence/cna-web-webgl2-firefox{,-run1..3}/` and `evidence/cna-web-webgl2-qualified/`
(whose `web-gameplay.png` is the defective Chrome frame).

Native Release OPENGLES3 was rebuilt on the fixed renderer and re-captured
(`evidence/postfix-native-release/`): menu, instructions, full gameplay scene, drag, pause and a
clean exit through the menu (`exit=0`). `cnanext` regression: `CnaGraphicsTests` and
`CnaRendererTests` (333 tests) pass, `CnaRuntimeTests` fails only the known window-manager-less
Xvfb case, and the new `EasyGL_ThreadContextLease_Exclusion` test passes alongside the existing
`EasyGL_BackgroundContent_ContextOwnership`.

## Owner-approved deviation: mouse input

The owner asked on 2026-09-06 for this touch-only phone game to be playable with a mouse on
the desktop. The port enables `TouchPanel::setMouseTouchEmulationEnabledEXT(true)` -- one
CNAEXT-marked line in `CatapultGame`'s constructor, off by default in the framework, with no
second input path in any screen. See `diff.md` for what it does and what it deliberately does
not do. Verified natively (Debug and Release artifact trees) and in real Chrome, with the
original touch gate still passing on the same bundle.

## Result

Catapult Wars is restored from the complete endpoint at
`Source/EX2_PolishAndMenus/End/CatapultGame/CatapultGame` in the 225-file
`CatapultWars_4_0` training kit. The 21 original game units are represented by the same
logical decomposition under `src/`; the additional `.cpp` files only move non-template C++
implementations out of headers. No earlier reduced port, loose-content substitute or sample-side
framework workaround remains.

The unchanged C# endpoint compiles with the XNA 4.0 assemblies. Its resulting
`CatapultGame.dll` has SHA-256
`5e4afbaa36b86dddb70aca7262443ff1f4d40321feb5721534fd359093dcd517`.
The local environment has no Windows Phone 7 application host, so this is intentionally not
reported as a visual run of the phone XAP. A separately labelled diagnostic shim supplies only
the absent `Microsoft.Devices.Haptic.VibrateController` assembly while compiling the unchanged
sources; it is evidence, not port input.

## Original content

The unchanged Windows Phone/Reach content project builds through the official XNA 4.0 content
pipeline. The checked-in `Content/` tree is exactly that output: two Moire ExtraBold SpriteFont
XNBs, 25 texture XNBs, six SoundEffect XNBs and the unchanged
`Textures/Catapults/AnimationsDef.xml` copy item. `diff -qr` against the retained 34-file pipeline
output is empty. The old PNG/WAV/font-JSON substitutes are removed, and the game again loads all
compiled assets through `Content.Load<T>()`.

The upstream Word tutorial and Microsoft Permissive License are retained byte-identical. The
repository's historical `help.png` is preserved at the sample root as required by `rules.md`; it
is not content, copied to the runtime or displayed. The earlier authored `CatapultWars.htm` was
not part of the training kit and is removed.

## Source and behavior restored

The port retains the original 800x480 fullscreen presentation, 30 Hz fixed timestep, screen
manager and transition timings, Tap/FreeDrag/DragComplete gesture input, human and AI players,
catapult and projectile state machines, wind and damage rules, animation XML parsing through
`XDocument`, `GameComponent`-owned audio manager, vibration calls, background asset-loading
`Thread`, pause/menu/game-over flow, and isolated-storage screen persistence. The original
lower-case load of `Textures/HUD/arrow` is also retained; CNA's XNA-compatible content lookup
resolves it to `Arrow.xnb` without a Linux filename workaround.

`diff.md` records the small language/host adaptations that have no gameplay effect. There are no
known active behavioral differences from the source endpoint.

## CNA issue discovered and repaired

The real background loader exposed a renderer defect, not a reason to simplify the sample. A
worker-created EasyGL content context could leave its own context current during handoff and later
frames would use the wrong binding. `cnanext` commit `599d14e54` makes the renderer's scoped restore
clear its temporary current context when there was no distinct previous context, while continuing
to restore a genuinely different one. The actual-GL regression
`cna_test_easygl_background_content_context` loads a real XNB model on a worker and renders two
later owner-thread frames; `cna_c_api_game_secondary_graphics_context_smoke` continues to prove
secondary-device context restoration.

For browsers, CNA deliberately separates its exception ABI from Asyncify: the JavaScript-driven
`cna_c_api_wasm` library remains synchronous, while application executables that call blocking
`Game::Run()` opt into `CNA::EmscriptenAsyncify`. `cmake/SampleHelpers.cmake` now applies that
documented application contract to every Emscripten sample target. The final Catapult Wars link
therefore contains WebGL 2 MIN/MAX settings, pthread/offscreen framebuffer support and
`-sASYNCIFY=1`; this does not change the C-API Wasm artifact's RunOneFrame contract.

## Native OPENGLES3 qualification

Debug and Release configurations build with the exact content. The final Release run uses Mesa
OpenGL ES 3.2 and completes this path without a sample-side input implementation:

`main menu -> instructions/loading -> background XNB load -> gameplay -> FreeDrag ->
DragComplete/fire -> pause -> Quit Game -> main menu -> Exit`

The SDL qualification harness converts real host mouse events into SDL touch events below CNA and
injects Escape only for repeatable automation; it is retained under the external evidence root,
not shipped in the sample. The runtime exits successfully. Captures and console evidence are in
`/rv/tmp/samples/SAMPLE-067-CatapultWars_4_0/evidence/cna-native-opengles3-qualified/`.

## Real-browser WEBGL2 qualification

A clean threaded Emscripten build runs in real headless Chrome with cross-origin isolation and an
actual WebGL 2 context (`WebGL 2.0 (OpenGL ES 3.0 Chromium)`). Browser touch events traverse the
same menu, loading, gameplay, drag/fire, pause and return-to-menu path. The 600-frame canary
completes after gameplay interaction. The browser probe reports:

- `rafCount = 600`;
- zero uncaught exceptions and zero unhandled promise rejections;
- zero HTTP asset errors;
- successful load of every referenced XNB/XML asset;
- distinct menu, instructions, gameplay, post-drag, 600-frame, pause and returned-menu captures.

Evidence is in
`/rv/tmp/samples/SAMPLE-067-CatapultWars_4_0/evidence/cna-web-webgl2-qualified/`.
