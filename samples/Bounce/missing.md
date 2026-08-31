# SAMPLE-016 audit — BounceSample_4_0

## Result

The CNA sample is a faithful C++ port of the XNA 4.0 Windows Phone 7 source. The
previous desktop substitutions and documented workarounds have been removed. No
known active source, behavior, content, native-renderer or browser-renderer gap
remains.

## Original reference and environment

- The exact upstream snapshot is
  `/rv/tmp/samples/SAMPLE-016-BounceSample_4_0/xna4-original`.
- The original contains only the Windows Phone 7 `Bounce (Phone).sln`. It has no
  Windows desktop executable that can be run with Wine.
- An unchanged Release build was attempted and recorded in
  `/rv/tmp/samples/SAMPLE-016-BounceSample_4_0/evidence/xna4-phone-xbuild.log`.
  This host cannot build it because the XNA content targets, XNA Game Studio
  targets and Windows Phone 7 project support are unavailable; the solution also
  has no `Release|Windows Phone` mapping for the content project.
- The Windows 7/VS2010 VM fallback was retried on 2026-08-31. VirtualBox and the
  guest now boot, but its saved `vboxuser` automatic-login credential is invalid;
  the VM is safely saved pending owner login. Shared environment evidence is at
  `/rv/tmp/samples/SAMPLES-DEC-007-Win7-SongProcessor/`.
- Comparison therefore used a line-by-line audit of every C# source file and
  project setting, the supplied sample documentation, and the original
  `Background.png`. That PNG is original visual-reference material, not a runtime
  capture.

## Restored source and behavior

- The original `Game1`, `Accelerometer`, `Sphere`, `SpherePrimitive`,
  `GeometricPrimitive` and custom `VertexPositionNormal` structure are represented
  directly in C++.
- The custom vertex has the original 24-byte Position/Normal layout. Its runtime
  `VertexBuffer::SetData` and `IndexBuffer::SetData` calls are faithful translations
  of the procedural geometry code, not content substitutes.
- The game creates the original 100 randomly colored spheres, CornflowerBlue
  background, lit sphere geometry and flattened black sphere shadows.
- Fullscreen and `TimeSpan::FromTicks(333333)` (30 Hz) are restored.
- Collision, gravity, orientation compensation, shake detection, floor response,
  camera rotation and drawing order follow the original statement-for-statement.
  This includes the original shake-speed assignment where `Min(speed, 4)` is
  immediately overwritten by `Max(speed, 2)`; the port does not silently correct
  that observable bug.
- Real accelerometer input is used when the CNA platform reports a supported
  sensor. Otherwise arrow keys reproduce the XNA Windows Phone emulator branch:
  each frame starts at `(0, 0, -1)`, held arrows add exactly one unit on X/Y, the
  vector is normalized, and release immediately returns to level.
- Escape retains CNA's normal desktop/web exit mapping alongside the original
  phone Back-button behavior. No invented HUD, help screen or persistent desktop
  tilt state remains.
- The original runtime Content project is empty. Historical `help.png` is retained
  only beside this sample's `CMakeLists.txt`; it is not packaged, loaded or drawn.

## Framework fixes exposed by this sample

- CNA `DirectionalLight` now uses the constructor defaults found in the shipped
  Microsoft XNA 4.0 reference assembly: direction Down, diffuse color One,
  specular color Zero, disabled. This differs from FNA's zero-initialized backing
  fields and removes the old sample-local diffuse-light assignment.
- CNA's SDL/Emscripten window layer maps XNA's fullscreen request to browser
  fullscreen, whose transition SDL defers until a browser user gesture. Native
  exclusive-fullscreen behavior is unchanged.
- No sharp-runtimenext change was required.

## Verification

- Native Release build:
  `/rv/tmp/samples/SAMPLE-016-BounceSample_4_0/cna-native-opengles3`
- Native smoke log:
  `/rv/tmp/samples/SAMPLE-016-BounceSample_4_0/evidence/cna-native-opengles3.log`
  confirms OPENGLES3 and a stable eight-second run; an interactive run also exited
  cleanly through Escape.
- Web Release build:
  `/rv/tmp/samples/SAMPLE-016-BounceSample_4_0/cna-web-webgl2`
- System Chrome loaded the build over local HTTP with WEBGL2. Every requested
  runtime file returned HTTP 200, no fatal console error, rejection or runtime
  exception occurred, animation changed frames, and ArrowUp changed the simulated
  tilt while fulfilling deferred browser fullscreen.
- Browser result:
  `/rv/tmp/samples/SAMPLE-016-BounceSample_4_0/evidence/cna-web-webgl2/browser-result.json`
- Captured frame SHA-256 values were
  `0aae4ea06215c689201b2c07dba7e3141b04b587558e7bcfc25243c171e5879f`,
  `fc81df8cc629582427c9c8529377dd53598733a98507a63bff0cb0294549b620`
  and `4ce278260111648969da2e29d5fde63136111c69fa8ef1760462007e5cf1c957`.
- The focused CNA graphics/platform suite passed all 50 tests, including the new
  DirectionalLight defaults and native SDL fullscreen regression coverage.

All reusable builds, scripts, logs and captures are under
`/rv/tmp/samples/SAMPLE-016-BounceSample_4_0`.
