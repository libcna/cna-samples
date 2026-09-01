# SAMPLE-120 — `ButtonImages` audit and owner decision

## Status

Fresh audit complete enough to require an owner representation decision under
`SAMPLES-DEC-005`. This is a licensed reusable controller-image pack, not a game or executable
utility. No gallery, controller viewer, input visualizer or sample target was invented around the
assets.

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

## Authentic XNA 4 and CNA content evidence

A retained `BuildContent` harness passed every unchanged source image through the official
Microsoft XNA 4.0 Windows/Reach pipeline:

- all fourteen individual images succeeded through `TextureImporter` → `TextureProcessor` and
  produced `Texture2D` XNBs;
- the unchanged strip succeeded through `TextureImporter` → `FontTextureProcessor` and produced a
  1,050,225-byte `SpriteFont` XNB;
- all fifteen builds completed with `BuildContent ... result: True`.

Current CNA then consumed those exact fifteen XNBs through `CNA.XnbImporter`. It selected
`CNA.TextureProcessor`/`Texture2DContentWriter` for the fourteen textures and
`CNA.SpriteFontProcessor`/`SpriteFontContentWriter` for the strip, producing fifteen structurally
valid CNBs with zero failures. The SpriteFont CNB retains fourteen glyph, crop, kerning and
character entries, its embedded atlas and zero external references. Thirteen focused
OPENGLES3 tests covering runtime/transcoded XNB equivalence, `Texture2D` and `SpriteFont`
`ContentManager` paths pass.

This proves that neither the historical TGA inputs nor the documented XNA content routes expose a
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
- `scripts/build-content.sh`, `audit.sh` and `qualify.sh` reproduce the evidence with no network or
  sample workaround.

There is no original/native/browser runtime gate because upstream supplies no runnable product.
Creating one would test newly authored behavior rather than this asset delivery.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port/resource-pack boundary for this complete licensed asset
   delivery;
2. classify it as retained shared source/support data for existing or future CNA samples, without
   inventing a standalone target; or
3. authorize a distinct asset-gallery/input-visualizer product, defining which images and glyph
   route it must expose plus native and WEBGL2 fidelity criteria.

Until that choice, a newly created viewer or aliasing the already-complete Pathfinding/Flocking
ports as this directory's product would violate source fidelity.
