# SAMPLE-010 audit — InputSequenceSample_4_0

Completed on 2026-09-13. No known behavioral or visual difference, workaround, missing dependency,
or CNA/sharp-runtime gap remains for this sample.

## Reference and source audit

- The exact 30-file upstream directory is retained at
  `/rv/tmp/samples/SAMPLE-010-InputSequenceSample_4_0/xna4-original` and matches
  `/rv/tmp/XNAGameStudio/Samples/InputSequenceSample_4_0` under `diff -qr`. The fresh pass corrected
  the old incomplete snapshot by adding both solutions, the HTML documentation and the license.
- `Direction.cs`, `InputManager.cs`, `Move.cs`, `MoveList.cs`, `Game.cs`, both project files, the
  Content project and support files were checked line by line against the C++ port.
- The exact move list and order are preserved: Jump, Punch, Double Jump, Jump Kick, Quad Punch,
  Fireball, Long Jump, Back Flip and 30 Lives. The port also preserves all sequences, sub-move
  flags, eight-way direction composition, two-player managers, A/B/X/Y keyboard map, Back/Escape
  exit, update/draw order, wrapping layout, drop shadows and 800x480 default presentation.
- The 500 ms buffer timeout, 100 ms merge window and one-second active-move timeout match. A
  separate logical capacity reproduces `List<Buttons>.Capacity`; a stable longest-first shallow
  pointer list reproduces LINQ's stable `OrderByDescending` without duplicating the move objects.
- Normal C++ ownership and initialization helpers, property syntax, enum display-name conversion
  and `CNAEXT GetTypeName()` are the only representation-level adaptations.
- Historical `help.png` remains beside `CMakeLists.txt`, outside `Content`; it is not loaded, copied
  or displayed.

## Content provenance

All 15 assets load through their original identifiers and remain verified Microsoft XNA 4.0 XNBs:
one Miramonte Bold SpriteFont and 14 input-symbol textures. A fresh build with the real XNA Content
Pipeline reproduced every texture XNB byte-for-byte. The freshly rebuilt `Font.xnb` differs because
current Wine/host font rasterization is environment-sensitive even with XNA's Miramonte fonts
installed. Its hashes are recorded in
`evidence/fresh-xna-original/content-rebuild-comparison.tsv`; per campaign policy it did not replace
the retained previously verified official font XNB used by all runtimes.

## No-workaround review

The sample has no loose runtime asset, generated font sidecar, direct `SetData`, raw mesh/model
helper, backend call, handwritten shader, invented control, omitted input path, F1 branch or help
overlay. It uses only XNA-facing CNA APIs. No sample source, CNA, sharp-runtime, meta-gl or easy-gl
change and no new stub was required.

## Fresh verification

All products, scripts, logs, captures and hashes are under
`/rv/tmp/samples/SAMPLE-010-InputSequenceSample_4_0`.

- Original XNA: fresh Debug and Release executables compile. The Debug runtime gate in an isolated
  Wine/WineD3D display recognizes all nine moves and exits cleanly with Escape.
- Native: the fresh sample-only Release OPENGLES3 build reports OpenGL ES 3.x and recognizes the
  same nine moves. The baseline and every active-move 800x480 capture are byte-for-byte and
  pixel-for-pixel identical to the XNA reference (`AE=0`); Escape exits with code zero.
- Web: the fresh four-file non-threaded Release WEBGL2 bundle has no DWARF or pthread/shared-memory
  markers. System Google Chrome fetches all four files with HTTP 200, creates an 800x480 WebGL 2
  canvas, reports GL error zero, recognizes all nine moves and clears the canvas after Escape with
  no application, wasm, WebGL or browser runtime error.
- The unfocused WEBGL2 baseline is pixel-identical to XNA over all 800x480 pixels. Chrome's focus
  decoration occupies the outer two pixels after keyboard focus; the remaining 796x476 game area
  is pixel-identical for every one of the nine active-move captures.
- The exact gallery copy produces the same eleven screenshot hashes and passes the complete Chrome
  gate again.
- Every compilation used at most four parallel jobs.

The canonical products are a runnable XNA reference with official Content, a stripped native
executable plus the same Content, and exactly four static-hostable web files. Detailed results are
in `evidence/fresh-source-audit.json`, `evidence/fresh-visual-comparison.tsv`, the three
`evidence/fresh-*` runtime directories and `evidence/SHA256SUMS`.

The reproducible work directories remain. Pruning SAMPLE-010 requires separate owner authorization;
only a dry run is permitted after completion.
