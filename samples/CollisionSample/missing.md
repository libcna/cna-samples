# SAMPLE-017 audit — CollisionSample_4_0

## Result

Requalified on 2026-09-19 from the exact upstream source, a new unchanged XNA
build, new Release OPENGLES3 and non-threaded Release WEBGL2 builds, and both
the canonical and gallery web copies. The original and C++ randomized suites
each pass 420,000 checks. Four C#-to-C++ arithmetic/default-value discrepancies
were corrected in the sample, not hidden in CNA. No known active source,
behavior, content, OPENGLES3 or WEBGL2 gap or workaround remains. CNA and
sharp-runtime needed no change in this pass.

## Fresh 2026-09-19 evidence

- The exact 28-file upstream `CollisionSample_4_0` directory is byte-identical
  to `xna4-original/`. Its two solutions contain a shared Windows/Phone game,
  one `Font.spritefont` content project and a separate console UnitTests
  project. The Windows Debug/x86/Reach configuration was rebuilt without source
  changes using `scripts/build-original.sh` into `work-xna4-20260919/` (since
  pruned after its outputs were verified byte-identical to `xna4-build/`); its
  original randomized program reports `Passed: 420000 Failed: 0` in
  `evidence/requal-20260919/xna-original/unit-tests.log`.
- The official XNA pipeline rebuilt `Font.xnb` at SHA-256
  `aad7c770f87443708af6bf7a0c6441d0fdf6a5ea2168e25c9fc16f656e2388ad`,
  byte-identical to the checked-in and native runtime copies. The complete
  WEBGL2 `.data` file is exactly those same 20,598 bytes. No loose font or
  hand-loaded replacement is used.
- The unchanged original ran under Wine/WineD3D on isolated Xvfb. Fresh captures
  show Sphere, Ray, Frustum, axis-aligned box, oriented box and orthographic
  oriented box; Escape exited cleanly. They are in
  `evidence/requal-20260919/xna-original/`.
- `CollisionSample.hpp` now keeps C# `Math.Sin`/`Math.Cos`/`Math.Exp`'s
  double-precision intermediate before the float cast, and the original
  camera-target interpolation operand order. `DebugDraw.hpp` now divides each
  grid axis by its division count, rather than multiplying by a rounded
  reciprocal, and uses double-precision trig before casting for ring steps.
  `BoundingOrientedBox` now has the zero quaternion produced by C#'s default
  value-type constructor rather than an invented identity quaternion; its
  C++ surface test checks this. These are faithful translation corrections,
  not runtime workarounds.
- The native Release OPENGLES3 game and `CollisionSampleUnitTests_cna_samples`
  were built in `work-native-opengles3-20260919/` with at most four compile
  jobs against `../cna` and `../sharp-runtime`. The C++ suite again reports
  `Passed: 420000 Failed: 0`. Isolated Xvfb captured all six views and a clean
  Escape exit; the stripped canonical product passed a further five-second
  smoke run. The preserved `WINDOWS_PHONE` branch passed a native syntax-only
  compile with `-DWINDOWS_PHONE`; a WP7 package was not built on this host.
- The non-threaded Release WEBGL2 bundle was built in
  `work-web-webgl2-20260919/` with at most four compile jobs. System Google
  Chrome on an isolated Xvfb display with software WebGL2 loaded it over HTTP
  at 853×480: six distinct views, stable paused scene after the original camera
  transition settles, visible single-step and arrow-key rotation, exact font,
  and no rejected promise, runtime exception, HTTP error or fatal console
  message. The byte-identical gallery copy passed the same gate. Results and
  captures are in `evidence/requal-20260919/{canonical-web,gallery-web}/`.
  The browser-gate script logs all delivered keys; isolation excludes unrelated
  desktop input. The bundle has no DWARF or pthread markers.
- The old canonical native product had an extra `Content/help.png`, identical
  to the retained sample-root historical image. That redundant runtime copy
  was removed; `help.png` remains beside `CMakeLists.txt`, is not packaged
  or drawn. `SetData` in `DebugDraw` is the original C# vertex/index streaming.
  No NOXNA, backend helper, loose-content substitute or invented overlay is
  present.

The exact source, updated canonical products, scripts and all evidence are under
`/rv/tmp/samples/SAMPLE-017-CollisionSample_4_0/`; see `MANIFEST.md` for the
four-job rebuild commands. At the owner's explicit request on 2026-09-19,
`tools/prune-completed-sample.sh --apply SAMPLE-017-CollisionSample_4_0`
removed the two fresh CMake work trees. The duplicate XNA work tree was also
removed after its `bin/` and `unit-tests/` outputs were verified byte-identical
to the retained `xna4-build/` outputs and its official pipeline `Font.xnb` was
preserved under `xna4-build/Content/`. The artifact root is now 17,753,100
bytes; its canonical C++ test executable still passes all 420,000 checks.
The validated `samples.libcna.com/CollisionSample.html` gallery commit was
pushed to `origin/main` at `ea9f3d7`; the sample commit was pushed to
`origin/develop` at `539a971`.

The sections below preserve details of the earlier audit; the fresh results
above supersede their build and capture timestamps.

## Original reference and environment

- The unchanged upstream snapshot is
  `/rv/tmp/samples/SAMPLE-017-CollisionSample_4_0/xna4-original`.
- Every file in the Windows and Phone projects, the shared game source,
  `BoundingOrientedBox`, `TriangleTest`, `GeomUtil`, `DebugDraw`,
  `FrameRateCounter`, the Content project and all five UnitTests source files was
  audited against the C++ port. Project manifests, icons and documentation were
  reviewed for target/build intent rather than turned into invented runtime code.
- The official XNA 4.0 pipeline built `Font.spritefont`, and the unchanged
  Windows game built and ran through Wine/WineD3D with the established
  `/home/robertvokac/.wine-cna-xna40` prefix.
- An isolated Xvfb run captured the initial Sphere view, all four G-key camera
  transitions (Ray, Frustum, axis-aligned box and oriented box), the B-key
  orthographic view and a clean Escape exit. Captures and logs are under
  `/rv/tmp/samples/SAMPLE-017-CollisionSample_4_0/evidence/xna-original`.
- The unchanged original UnitTests executable reports
  `Passed: 420000 Failed: 0`.

## Restored source, behavior and content

- The game class, initialization order, five primary shapes, four animated
  secondary shapes per group, collision result colors, ray hit marker, camera
  interpolation, pause/single-step behavior and draw order follow the original.
- Keyboard, gamepad and gesture paths are restored, including G/A camera cycling,
  Home/Y reset, B/B projection toggle, O/P projection selection, Space/X pause,
  arrow rotation, +/- zoom, Tap, FreeDrag and Pinch. Escape is the normal CNA
  desktop/web counterpart to exiting the running sample.
- The inactive `WINDOWS_PHONE` branch remains in source with the original
  fullscreen 30 Hz setup; the audited reference builds use the original Windows
  853x480 setup.
- `FrameRateCounter` again owns its original service-based `ContentManager`,
  loads `Font`, creates a `SpriteBatch` and draws white `fps: N` text at `(32,32)`.
- `BoundingOrientedBox` again exposes the original hash, string and both corner
  APIs. The original `TriangleTest` oriented-box overloads are methods of
  `TriangleTest`, not sample-specific free-function substitutes.
- `DebugDraw` again implements `IDisposable`; its dynamic `SetData` calls are the
  original debug renderer's per-frame vertex/index streaming, not content
  substitution.
- The official `Font.xnb` is used unchanged. Its SHA-256 is
  `aad7c770f87443708af6bf7a0c6441d0fdf6a5ea2168e25c9fc16f656e2388ad`
  in the XNA build, the repository and the native CNA build.
- The original `CollisionSample.htm` remains byte-identical. Historical
  `help.png` is retained only beside this sample's `CMakeLists.txt`; it is not
  packaged, loaded or drawn.

## Framework fix exposed by this sample

- XNA/FNA's `ContentManager(IServiceProvider)` starts with an empty
  `RootDirectory`. CNA incorrectly inherited its convenience constructor's
  `"Content"` default, so the faithful `FrameRateCounter` searched
  `Content/Content/Font.xnb`.
- CNA now restores the empty XNA root for the service-provider constructor while
  its explicitly non-XNA zero-argument convenience constructor keeps the
  `"Content"` default. Two focused constructor regressions pass.
- No sharp-runtimenext change was required.

## Verification

- Native Release OPENGLES3 build:
  `/rv/tmp/samples/SAMPLE-017-CollisionSample_4_0/cna-native-opengles3`
- A real-display native smoke run remained active and error-free for five
  seconds. An isolated Xvfb visual run captured all six views and verified G, B
  and Escape. The log identifies OpenGL ES 3.2 and the OPENGLES3 renderer; no
  fatal error or exception occurred.
- The C++ port of the original randomized suite reports
  `Passed: 420000 Failed: 0`; additional surface checks cover the restored hash,
  string and offset-corner APIs without changing the original pass count.
- Web Release WEBGL2 build:
  `/rv/tmp/samples/SAMPLE-017-CollisionSample_4_0/cna-web-webgl2`
- System Google Chrome loaded the build over local HTTP. The canvas is 853x480,
  all HTML/JS/WASM/data requests returned HTTP 200, WebGL2 and CNA's WEBGL2
  renderer were confirmed, the official font loaded, six controlled captures
  were distinct, and no HTTP error, rejection, runtime exception or fatal console
  message occurred.
- Side-by-side review of original XNA, native CNA and browser CNA captures
  confirms the same CornflowerBlue background, black grids, five camera groups,
  white primary geometry, LightGray/Yellow/Red collision results, animated
  secondary geometry and FPS placement. Moving shapes differ between captures
  only because the original time-based animation continues while each view is
  recorded.
- Browser result:
  `/rv/tmp/samples/SAMPLE-017-CollisionSample_4_0/evidence/cna-web-webgl2/browser-result.json`
- CNA constructor tests: 2/2 passed. The modular build has no aggregate `CNA`
  target; the affected `cna_content` target and `CnaTests` executable built
  successfully.

All reusable builds, exact original source, scripts, logs, test outputs and
captures are under `/rv/tmp/samples/SAMPLE-017-CollisionSample_4_0`.
