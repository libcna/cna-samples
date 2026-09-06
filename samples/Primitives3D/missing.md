# Missing / Differences from XNA 4.0 original

**No known differences.** The content defect that reopened this row on 2026-09-02 is closed: the
port now ships the authentic XNA 4.0 pipeline product and both substitutes are gone.

Artifact root: `/rv/tmp/samples/SAMPLE-002-Primitives3DSample_4_0/`.

| Directory | Contents |
|---|---|
| `xna4-original/` | The unchanged upstream snapshot plus the `Primitives3D-audit.exe` reference build. |
| `xna4-build/` | The official XNA 4.0 pipeline runner and its `Content-windows` output. |
| `cna-native-opengles3/` | Reusable CMake build tree and the native `Primitives3D_cna_samples`. |
| `cna-web-webgl2/` | Reusable Emscripten build tree and the complete WEBGL2 bundle. |
| `evidence/` | XNA, native and browser captures with their logs. |
| `scripts/` | `build-original.sh`, `capture-xna-original.sh`, `capture-cna-native.sh`, `capture-cna-web.sh`, `chrome-smoke.mjs`, `serve-threaded-wasm.py`. |

## The content defect, closed

`Primitives3DContent.contentproj` has one row — `hudFont.spritefont`, `FontDescriptionImporter`,
`FontDescriptionProcessor` — and the game loads it as `Content.Load<SpriteFont>("hudfont")`. The
port used to ship `Content/hudfont.cnj` plus a loose `Content/hudfont.png` instead, and the
historical reference XNB was synthesized by `build-tools/make_spritefont_xnb.py` rather than built.

`scripts/build-original.sh` now drives the unchanged content project through XNA 4.0's own
`BuildContent` task for Windows/Reach, the platform and profile the sample's Windows configuration
declares. `hudFont.spritefont` asks for **Segoe UI Mono**, which the reference prefix already
carries; the script registers the face in the prefix's font registry, since
`FontDescriptionProcessor` resolves the name through it.

```text
Loaded 1 exact compiled content-project assets.
Importing hudFont.spritefont with Microsoft.Xna.Framework.Content.Pipeline.FontDescriptionImporter
Processing hudFont.spritefont with Microsoft.Xna.Framework.Content.Pipeline.Processors.FontDescriptionProcessor
BuildContent (Windows/Reach) result: True
```

`Content/hudFont.xnb` is that product, checked in unchanged: 13,486 bytes, SHA-256
`d944c80be4eb2bd3a4e8cb53945406401e59674f1a6d4a5261ff05513f5a59a7`, an `XNBw` container naming
`SpriteFontReader`, `Texture2DReader`, `ListReader<Rectangle>`, `ListReader<Char>` and
`ListReader<Vector3>`, carrying a 128x64 DXT3 glyph sheet, 95 glyphs for `' '`..`'~'`,
`LineSpacing = 18` and `Spacing = 0`. Both substitutes are deleted; `Content/` holds this one file,
and the WEBGL2 `.data` package is exactly 13,486 bytes.

The retained synthesized asset is kept only as superseded evidence
(`evidence/hudfont-synthesized-superseded.xnb`, 70,830 bytes) so the two can be told apart.

## Original reference, rebuilt on the authentic font

The reference executable was re-run with the authentic XNB in its own `Content/`
(`scripts/capture-xna-original.sh`, `evidence/xna4-original-authentic-font/`). Its IL was checked
first: the HUD is one `SpriteBatch.DrawString(spriteFont, text, Vector2(48, 48), Color.White)` over
a three-line string, so line placement comes from the font's own `LineSpacing` and nothing else.

**Capture the reference on a screen large enough for a decorated 800x480 window.** On an 800x480
Xvfb the Wine window manager leaves an 800x**460** client area, XNA presents its 480-row back
buffer into it, and every vertical measurement comes out squashed — the HUD's line pitch reads 17
instead of 18, which looks exactly like a font-metric defect and is not one. On a 1280x1024 screen
the same build reproduces the XNB's own numbers.

## Verification

**Native, `CNA_GRAPHICS_RENDERER=OPENGLES3`** (`scripts/capture-cna-native.sh`, eight captures in
`evidence/cna-native-opengles3-final/`): all five primitives in the original order — cube, sphere,
cylinder, torus, teapot — the colour cycle, the wireframe toggle and back, and Escape leaving with
status 0. The run log loads exactly one asset, `hudfont`, and carries no exception.

**The HUD against real XNA.** With both engines rendering the same authentic font at 800x480, the
three-line block at `(48, 48)` agrees to **99.89 % of pixels exactly**, mean absolute difference
**0.161 of 255**. The glyph ink tops land on rows 53, 71 and 89 in both — a pitch of 18, the
`LineSpacing` the XNB records — and the block is 397 px wide in both.

**Browser, `CNA_GRAPHICS_RENDERER=WEBGL2`** (`scripts/capture-cna-web.sh`,
`evidence/cna-web-webgl2-chrome/`): the complete `.html`/`.js`/`.wasm`/`.data` bundle served over
HTTP and driven in real Google Chrome — WEBGL2 context, 800x480 canvas, 600 animation frames, the
`hudfont` load, and A/B/Y each changing the frame, with no runtime exception, no fatal console
message and no HTTP error other than the browser's own `favicon.ico`.

## Code audit (unchanged, re-checked)

The 2026-08 line-by-line result still holds and was re-scanned against the stricter bar this
campaign now applies:

- the five primitives are created in the original order with the same defaults and construction
  formulae; all 127 teapot control points and 160 patch indices are numerically identical;
- the custom vertex is position `Vector3` at byte 0 plus normal `Vector3` at byte 12, stride 24;
- `BasicEffect.EnableDefaultLighting()`, matrices, rotation rates, depth/blend/rasterizer state,
  tint cycle and indexed triangle drawing match the original;
- keyboard, gamepad, mouse regions, edge-triggered input, Escape/Back exit and the three toggles
  retain the original branches;
- the exact three HUD strings, line breaks, `(48, 48)` position and white colour are present; the
  invented F1 overlay is gone and its historical `help.png` sits beside `CMakeLists.txt`, outside
  `Content`, unloaded;
- no STL type stands in for a .NET one the original names, every file carries its MS-PL SPDX line,
  and the two `SetData` calls are the original's own. `CNAEXT GetTypeName()` is the one required
  C++ runtime extension.

`Primitives3D.htm` is byte-identical to the upstream file.

## Framework changes

None. No `cnanext` or `sharp-runtimenext` defect was found by this row.
