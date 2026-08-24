# SAMPLE-017 audit — CollisionSample_4_0

## Result

The CNA sample is a faithful C++ port of the complete XNA 4.0 Windows game and
its reusable collision code. The original 420,000-case randomized test program
is also ported and passes. The stale omissions and sample workarounds have been
removed; no known active source, behavior, content, OPENGLES3 or WEBGL2 gap
remains.

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
