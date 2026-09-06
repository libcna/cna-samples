# SAMPLE-067 — Catapult Wars audit and qualification

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

## Open defect: assets loaded on the background thread are intermittently empty

Found on 2026-09-06 immediately after the abort above stopped hiding it -- in Firefox the game had
never reached this screen before, so this is newly *visible*, not newly *introduced*.

**What is seen.** On the gameplay screen, a subset of the textures `GameplayScreen::LoadAssets`
loads on the background thread renders as pure black. Measured over the sampled patches
`sky`, `mountain` and the `HUDFont` "WIND" caption:

| Browser | runs | defective |
|---|---|---|
| Firefox 140 ESR | 5 | 2 |
| Chrome (headless, SwiftShader) | 5 | 1 |

It is not browser-specific and not deterministic. The affected set varies between runs: one
Firefox run lost `sky`, `mountain` and `HUDFont` while every other asset drew; the Chrome touch
run lost `gameplay_screen`, both clouds, `sky`, `mountain` and `HUDFont` but kept the HUD frames,
both catapults and the menu font. Nothing is reported anywhere -- all 33 `Loading asset` lines
appear in a defective run exactly as in a clean one, no GL error, no exception, no rejection,
and the functional gate passes. Only the pixels are wrong.

**Why.** The renderer serializes a background content load against the drawing thread with
`IGraphicsRenderer::AcquireThreadContextLeaseEXT`, which `GraphicsDevice` takes around `Present()`
and every bounded operation. `EasyGLRenderer::AcquireThreadContextLeaseEXT`
(`modules/renderers/easygl/src/EasyGLRenderer.cpp`) takes `threadContextMutex_` and makes the
context current for the calling thread -- but only natively: its whole body is `#if
defined(__EMSCRIPTEN__) return nullptr; #else … #endif`, added by `cnanext 71576a7b9`
("fix(SAMPLE-061): support threaded graphics content loading", 2026-08-31). On the web there is
therefore no mutual exclusion at all, while `-sOFFSCREEN_FRAMEBUFFER=1` proxies **each individual
GL call** to the browser thread, where both threads share one WebGL context. A loader's
`glBindTexture` / `glTexImage2D` pair can thus be split by the render thread's own binds, and the
upload lands somewhere else -- which is exactly the shape of the damage: whole textures empty,
a different subset each run, no error anywhere.

This is a `cnanext` defect, not a sample one, and it affects every sample that loads content on a
background thread on the web (SAMPLE-061 Marble Maze is the other known one). It is recorded here
rather than worked around, and it is not fixed yet: the fix is a renderer-architecture change --
give the Emscripten path a real lease that serializes the loader's GL against the frame -- and
awaits the owner's decision.

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
