# SAMPLE-009 audit — InputReporter_4_0

No known behavioral or visual differences remain after the `SAMPLE-009` audit. There is no
unresolved CNA or sharp-runtime implementation gap for this sample.

## Reference and source audit

- The unchanged XNA 4.0 source snapshot is retained under
  `/rv/tmp/samples/SAMPLE-009-InputReporter_4_0/xna4-original/InputReporter`.
- `InputReporterGame.cs`, all three `ChargeSwitch` files, the generated resources, `.resx`,
  Content project and Windows/Xbox game projects were reviewed line by line. The Windows project
  is the runnable reference.
- The original executable and all 15 assets were built with the real XNA 4.0 assemblies and
  official Content Pipeline inside the isolated Wine prefix. The pipeline used the original Arial
  and Arial Bold declarations after installing Microsoft's core fonts into that prefix.
- The 15 checked-in XNBs are byte-identical copies of those official pipeline outputs: six
  `SpriteFont` assets, the background and eight controller-state textures. The former DejaVu
  JSON/PNG sidecars and loose JPG/PNG runtime substitutes were removed.

The C++ translation preserves the original 853x480 layout, positions, colors, resource strings,
draw order, controller selection, capability branches, packet number, dead-zone modes and
two-second charge-switch behavior. `ChargeSwitch::Fire` is again a multicast zero-argument event,
and a negative duration throws `System::ArgumentOutOfRangeException`. Normal C++ ownership,
property calls, enum-to-string helpers and `CNAEXT GetTypeName()` are the only representation-level
adaptations.

The old F1 branch and help overlay are removed. Historical `help.png` is retained beside the
sample's `CMakeLists.txt`, outside `Content`, and is not loaded, copied or preloaded.

## Framework correction found by the sample

The original source intentionally loads names such as `Fonts\\TitleFont` and
`Textures\\Background`, while the official XNA pipeline emits `Textures/background.xnb`. XNA on
Windows accepts both the backslash separators and the case difference. The old CNA port avoided
that behavior with POSIX separators and exact-case loose files.

`ContentManager` now normalizes XNA path separators and resolves existing asset path components
case-insensitively on case-sensitive hosts. Exact matches still win, ambiguous case-colliding
matches fail deterministically, and the behavior applies uniformly to XNB, CNJ and loose-file
resolution rather than special-casing this sample. A regression test loads
`textures\\Background` from a physical `Textures/background.xnb`. The exact original asset names
now remain in the sample source.

## No-workaround review

The audited sample contains no loose content substitute, generated font sidecar, direct `SetData`
replacement, raw model/mesh helper, backend call, handwritten shader, omitted capability branch,
simplified charge switch, invented gameplay input or help overlay. No sharp-runtime change was
needed because its multicast action and argument exception already provide the required .NET
semantics.

## Verification evidence

All source snapshots, generated files, builds, scripts, logs and captures are under
`/rv/tmp/samples/SAMPLE-009-InputReporter_4_0`:

- `xna4-build/bin/InputReporter.exe` is the real XNA 4.0 reference. It runs in the dedicated
  Wine/WineD3D prefix; Space cycles the dead-zone mode and Escape exits. Its baseline and Circular
  captures are under `evidence/xna-original`.
- `cna-native-opengles3/samples/InputReporter/InputReporter_cna_samples` reports EasyGL OpenGL ES
  3.2 and `OPENGLES3`. Space and Escape pass. Its baseline and Circular 853x480 captures are
  byte-for-byte and pixel-for-pixel identical to the corresponding XNA captures, including the
  original Arial glyphs.
- `cna-web-webgl2/samples/InputReporter/InputReporter_cna_samples.{html,data,js,wasm}` is the
  complete browser bundle. System Google Chrome fetched every file with HTTP 200, reported WebGL
  2.0 and `CNA: graphics renderer: WEBGL2`, loaded the original backslash asset identifiers and
  produced no application, wasm or WebGL runtime error. Space and Escape pass. Its canvas differs
  from the XNA reference only by a normalized raster RMSE of about `0.00014` (below 0.015%).
- A separate audit-only copy of the generated HTML injects a standard browser gamepad without
  changing the shipped sample or CNA. Chrome visibly verified both thumbsticks, both triggers,
  A/B/X/Y, shoulders, stick buttons, all four D-pad directions, START and BACK. Holding START for
  2.5 seconds changed IndependentAxes to Circular; holding BACK for two seconds ended the game and
  cleared the canvas before a later injected input change. Reproduction script and captures are
  `scripts/capture-gamepad-web.sh` and `evidence/cna-web-webgl2/input-reporter-webgl2-*.png`.

Exact hashes and browser comparison metrics are recorded in `evidence/visual-comparison.txt`.
