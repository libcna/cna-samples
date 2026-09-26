# MarbleMaze — XNA 4.0 fidelity audit

## Current-head requalification — 2026-09-26

**Status: COMPLETE for the final `EX2_Polishing/End` tutorial endpoint.** The
owner explicitly chose this endpoint alone on 2026-09-26; the other seven
independently runnable Begin/End stages remain unchanged in the exact upstream
archive and are outside this CNA port scope (`SAMPLES-DEC-011` in `plan.md`).
The current chain is CNA `next` `cefe6c83b` and SharpRuntime `next` `41b918c9`.
No dependency source change was required. The previously approved, off-by-default
mouse-to-touch opt-in remains the only user-visible deviation (`diff.md`).

### Original and content reference

The whole physical `MarbleMaze_4_0` upstream is byte-identical to
`/rv/tmp/samples/SAMPLE-061-MarbleMaze_4_0/xna4-original/`. The selected final
Phone/Reach game has 24 game C# files plus the two-file custom processor.
`bash scripts/build-original.sh` rebuilt the original processor and both official
Phone/Reach and Windows/Reach content sets. All **26** checked-in XNBs match the
new Phone output and current native product byte for byte; the source still
loads the original `Model` and its typed `Model.Tag` collision data.

The unchanged Phone application cannot execute as a desktop XNA program. The
retained `bin-diagnostic/MarbleMazeGame.exe` links unchanged game sources with an
external `Microsoft.Devices` shim. The original-source diagnostic build reached
`Game.Run()` under Wine, then threw `IsolatedStorageException`: a plain desktop
EXE has no application identity for `GetUserStoreForApplication()`. A separate
`bin-diagnostic-playable/` build copies four C# files **only for this Wine host**:
two use assembly-scoped isolated storage, one maps mouse release to a Phone tap,
and one enables the existing keyboard menu actions. The original Phone snapshot
and CNA port sources are unchanged. This host rendered the full title menu under
WineD3D with `WINEDLLOVERRIDES=d3d9=b`; the owner confirmed mouse and keyboard
input worked and closed it. `scripts/{build,run}-desktop-diagnostic-playable.sh`
reproduce it, and `evidence/requal-20260926/xna-diagnostic-playable/menu.png`
records the visible result. It remains an **adapted desktop diagnostic**, not the
Phone original or a basis for exact visual parity with the Phone device.
The 101-page tutorial and Microsoft Permissive License accompany the unchanged
archive and are now also retained in the sample source package. The original
Phone background, icon, thumbnail, splash and two manifests were restored.

### Browser loading workaround removed

The old Emscripten-only `GameplayScreen::LoadAssets()` call inside
`LoadingAndInstructionScreen::HandleInput()` ran synchronously and hid the
original loading screen while blocking the browser game loop. It was a
sample-local workaround for a historical Firefox WebGL worker stall, not a
faithful translation. The branch and its `assetsLoaded_` flag were removed.
Both native and web now create `System::Threading::Thread`, start the same
background loader and poll `ThreadState::Stopped` in `Update()`, exactly as
the selected C# source does. Current CNA EasyGL's general Emscripten context
lease/proxy path succeeds: a fresh threaded Release WEBGL2 build reached the
3D game and pause in system Chrome, and Firefox ESR 140.15.0 reached the 3D
game by real pointer clicks. The earlier workaround is **unnecessary on these
current heads**. The game source contains no replacement WebGL branch.

### Current native and web gates

- Current Release OPENGLES3 on Mesa ES 3.2, using private Xvfb and no SDL
  mouse-to-touch preload, passed menu, instructions, background model load,
  gameplay, sustained arrow-key tilt, pause, Quit Game and normal Exit.
  `evidence/requal-20260926/cna-native-opengles3-audio-gate/` contains the
  captures, clean exit and 33.81 seconds of actual PCM16 stereo 44.1 kHz audio
  (mean −30.7 dB, peak −3.7 dB). The test moves only this game's PulseAudio
  stream to its isolated sink. Earlier silent recordings had captured a sink
  the SDL stream was not using.
- Current Release WEBGL2 uses pthreads because the unchanged game needs
  `System.Threading.Thread`. Under a COOP/COEP test server, system Chrome
  passed menu, instructions, rendered maze and pause with no uncaught
  exception, rejection or HTTP error; Firefox rendered gameplay through the
  same background-thread path. The four original effects load through CNA's
  normal XNB audio path.
- The GitHub Pages gallery cannot set COOP/COEP as server headers. A scoped
  `coi-sw.js` service worker and `launch.html` were added **to the web product,
  not the game source**. They provide these headers for the game's same-origin
  files while keeping the Emscripten shell unchanged. A clean Chrome profile
  on plain HTTP passed with `crossOriginIsolated=true` and rendered gameplay
  and pause. The exact six-file gallery copy passed the same plain-HTTP gate
  from `/MarbleMaze/`, including 50.16 seconds of PCM16 stereo 44.1 kHz audio
  (mean −32.4 dB, peak −3.3 dB). Evidence is under
  `evidence/requal-20260926/{cna-web-webgl2-static2,gallery-exact,gallery-audio}/`.
  The 60th gallery card, detail, active-gameplay screenshot, navigation and local
  file routes were checked. The published URL is not yet tested remotely.

`/rv/tmp/samples/SAMPLE-061-MarbleMaze_4_0/MANIFEST.md` and retained scripts
contain the current original, native and threaded-web rebuild/capture commands.
The native RUNPATH now names the active `libcna/cna` checkout. All retained
new WEBGL2 files are Release-size without DWARF debug sections. The targeted
no-workaround scan finds only the documented AOT/runtime identity mechanics
and the owner-approved input opt-in; no sample-local WebGL load bypass remains.
The prune dry run proposes 27 intermediate paths, approximately 148.3 MB;
no `--apply` was run. Its output is `evidence/requal-20260926/prune-dry-run.txt`.

## Historical audit — 2026-09-05

**Status: COMPLETE (2026-09-05).** This port targets the final tutorial endpoint,
`Source/EX2_Polishing/End`, not one of the incremental `EX1` or `Begin` projects. The complete
selected source consists of 24 game `.cs` files and the two-file `MarbleMazePipeline` project; all
26 files, both project files, every content declaration and the phone-specific branches were
reviewed against the C++ port. The upstream directory has no HTML sample page; its documentation is
the retained `3D Game Development With XNA.doc` tutorial.

## Original build and reference boundary

The exact upstream directory is retained at:

```text
/rv/tmp/samples/SAMPLE-061-MarbleMaze_4_0/xna4-original/
```

`scripts/build-original.sh` builds the unchanged custom `MarbleMazeProcessor` and runs the official
XNA 4.0 content pipeline twice. It produced 26 Windows/Reach XNBs and 26 Windows Phone/Reach XNBs.
It also proves that all 24 unchanged game sources compile by linking a separately labelled desktop
diagnostic host with a minimal `Microsoft.Devices` host shim.

That diagnostic is not represented as the original Windows Phone application. Under Wine it gets
as far as constructing the game, selecting the presentation and entering `Game.Run()`, then the
XNA/D3D host never creates a window. The Windows 7 VM fallback was retried on 2026-08-31:
VirtualBox and the guest boot, but its saved `vboxuser` automatic-login credential is invalid, so
the VM is safely saved pending owner login. Shared environment evidence is retained at
`/rv/tmp/samples/SAMPLES-DEC-007-Win7-SongProcessor/`. The original retained evidence is
`evidence/xna4-diagnostic-final/console.log`; no visual claim is inferred from it. Original behavior
was therefore established from the complete unchanged source, official pipeline output, tutorial
and assets, while native/browser execution validates the translation.

## Content provenance

The port checks in exactly the 26 XNBs from the freshly generated Windows Phone/Reach output.
`cmp` verified every file byte-for-byte against `xna4-build/Content-phone` (`verified=26`):

- `Models/maze1.xnb` is the real output of the sample's `MarbleMazeProcessor`, including the stock
  renderable `Model`, marker bones and the original `Model.Tag`
  `Dictionary<string,List<Vector3>>` collision payload;
- `Models/marble.xnb`, the sprite font, title/instruction/background textures and all four sound
  effects travel through their normal XNA content readers;
- the processor-generated dependent textures and mip payloads remain exactly as authored.

The previous loose PNG/WAV/font files, JSON/raw model metadata and vertex/index sidecars were
removed. `RawMesh.hpp`, runtime collision reconstruction, hard-coded marker transforms,
sample-local culling changes and the unused F1 help load are gone. Historical `help.png` is retained
only beside `CMakeLists.txt`, as required by repository policy; it is neither content nor runtime UI.

## Ported behavior

The source decomposition, `MarbleMazeGame` and `GameStateManagement` namespaces, 30 Hz game timing,
landscape/fullscreen settings and screen stack match the selected original. The port includes:

- background/main/high-score/loading/instruction/gameplay/pause/calibration screens and their
  transitions, serialization and isolated-storage high scores;
- the original background `System.Threading.Thread` content load on native targets, with the
  documented WebGL context-thread adaptation on Emscripten;
- the original accelerometer wrapper, real-device calibration branch, emulator arrow-key branch
  and `Microsoft.Devices.Environment.DeviceType` selection;
- asynchronous `Guide.BeginShowKeyboardInput` high-score naming;
- the stock `Model`/`ModelMesh`/`BasicEffect` draw path, original `Model.Tag` collision triangles,
  bones/checkpoints, marble physics, camera, countdown, win/fall/pause behavior and audio state.

No unconditional synchronous-load substitute, STL file persistence, fixed player name, omitted
phone branch, invented keyboard path, reduced screen set, fake content or renderer helper remains
in sample code.

## Required runtime fixes

The faithful port exposed general runtime defects; they were repaired in their owning repositories
and regression-tested independently:

- `cnanext 17b5a90a0` loads the original typed dictionary stored in `Model.Tag` without changing
  its serialized types or data;
- `cnanext 71576a7b9` gives native EasyGL a bounded renderer-context lease for a whole background
  content decode/frame, initializes thread-local GL dispatch correctly, adds threaded Emscripten
  wiring, and adds the real `EasyGL_BackgroundContent_ContextOwnership` regression;
- `sharp-runtimenext 4a49afb0` provides opt-in Emscripten pthread support for the existing
  `System.Threading.Thread` API;
- `meta-gl eaf0788` makes native GL version parsing safe when a new thread has not initialized its
  dispatch table yet.
- `cnanext cb2c90208` classifies desktop/browser hosts as the Phone emulator input path, retains
  physical-device behavior for Android/iOS, and enables WasmFS so Firefox does not proxy every
  worker filesystem operation through the browser main thread.

These are framework/runtime fixes, not MarbleMaze checks or sample-specific bypasses.

## Qualification

Native OPENGLES3:

- `MarbleMaze_cna_samples` builds from the retained CMake tree;
- `scripts/capture-native.sh` completed with exit 0 on Mesa OpenGL ES 3.2;
- the evidence records main menu -> instructions -> actual background XNB model load -> rendered
  3D gameplay -> sustained tilt input -> pause -> Quit Game -> Exit;
- all models, dependent textures, font and four sounds appear in the content-load log. Xvfb's
  expected inability to switch to physical fullscreen falls back to a usable 800x480 window.

Evidence: `evidence/cna-native-opengles3-final/`.

WEBGL2:

- the complete threaded bundle builds with `CNA_GRAPHICS_RENDERER=WEBGL2` and
  `CNA_ENABLE_EMSCRIPTEN_THREADS=ON`;
- the earlier real-system Chrome gate reports OpenGL ES 3.0 / WebGL 2.0 and renders gameplay and
  pause through the same XNB model/resource path;
- Firefox 140.10.1 ESR was requalified after the context-thread adaptation: automated pointer
  input reached the rendered 3D maze and marble on an 800x480 canvas with no uncaught exception or
  unhandled promise rejection.

Evidence: `evidence/cna-web-webgl2-clean-final/` and
`evidence/cna-web-webgl2-firefox-final/gameplay.png`.

Focused CNA qualification on a real X11/Mesa context passed 47 content tests, 14 runtime tests, 44
EasyGL renderer tests (one capability skip) and the new end-to-end background-XNB regression. The
affected Debug targets rebuilt successfully with two parallel jobs, below the owner's eight-core
ceiling.

## Intentional language adaptations

There is no active port gap. The closed-world C++ equivalents for C# reflection/runtime identity,
the owner-approved off-by-default mouse-to-touch opt-in, and the Emscripten WebGL loading-thread
adaptation are documented in [`diff.md`](diff.md). The input opt-in preserves the original
touch/gesture path rather than adding a parallel control implementation. Native OPENGLES3 and
Firefox WEBGL2 builds pass; the project owner confirmed the native arrow-key controls.

## Pruned artifact inventory

The artifact root was pruned on 2026-09-05 from 3.3 GB to 281.7 MB; after retaining the final
Firefox screenshot it occupies 282.0 MB. The canonical retained runtime products are the corrected
post-change builds, not the superseded pre-pointer or Firefox-blocked variants:

- `cna-native-opengles3/samples/MarbleMaze/`: stripped native executable plus all 26 runtime
  content files;
- `cna-web-webgl2/samples/MarbleMaze/`: the verified HTML, JavaScript, Wasm and data bundle;
- `xna4-original/`, `xna4-build/`, `scripts/` and `evidence/`: the upstream snapshot, official XNA
  products, reproducible helpers and qualification record.

Before pruning, SHA-256 comparison proved that both canonical product directories contained the
freshly tested outputs from the `*-mouse-touch` build variants. The generic pruning policy retained
the two local `fna3d-source-*-mouse-touch` snapshots because they are unrecognised artifacts rather
than disposable CMake intermediates. The removed 3.0 GB comprises superseded build variants and
reproducible CNA/CMake intermediates. `MANIFEST.md` in the artifact root records the restoration
commands.
