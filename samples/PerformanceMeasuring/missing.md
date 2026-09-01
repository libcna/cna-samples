# SAMPLE-081 — Performance Measuring audit

**Status: complete — no known behavior or content differences from the selected XNA 4.0 Windows original.**

The historical port was not an acceptable endpoint. It omitted `RemoteDebugCommand` and
`StringBuilderExtensions`, replaced the lit position/normal sphere geometry with flat-colored
vertices, substituted loose sidecar content for the ground, checker texture and font, added an F1
overlay that does not exist upstream, renamed original members, and compensated for a historical
viewport issue inside the sample. Those workarounds and omissions are removed.

Artifact root:
`/rv/tmp/samples/SAMPLE-081-PerformanceMeasuringSample_4_0/`

## Original surface audited

The exact 33-file `PerformanceMeasuringSample_4_0` source snapshot is retained. The selected
runnable product is `PerformanceMeasuring (Windows).csproj`, Windows/Reach. Its Debug constants are
`DEBUG;TRACE;WINDOWS`; Release retains `TRACE;WINDOWS`, so the timing instrumentation is present in
both selected configurations. The inactive Windows Phone timing/fullscreen and touch branches are
also retained in the translation. The Xbox solution/project and all project/content declarations
were inspected rather than treated as authority for the desktop product.

All 17 C# units were reviewed and represented: `PerformanceMeasuringGame`, `Sphere`, the three
primitive files, all ten GameDebugTools units, `Program` and assembly metadata. The port preserves
the original initial pool of 50 active spheres out of 200, time-seeded placement and velocity,
world bounds, collision resolution and draw/update ordering. `X` toggles collision handling,
Up/Down change the active count once per update, Back/Escape exits, and the original Tap/FreeDrag
touch behavior remains.

The profiling surface is complete: FPS accumulation, the two-buffer/nested-marker TimeRuler,
sampling and log calculations, adaptive ruler scale, debug console editing/history/layout, all
`fps`, `tr`, `cls`, `echo`, `help` commands, culture-aware manual number formatting, and the
Windows SystemLink `remote` command state machine and packet protocol. Original misspellings that
are observable names or messages are retained, including `IDebugEchoListner`, `ConnectionPahse`,
`AppendNumbernternal` and the exception/console text.

Two lossless C++ representation details replace C# runtime mechanics. Component references are
owned outside CNA's non-owning `GameComponentCollection`, and the custom vertex supplies its
declaration explicitly instead of deriving CNA's virtual C++ `IVertexType`; that keeps the actual
GPU stream at the original 24-byte `Vector3 + Vector3` layout rather than inserting a vtable
pointer. Neither changes sample behavior or content.

`PerformanceMeasuring.htm` is byte-identical to upstream. The historical `help.png` is retained
beside `CMakeLists.txt` only; it is not packaged, loaded or displayed.

## Authentic content

The unchanged Microsoft XNA Game Studio 4.0 pipeline built the Windows/Reach content. The checked-in
`Content/` now contains only these exact outputs, byte-identical to `xna4-build/windows-reach/Content/`:

| File | Bytes | SHA-256 |
|---|---:|---|
| `Checker_0.xnb` | 11,151 | `e76157fed0e09dfa5e9c646f49adad29d611d5985cb62699a8c18a799a8efb9d` |
| `Font.xnb` | 21,678 | `4eefa823f31e700ce41ade8fc31ee4cb733bfa568eab286a75f6a904fa97a0e3` |
| `Ground.xnb` | 1,302 | `16b581e56da4a18c8f7eda8b50e430bcc06ef8cd500e1546c7473f925afc4e6d` |

`Ground.xnb` preserves the original model, material, UVs and external `Checker_0` texture reference;
the checker therefore repeats through `SamplerState::LinearWrap` exactly as authored. The original
Segoe UI Mono sprite font is preserved rather than rebuilt from a substitute font. The former
`.model.json`, vertex/index binaries, loose checker/font PNGs and font JSON are gone.

## Qualification

All CNA builds used `CCACHE_DIR=/rv/cnaccache` and at most eight parallel jobs.

- The unchanged 17 C# units compiled against the installed XNA 4.0 assemblies with the selected
  Windows constants. The real XNA executable ran through WineD3D at 800x480 and loaded the official
  outputs. Captures prove lit spheres, repeating checker ground, FPS/TimeRuler UI, count 50 and
  collisions `True`; holding Up and pressing X produced count 75 and collisions `False` before
  Escape completed the run.
- Debug and Release OPENGLES3 targets both built. Debug ran on a real Mesa OpenGL ES 3.2 context,
  loaded all three XNB assets and reproduced the same scene and UI. Up/X changed the state, Tab
  opened the original console, and `remote` executed the real networking route through the honest
  no-session result (`Finding available sessions...`, `Couldn't find a session.`). Closing the
  console and Escape completed teardown. The desktop compositor's default XWayland-vsync path
  throttled one diagnostic run to 0.99 FPS; with vblank override the same binary reported
  59.31–59.76 FPS, so this was host presentation policy rather than sample CPU or storage load.
- CNA's real EasyGL regression suite passed all 83 declared SpriteBatch/3D ordering tests (three
  declared skips), independently confirming the depth-state transition used by the scene and UI.
- A clean Release WEBGL2 bundle built and ran in system Google Chrome. The final link requested
  WebGL 2 exactly, Chrome returned `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, and the 800x480 scene used
  the authentic XNBs. CDP held Up across updates (count 50 to 81), then held X across updates
  (collisions `True` to `False`). The probe completed a 60-frame motion canary and another 600
  callbacks (763 total), with changing rendered frames and no runtime exception, unhandled
  rejection, fatal console message or relevant HTTP error.
- The native and browser captures reproduce the reference's scene structure, lighting, material,
  text, profiling panels and representative state transitions. Random sphere state intentionally
  prevents whole-frame pixel identity.
- No CNA or Sharp Runtime source change was required for this sample. The only build-discovered
  source correction was moving the definition of `DebugSystem`'s static `unique_ptr` after the
  class is complete, which makes the same ownership compile under both libstdc++ and Emscripten's
  libc++.
- The subsequent SAMPLE-104 audit reproduced a deterministic clean-exit crash in the shared
  GameDebugTools translation: its static C++ owner destroyed `Texture2D` and `SpriteBatch`
  resources only after the stack-owned `Game` and graphics device were already gone. `Shutdown`
  now removes the five components and three services while the game is alive, then releases their
  C++ ownership from the derived game destructor. The OPENGLES3 target was rebuilt and an isolated
  Xvfb start/render/Escape run exited with status zero; no original update/draw behavior changed.

## Retained evidence

- Exact source snapshot and hashes: `xna4-original/`, `original-manifest.txt`,
  `original-sha256.txt`
- Official content and unchanged reference executable: `xna4-build/windows-reach/`
- Original run: `evidence/original-baseline.png`, `evidence/original-interacted.png`
- Native run and console branch: `evidence/cna-native-baseline.png`,
  `evidence/cna-native-interacted.png`, `evidence/cna-native-debug-console.png`
- Reusable Debug and Release trees: `cna-native-opengles3/`,
  `cna-native-opengles3-release/`
- Real-browser JSON/logs and five captures: `evidence/cna-web-webgl2-qualified/`
- Reusable complete WebGL2 bundle: `cna-web-webgl2/`
- Reproducible WebGL2 build, server and browser probe: `scripts/`
