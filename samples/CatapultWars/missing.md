# SAMPLE-067 — Catapult Wars audit and qualification

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
