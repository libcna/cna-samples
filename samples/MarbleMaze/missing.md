# MarbleMaze — XNA 4.0 fidelity audit

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
- the original background `System.Threading.Thread` content load;
- the original accelerometer wrapper, real-device calibration branch, emulator arrow-key branch
  and `Microsoft.Devices.Environment.DeviceType` selection;
- asynchronous `Guide.BeginShowKeyboardInput` high-score naming;
- the stock `Model`/`ModelMesh`/`BasicEffect` draw path, original `Model.Tag` collision triangles,
  bones/checkpoints, marble physics, camera, countdown, win/fall/pause behavior and audio state.

No synchronous-load substitute, STL file persistence, fixed player name, omitted phone branch,
invented keyboard path, reduced screen set, fake content or renderer helper remains in sample code.

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
- the real system Chrome reports OpenGL ES 3.0 / WebGL 2.0, loads the same XNB model/resource path
  from the background thread and renders gameplay and pause;
- the browser gate recorded four touch events, `crossOriginIsolated=true`, an 800x480 canvas, no
  uncaught exception, no unhandled promise rejection and no failed HTTP response.

Evidence: `evidence/cna-web-webgl2-clean-final/`.

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

## Reproduction artifacts

```text
/rv/tmp/samples/SAMPLE-061-MarbleMaze_4_0/
  xna4-original/                 exact complete upstream snapshot
  xna4-build/                    official XNA content and diagnostic host
  cna-native-opengles3/          reusable native build and MarbleMaze product
  cna-web-webgl2/                reusable threaded browser build and full bundle
  scripts/                       original/native/browser reproduction helpers
  evidence/                      logs, results and screenshots cited above
```
