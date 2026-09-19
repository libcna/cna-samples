# SAMPLE-016 audit — BounceSample_4_0

## Result

Requalified on 2026-09-19 against the exact XNA 4.0 Windows Phone 7 source.
The one newly found translation difference, `Math.Sin`/`Math.Cos` precision in
`SpherePrimitive`, is corrected. The previous desktop substitutions and
documented workarounds remain removed. No known active source, behavior,
content, native-renderer or browser-renderer gap remains.

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
- The Windows 7/VS2010 VM fallback was retried on 2026-08-31, when its saved
  `vboxuser` automatic-login credential was invalid. On 2026-09-19 the host
  additionally has no `/dev/vboxdrv`, and VirtualBox reports that it cannot
  start VMs. Shared historical environment evidence is at
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
- The sphere's sine and cosine calculations now retain the original
  C# `Math.Sin`/`Math.Cos` double-precision intermediate and cast the result to
  `float`, instead of using C++'s float overload.
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
- These CNA fixes are from the original SAMPLE-016 port. The 2026-09-19
  requalification required no CNA or sharp-runtime change.

## Fresh verification — 2026-09-19

- Both fresh, sample-only Release builds succeeded against sibling checkouts:
  `/rv/tmp/samples/SAMPLE-016-BounceSample_4_0/work-native-opengles3-20260919`
  and `work-web-webgl2-20260919`, each with `--parallel 4`. The web build has
  Emscripten threads disabled, no DWARF `debug_info`, no pthread/shared-memory
  runtime markers and no `.data` file (the original runtime Content is empty).
- The native binary rendered real 800×480 frames with OPENGLES3 on an isolated
  X11 display. Holding Up for two seconds visibly moved the spheres, and Escape
  ended the process with exit code 0. Evidence is in
  `evidence/cna-native-opengles3-20260919/`: `game.log`, both PNGs and
  `frame-sha256.txt`.
- System Google Chrome loaded the new bundle over local HTTP with WEBGL2.
  It drew changing frames and responded to held ArrowUp; all runtime requests
  returned HTTP 200, with zero fatal console messages, unhandled rejections,
  JavaScript exceptions or relevant HTTP errors. The browser's gesture-enabled
  fullscreen transition was also captured. Evidence is in
  `evidence/cna-web-webgl2-20260919/`, including `browser-result.json`.
- The gallery's HTML/JS/WASM files are byte-identical to the verified build.
  The exact site copy passed the same independent system-Chrome gate, recorded
  under `evidence/gallery-copy-web-20260919/`. Local HTTP returned 200 for the
  index, Bounce detail, Play bundle and both gallery images. The detail page
  was also visually inspected from a fresh Chrome capture at
  `evidence/gallery-bounce-detail-20260919.png`.
- The previous SAMPLE-016 graphics/platform suite passed 50 tests when the
  earlier CNA DirectionalLight and fullscreen fixes landed. No CNA API changed
  during this requalification, so that suite was not re-run.

The canonical retained products, new work trees, scripts, logs and captures are
all under `/rv/tmp/samples/SAMPLE-016-BounceSample_4_0`; see `MANIFEST.md`.
Neither fresh work tree was pruned.
