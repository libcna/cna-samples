# SAMPLE-009 audit — InputReporter_4_0

Completed on 2026-09-13. No known behavioral or visual difference, workaround, missing dependency,
or CNA/sharp-runtime gap remains for this sample.

## Reference and source audit

- The exact 31-file upstream tree is retained at
  `/rv/tmp/samples/SAMPLE-009-InputReporter_4_0/xna4-original/InputReporter` and matches
  `/rv/tmp/XNAGameStudio/Samples/InputReporter_4_0` under `diff -qr`.
- Every C# source, generated resource, `.resx`, project file and Content declaration was checked.
  All 31 resource strings, 15 `Content.Load` identifiers and repeated value-drawing paths match.
- The port preserves the original classes and namespace; 853x480 layout; positions, colors and draw
  order; four-controller selection; capability branches; packet number; both dead-zone modes; and
  the two-second START/BACK charge switches. Normal C++ ownership, XNA property calls and
  `CNAEXT GetTypeName()` are the only representation-level adaptations.
- `ChargeSwitch::Fire` remains a zero-argument `System::MulticastAction<>`, and a negative duration
  throws `System::ArgumentOutOfRangeException`.
- The original source's `Fonts\\...` and `Textures\\Background` identifiers are preserved exactly,
  including the case difference from the emitted `Textures/background.xnb`. CNA's general content
  resolution handles Windows separators and XNA-compatible case-insensitive lookup; it is not a
  sample-specific alias.
- Historical `help.png` remains beside `CMakeLists.txt`, outside `Content`; the original sample does
  not load it, and the port does not copy or preload it.

## Content provenance

The runtime package contains the same 15 official Microsoft XNA 4.0 XNBs: six SpriteFonts, the
background and eight controller-state textures. A fresh Wine build used the real XNA 4.0 assemblies
and Content Pipeline. Its nine texture XNBs are byte-for-byte identical to the retained files. The
six freshly rebuilt SpriteFont XNBs differ because the current host/Wine font rasterization is
environment-sensitive, even with Microsoft Arial installed; exact sizes and hashes are recorded in
`evidence/fresh-xna-original/font-rebuild-comparison.tsv`. Per campaign policy, this environmental
rebuild did not replace the previously verified official Arial XNBs used by all three runtimes.

## No-workaround review

The sample contains no loose content substitute, generated font sidecar, direct `SetData`
replacement, raw model/mesh helper, backend call, handwritten shader, omitted capability branch,
simplified charge switch, invented gameplay input or help overlay. No CNA, sharp-runtime, meta-gl
or easy-gl source change and no new stub was needed.

## Verification evidence

All products, generated files, builds, scripts, logs and captures are under
`/rv/tmp/samples/SAMPLE-009-InputReporter_4_0`:

- Fresh XNA Debug and Release executables compile successfully. The Debug runtime gate in the
  isolated Wine prefix verifies the 853x480 window, Space transition from IndependentAxes to
  Circular and Escape exit.
- The Release OPENGLES3 product reports OpenGL ES 3.2. Space and Escape pass. Its baseline and
  Circular captures are byte-for-byte and pixel-for-pixel identical to XNA.
- The non-threaded Release WEBGL2 four-file bundle passes in system Google Chrome: all four files
  return HTTP 200, the canvas is 853x480, WebGL 2 is active, GL error is zero, and no application,
  wasm or browser runtime error occurs. Space and Escape pass.
- A test-only standard browser gamepad injection visibly exercises both thumbsticks, both triggers,
  A/B/X/Y, shoulders, stick buttons, all four D-pad directions, START and BACK. Holding START for
  longer than two seconds changes IndependentAxes to Circular; holding BACK exits and clears the
  canvas. The harness is not part of the shipped sample or bundle.
- Against the XNA canvas, normalized WEBGL2 RMSE is `0.000140265` at baseline and `0.000137801`
  in Circular mode, both below 0.02%.
- Both focused ContentManager tests for Windows separators/XNA case-insensitive lookup and
  deterministic rejection of ambiguous case collisions pass.
- All compilation used at most four CPU cores.

The canonical products are the original XNA runtime, a stripped native executable plus Content,
and exactly four web files. The site copy has identical hashes and passed the complete browser gate
again. Exact hashes and metrics are in `evidence/SHA256SUMS`, `evidence/fresh-source-audit.json`,
`evidence/fresh-visual-comparison.tsv` and the `evidence/fresh-*` directories.

The reproducible work directories remain. Pruning requires separate owner authorization; only a
dry run is permitted after completion.
