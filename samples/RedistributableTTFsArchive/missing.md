# SAMPLE-140 — `RedistributableTTFs_ARCHIVE_3_1` audit and owner decision

## Status

Fresh audit complete enough to require a shared-support-data decision under `SAMPLES-DEC-005`.
This delivery is Microsoft's licensed XNA Game Studio font pack, not a runnable sample. No font
gallery, text game, installer, synthetic content project or duplicate sample target was invented.

## Complete package inventory and licence boundary

The upstream delivery contains **29 files / 1,086,730 bytes**: 28 unchanged OpenType TrueType
font files and one 2007 Microsoft HTML document. It has no solution, project, C# source, content
project, entry point, executable, XNB or runtime behavior.

The supplied document says the fonts were created by Ascender Corporation and licensed by
Microsoft, are free to use in an XNA Game Studio game, and may be redistributed in their original
format as part of that game. It also makes the user responsible for the redistribution rights of
any other fonts and supplies this pack "as is." This audit does not broaden those terms into a
general standalone-font or newly invented gallery licence.

The document tells developers to install the files in `Windows\Fonts` and use the listed names in
SpriteFont XML. The table has one precise historical typo: it names News Gothic Bold as
`NGOB_____.ttf` (five underscores), while the delivered and valid file is `NGOB____.TTF` (four).
The audit records rather than silently corrects that source-document discrepancy.

## Exact font validation

All 28 physical files have the TrueType-flavoured SFNT signature and valid bounded table
directories, are recognized by Fontconfig with nonempty family/style/PostScript identities, and
cover every code point from U+0020 through U+007E. Twelve carry OS/2 `fsType` `0x0004` and sixteen
carry `0x0008`; the exact per-file values, table counts, charset ranges, sizes and SHA-256 hashes
are retained in `evidence/font-metadata.tsv`. The HTML's statement that these are an ANSI subset
with typographic additions is not inflated into a claim of complete Windows-1252 coverage.

The Unix `file` heuristic calls `MotorwerkOblique.ttf` a SIMH tape. That is a misclassification,
not a broken asset: its SFNT structure and names are valid, Fontconfig renders it, and the official
XNA 4.0 pipeline processes the exact file successfully. `evidence/font-preview.png` is a 28-row
offline contact sheet rendered directly from every delivered font; it is audit evidence only, not
a proposed sample product.

## Collection-wide consumer role

The complete upstream collection contains 220 `.spritefont` declarations. **171 declarations
across 67 distinct top-level sample directories** name nine families supplied by this pack:

| SpriteFont name | Declarations | Sample directories |
|---|---:|---:|
| Moire | 58 | 7 |
| Segoe UI Mono | 50 | 49 |
| Moire ExtraBold | 41 | 7 |
| Quartz MS | 12 | 1 |
| Kootenay | 3 | 3 |
| Motorwerk | 3 | 1 |
| Lindsey | 2 | 1 |
| Miramonte | 1 | 1 |
| Pericles | 1 | 1 |

Those declarations include completed samples and future rows such as Role Playing Game Phone and
Space Shooter. A full collection scan finds no second TTF anywhere outside this pack, so these
files are the shared design-time source for those references rather than redundant copies bundled
with each consumer. `evidence/spritefont-consumers.tsv` records every matching declaration.

## Authentic XNA 4.0 and CNA content evidence

Artifact root: `/rv/tmp/samples/SAMPLE-140-RedistributableTTFs_ARCHIVE_3_1/`.

`original/` is a byte-for-byte snapshot of all 29 upstream files. For a uniform, non-product
diagnostic, `scripts/generate-diagnostics.py` creates one 16-point Windows/Reach SpriteFont
declaration per exact font for U+0020–U+007E. `scripts/build-xna4-content.sh` clones the established
XNA 4 Wine prefix into an isolated temporary prefix, installs/registers the fonts only there, and
invokes the official XNA Game Studio 4.0 `FontDescriptionImporter` and
`FontDescriptionProcessor`. It removes all 28 process-private registrations and destroys the
temporary prefix on exit; the owner's base prefix and host font installation are not modified.

The official pipeline builds **28/28 Windows/Reach XNBs**. CNA's normal
`CNA.XnbImporter -> CNA.SpriteFontProcessor -> CNA.SpriteFontContentWriter` path then converts
**28/28** with eight workers. Every resulting CNB is identified as
`Microsoft.Xna.Framework.Graphics.SpriteFont`, is self-contained, and retains a 95-character map
plus meaningful atlas payload. This proves the pack's actual consumer role without pretending the
diagnostic declarations are an original game.

Twenty-three focused SpriteFont XNB/CNJ/CNB/compiler/runtime tests pass on EasyGL OpenGL ES 3.2
under an isolated Xvfb display. They cover real compressed and uncompressed SpriteFont fixtures,
XNB-to-CNB semantic equivalence, canonical tables, self-contained atlas loading and runtime
ContentManager use. No host browser or real desktop display is opened. No CNA or Sharp Runtime
source change was needed at CNA HEAD `5347b52eae13` and Sharp Runtime HEAD `9cc96cd57cde`.

## Evidence and reproducibility

- `evidence/inventory.tsv` and the audit's snapshot comparison cover all 29 exact source files;
- `evidence/font-metadata.tsv` retains SFNT/Fontconfig/charset/embedding-bit metadata and hashes;
- `evidence/spritefont-consumers.tsv` retains all 171 collection references;
- `evidence/xna4-spritefont-build.log` and `xna4-build/Content/` retain the 28 official XNBs;
- `evidence/cna-content-transcode.log`, `cnb-info.log` and `cna-diagnostic/` retain the 28 CNBs;
- `evidence/cna-spritefont-tests.log` retains the 23 passing real-GLES3 regressions;
- `evidence/font-preview.png` visually confirms every exact file renders;
- `scripts/qualify.sh` reproduces the full offline audit, uses at most eight CPU cores/workers and
  confines graphics to its own Xvfb display.

## Owner decision required

Choose one:

1. accept this as an evidence-backed licensed shared-support archive/non-port, with no runnable CNA
   sample target;
2. authorize retaining or packaging the exact original TTFs as shared design-time support data for
   the many source SpriteFont declarations, with distribution placement explicitly constrained by
   the supplied terms; or
3. explicitly scope a new native/WEBGL2 font-viewer/gallery product and define its purpose,
   distribution and browser-font contract, acknowledging that it would be a modernization/new
   product rather than a port of delivered runtime behavior.

Until that ruling, copying the fonts into an unrelated executable, treating an audit contact sheet
as a game, or declaring one of the 67 consumers to be this archive's standalone port would violate
the fidelity and owner-decision rules.
