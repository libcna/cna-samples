# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked. No C++ port or invented game wrapper has
been started.** The upstream product is a Windows Forms design-time font authoring utility. It has
no XNA `Game`, runtime XNA reference or content project. A faithful port requires a decision to
expand this repository from runnable samples into desktop authoring tools, and its stated product
also depends on the XNA `FontTextureProcessor`/"Sprite Font Texture" pipeline route.

Source: `/rv/tmp/XNAGameStudio/Samples/BitmapFontMaker_4_0/`.

Retained audit root: `/rv/tmp/samples/SAMPLE-090-BitmapFontMaker_4_0/`.

## Audited original

The directory contains one .NET 2.0 AnyCPU `WinExe` named `ttf2bmp.exe`. The three application
source units contain 660 lines; generated resources and assembly metadata add 96 lines. The
835-line supplied documentation agrees with the complete source. The utility:

- enumerates every Windows `FontFamily` into a WinForms selector and previews the selected font;
- selects Regular, Italic, Bold or Bold+Italic, an arbitrary positive point size and antialiased
  (`AntiAliasGridFit`) or one-bit (`SingleBitPerPixelGridFit`) rasterization;
- accepts decimal or `0x` character bounds, with an inclusive minimum and exclusive maximum;
- defaults to `0x20` through `0x7F`, therefore exporting the 95 printable ASCII code points
  `U+0020` through `U+007E`;
- measures and draws every glyph through `System.Drawing`, crops only empty left/right columns,
  then restores one pixel of horizontal padding when possible;
- packs 16 glyphs per row with eight-pixel separators into a 32-bit ARGB BMP;
- clears the atlas to magenta and uses source-copy compositing for glyph images, producing the
  marker layout the XNA font-texture processor uses to discover glyph cells;
- exposes a normal Save File dialog and reports invalid fonts, sizes, ranges and write failures
  through a message box.

It does not itself create an XNB or a `SpriteFont`. Its documented workflow is deliberately two
stage: export BMP, optionally edit the glyph art in an image editor without damaging the magenta
markers/alpha, then select the XNA "Sprite Font Texture" processor in a separate content project.
Replacing this with an in-game font preview would not port the product.

## Original build and execution evidence

`scripts/build-original.sh` compiles the unchanged project in Release with local Mono/xbuild. The
result is a 15,360-byte PE32 .NET GUI assembly with SHA-256
`606d48a211bf407f64918add6d5949d435a8279fb3ffdfc867de88b454121c54`.

The exact assembly opens successfully in an isolated Xvfb/Mono WinForms session. Retained captures
show the 378x450 selector/preview form and the real Export Font dialog:

- `evidence/original-mono-main.png`;
- `evidence/original-mono-save-dialog.png`;
- `evidence/original-mono-after-export.png`.

The host does not have Comic Sans MS, so the functional export probe explicitly selected installed
DejaVu Sans, Regular 23, antialiasing, and the default printable-ASCII interval. The unchanged
handler wrote a 425x332 32-bit BMP (SHA-256
`0c4e33fc1682f408fea27a73bc08b1c3cca2f9b6f70f0ffb7cf16f40982c8e4b`). Mono/libgdiplus did
not preserve the expected marker/glyph atlas: inspection finds almost the entire file transparent
black and only a tiny rendered remnant. That is host compatibility evidence, not a claimed Windows
reference output and not evidence that the Microsoft sample is defective. A faithful tool project
must establish the pixel baseline on the available offline Win7/XNA machine with a named installed
font and verify the resulting BMP through the official XNA processor.

## Live CNA and Sharp Runtime audit

The audit used CNA `35268971c` and Sharp Runtime `next` at `bd282d1016`; neither dependency was
modified. The owner's separate XML-serializer work is unrelated to this tool.

Sharp Runtime has no `System::Windows::Forms` or `System::Drawing` tree. Consequently the complete
UI and raster path are absent: `Application`, `Form`, controls/events, dialogs, resources,
`FontFamily` enumeration, `Font`, GDI+ measurement/drawing, `Bitmap`, pixel formats,
compositing/text-rendering modes and BMP encoding.

CNA can consume official SpriteFont XNBs and its current native content pipeline can encode an
explicit CNJ SpriteFont description into CNB. That path starts from an already-described RGBA
atlas plus glyph bounds/cropping/kerning. The live tree contains no XNA-compatible
`FontTextureProcessor`/`BitmapFontProcessor` importer that discovers ordered glyph cells from this
sample's magenta-marker BMP. A generic texture importer is not equivalent.

The browser requirement is also a genuine scope choice for this product. Browser font enumeration,
Canvas text metrics/rasterization and file download do not reproduce Windows GDI+ pixels or the
WinForms workflow automatically, and browsers cannot assume access to arbitrary locally installed
fonts. A browser-only rewrite would need an explicit owner-approved modernization contract and
test fonts, not silent substitution.

## Current result and resume conditions

No C++ sample directory, fake `Game`, reduced CLI, alternate GUI, loose runtime atlas, sample-local
font parser or CNA/Sharp Runtime workaround was added. SAMPLE-090 remains `🛑` under both:

- `SAMPLES-DEC-005`, for whether the campaign includes faithful WinForms/design-time tools and
  whether such a tool may receive a documented native-only or modernized-browser scope;
- `SAMPLES-DEC-002`, for whether CNA should ingest the exported XNA magenta-marker font texture as
  authored content rather than only consuming pregenerated SpriteFont XNB/CNJ data.

Realistic owner choices are:

1. accept an evidence-backed non-game/design-time-tool non-port result;
2. authorize a faithful Windows desktop tool scope, including an authentic Win7 baseline and the
   required UI/drawing implementation or an explicitly accepted hosting strategy, plus official
   processor round-trip tests; separately rule on the campaign's browser gate;
3. authorize a documented cross-platform modernization (for example a native/browser font-file
   input and downloadable marker BMP) and explicitly define acceptable UI, rasterization/font and
   output differences, together with a reusable CNA marker-BMP SpriteFont importer.

A headless converter, bundled replacement font, direct CNJ emitter or runtime preview can be useful
new tooling, but none is the original product unless the owner selects it as an explicit deviation.

---

## Re-audited 2026-09-09: this is not an XNA sample, and the gap it points at is real

**It does not reference XNA at all.** `ttf2bmp` is 756 lines of WinForms over `System.Drawing`,
`System.Drawing.Imaging`, `System.Drawing.Drawing2D` and `System.Drawing.Text`; a grep for
`Microsoft.Xna` across its sources and its `.csproj` returns nothing. It is a design-time desktop
utility whose *output* feeds XNA, not a program that uses XNA.

That settles what porting it would mean: building a WinForms-shaped GUI application inside a game
framework, to produce a file. Nothing in the campaign's definition of a port applies, and CNA has
neither the widget toolkit nor a reason to grow one.

**The gap it points at is separable and outlives the tool.** XNA has two font routes into a
`SpriteFont`:

| route | XNA processor | CNA |
| --- | --- | --- |
| `.spritefont` XML → rasterise an installed TrueType face | `FontDescriptionProcessor` | **present** — `CNA::Content::Pipeline::FontDescription` in `SpriteFontContentPipeline.hpp` |
| a pre-rendered bitmap whose glyphs are separated by a marker colour | `FontTextureProcessor` | **absent** |

`ttf2bmp` exists to produce the input for the second route: `MainForm.cs:219` clears the atlas to
`Color.Magenta` and blits each glyph over it with `CompositingMode.SourceCopy`, saving a 32-bit ARGB
BMP.

So the missing piece is not the tool. It is that **CNA cannot consume a marker bitmap font at all**,
which affects any XNA project that ships a hand-drawn or pre-rendered font rather than a
`.spritefont` description — regardless of what produced the bitmap. Whatever is decided about this
row, that gap stands on its own and is worth recording separately.
