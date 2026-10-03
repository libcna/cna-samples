# SAMPLE-120 — `ButtonImages` audit and owner decision

## Status

**⛔ Cancelled by explicit owner decision, 2026-10-03:** "ponech cancelled a analyzuj 121".
The instruction refers to the just-analyzed SAMPLE-120 and requests SAMPLE-121 next.
Retain every original image, document, official XNB, CNA product and audit record. No standalone
gallery/input visualizer, shared-product acceptance or artifact cleanup was requested.
The successful content-pipeline results below remain valid evidence; cancellation defines the
sample's representation and does not assert a content compatibility defect.

## Current-head re-analysis — 2026-10-03

At the analysis checkpoint the status was **🛑, owner decision pending**. The owner cancelled SAMPLE-119 and requested the
next numbered sample; that instruction does not cancel SAMPLE-120. All **17 files / 1,125,559
bytes** still match the complete retained snapshot. All fifteen 32-bit/RLE TGAs decode; the
original HTML, fourteen-row character table, visible Ms-PL body and images were reviewed again.
The delivery has no application, code, project or runtime interaction.

Fresh content builds establish both faithful routes:

- Official Microsoft XNA 4.0 `BuildContent`, Windows/Reach, builds all fifteen unchanged TGAs:
  fourteen `Texture2D` XNBs and one `FontTextureProcessor` SpriteFont XNB. **All fifteen XNBs are
  byte-identical to the retained original-pipeline generation.** The harness is a diagnostic
  around the delivered assets, not an upstream project.
- The retained CNA host `cna-content` tool converts those exact XNBs to fifteen valid CNBs.
  Independently, it processes byte-identical TGA copies through `CNA.ImageImporter` and
  `CNA.TextureProcessor`, selecting `CNA.FontTextureProcessor` for the strip. Its explicit
  parameters preserve the documented XNA defaults and first character Space.
- **All fifteen native-source CNBs are byte-identical to the official-XNB-transcoded CNBs.**
  This includes all image bytes, character mapping, glyph/crop/kerning arrays, font settings and
  atlas layout. The icon font has fourteen characters U+0020–U+002D, line spacing 184, spacing
  zero, no default character, a 512×512 RGBA atlas with 1,048,576 payload bytes, and zero external
  references. CNA's container inspector accepts every output in both sets (30/30).

Current CNA `next 75b55659c` has both canonical and XNA-facing `FontTextureProcessor` source
routes; `XnaComponentNames` maps the original processor name and parameters. The host tools
above were **reused**, not rebuilt or represented as current-HEAD runtime qualification.
Their binary hashes, timestamps and commands are retained. The earlier **13/13 OPENGLES3 tests
below are historical**; no native/browser runtime gate was invented for this content-only pack.
No sample, CNA or Sharp Runtime implementation change is required by these measured assets.

A fresh scan again covers 1,309 other TGAs, with no byte-identical source copies. The only
matching code/content references remain Pathfinding's own 20×20 A/B/X/Y and Flocking's own
20×20 B/X/Y. Those completed ports do not define an executable product for this directory.

Recommend **archival cancellation with all source and evidence retained**, like the preceding
source-only packs. Alternatively the owner can retain it as shared support data without a
standalone target, or explicitly define a new asset-gallery/input-visualizer product and its
native/WEBGL2 acceptance criteria. The remaining choice is product scope, not a measured content
pipeline blocker. No standalone-viewer implementation estimate is established before its
behavior and acceptance criteria are defined. No pruning was requested.

Fresh evidence is under
`/rv/tmp/samples/SAMPLE-120-ButtonImages/evidence/current-head-analysis-20261003/`:
`source-audit.json`, `readme-text.txt`, `license-body-text.txt`, `documented-mapping.json`,
the two original-image PNG inspection renders, `consumer-hash-scan.json`,
`consumer-source-references.txt`, `official-xnb-inventory.json`, `official-harness-provenance.json`,
`native-source-contract.json`, `tools.json`, `cnb-inventories.json`, `content-equivalence.json`
and all build/inspection logs. Reproduce with
`scripts/current-head-analysis-20261003.py audit official cna compare`. Fresh official products
are in `xna4-build/current-head-20261003/Content/`, the two CNA sets in
`cna-build/current-head-20261003/{from-official-xnb,from-native-tga}/`.
Original snapshots and earlier products/evidence remain at their original paths.

## Classification and complete inventory

The entire upstream directory contains **17 files / 1,125,559 bytes**:

- fourteen individual 32-bit RGBA/RLE TGA images for BACK, START, Guide, DPad, A/B/X/Y, both
  shoulders, both triggers and both thumbsticks;
- one 1,729×188 RGBA/RLE TGA strip, `xboxControllerSpriteFont.tga`;
- a 15,926-byte HTML usage page and the 45,920-byte Microsoft Permissive License RTF.

The individual images range from 80×80 face buttons to 218×92 shoulder buttons. There is no
solution, project, content project, source file, entry point, executable, scene, input path or
runtime UI. Although the readme calls the delivery a utility, the delivered product is only the
images and documentation.

Representative SHA-256 values are:

```text
ButtonImages.htm                  1021bb99e400034c9dcdf7b3fccd99aa96e9eb40c2c409a8d42e752e1a04fe25
xboxControllerButtonA.tga         f3493d8940726e4afce9806c6c963299812a40e87536f81a469144a1e64a12b2
xboxControllerSpriteFont.tga      f43ae4961cbdb92f8b62f808315a99eede7b00aef47ac1cc1dcd57e7fad91c93
```

## Documented content contract

The individual files are ordinary `Texture2D` inputs. For the strip, the documentation explicitly
requires XNA's normal texture importer plus **Sprite Font Texture - XNA Framework**
(`FontTextureProcessor`). The resulting characters begin at Space and map exactly as follows:

| Character | Image |
|---|---|
| Space | Left Thumbstick |
| `!` | Directional Pad |
| `"` | Right Thumbstick |
| `#` | BACK |
| `$` | Guide |
| `%` | START |
| `&` | X |
| `'` | A |
| `(` | Y |
| `)` | B |
| `*` | Right Shoulder |
| `+` | Right Trigger |
| `,` | Left Trigger |
| `-` | Left Shoulder |

This is a 14-glyph icon font, not an ordinary text font.

## Historical authentic XNA 4 and CNA content evidence

A retained `BuildContent` harness passed every unchanged source image through the official
Microsoft XNA 4.0 Windows/Reach pipeline:

- all fourteen individual images succeeded through `TextureImporter` → `TextureProcessor` and
  produced `Texture2D` XNBs;
- the unchanged strip succeeded through `TextureImporter` → `FontTextureProcessor` and produced a
  1,050,225-byte `SpriteFont` XNB;
- all fifteen builds completed with `BuildContent ... result: True`.

At that earlier audit, CNA consumed those exact fifteen XNBs through `CNA.XnbImporter`. It selected
`CNA.TextureProcessor`/`Texture2DContentWriter` for the fourteen textures and
`CNA.SpriteFontProcessor`/`SpriteFontContentWriter` for the strip, producing fifteen structurally
valid CNBs with zero failures. The SpriteFont CNB retains fourteen glyph, crop, kerning and
character entries, its embedded atlas and zero external references. Thirteen focused
OPENGLES3 tests covering runtime/transcoded XNB equivalence, `Texture2D` and `SpriteFont`
`ContentManager` paths passed at that audit; these are retained historical runtime results.

That audit found that neither the historical TGA inputs nor the documented XNA content routes exposed a
CNA defect. No CNA or Sharp Runtime change was needed.

## Consumer audit

A hash scan covered all 1,309 other TGA files under the upstream sample collection. None of these
fifteen source images has a byte-identical copy elsewhere. The only source-code/content-project
references matching these asset names are:

- `Pathfinding_4_0`, which loads its own 20×20 A/B/X/Y images;
- `FlockingSample_4_0`, which loads its own 20×20 B/X/Y images.

The B/X/Y files are byte-identical between those two consumers. The ButtonImages A image is an
80×80 version of the same icon represented by Pathfinding's 20×20 A image; a measured box-filter
reduction is visually and numerically very close (normalized RMSE 0.00248483), but the evidence
does not claim the exact historical resampling algorithm. There are no consumers of the complete
strip or of the remaining full-size images.

The existing `SAMPLE-022` Pathfinding and `SAMPLE-024` Flocking ports are already complete and
qualified with authentic XNBs built from their own exact 20×20 inputs. Therefore this physical
directory is useful reusable source material, but it is not a missing runtime dependency of either
port.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-120-ButtonImages/`.

- `xna4-original/` is the complete byte-for-byte 17-file upstream snapshot;
- `evidence/file-inventory.txt`, `sha256sum.txt` and `image-metadata.tsv` cover every file and image;
- `evidence/readme-text.txt` retains the documented importer, processor and character mapping;
- `evidence/exact-copy-scan.tsv`, `consumer-reference-scan.txt`,
  `known-consumer-image-hashes.tsv` and `source-a-vs-pathfinding-a-box-rmse.txt` retain the complete
  consumer/relationship evidence;
- `xna4-build/Content/` retains all fifteen official XNA 4 XNBs, with build log, inventory and
  hashes under `evidence/`;
- `cna-build/` retains all fifteen CNA-transcoded CNBs; the transcode log, focused test log and
  representative CNB structure reports are under `evidence/`;
- `evidence/snapshot-diff.txt` is empty;
- `scripts/build-content.sh`, `audit.sh` and `qualify.sh` retain the historical workflow. The old
  `qualify.sh` names a superseded CNA checkout and overwrites historical products; use the new
  dated script above for this audit, which keeps both generations separately.

There is no original/native/browser runtime gate because upstream supplies no runnable product.
Creating one would test newly authored behavior rather than this asset delivery.

## Historical owner options before the 2026-10-03 cancellation

Choose one:

1. accept an evidence-backed non-port/resource-pack boundary for this complete licensed asset
   delivery;
2. classify it as retained shared source/support data for existing or future CNA samples, without
   inventing a standalone target; or
3. authorize a distinct asset-gallery/input-visualizer product, defining which images and glyph
   route it must expose plus native and WEBGL2 fidelity criteria.

Until that choice, a newly created viewer or aliasing the already-complete Pathfinding/Flocking
ports as this directory's product would violate source fidelity.
