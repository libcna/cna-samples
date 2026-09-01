# SAMPLE-123 — `ControllerImages` audit and owner decision

## Status

Fresh audit complete enough to require an owner representation decision under
`SAMPLES-DEC-005`. This is a licensed reusable controller-render pack, not an XNA game or
executable utility. No gallery, input visualizer, alias or other runtime product was invented
around the images.

## Classification and complete inventory

The entire upstream directory contains **5 files / 748,023 bytes**:

- four 512×512, 8-bit RGBA PNG renders of a white Xbox 360 controller;
- front orthographic, top orthographic and two top-perspective views;
- the 45,920-byte Microsoft Permissive License RTF.

There is no solution, project, content project, source file, entry point, executable, scene,
input path, runtime UI or usage document. The asset filenames and SHA-256 values are:

```text
con_front_ortho.png  6829cf329fcfd662e005452f8e18b8c004ba7c0072634e6165283eb1d191f6df
con_top_ortho.png    b6e3cfff2ee55fcae894c3d6d691a4d78c895d2c296974cc68e7f19da347615a
con_top_persp1.png   b8d0ce7ddd710d744dc1cc6047a921d9068a601f1c2a3377bd0cd07bc9c9f553
con_top_persp2.png   79da7f4ef8eb5491febb7ca5f57e1851f4cfa9af86ab2b1ce187fa633f51c9e6
```

All four images use transparent backgrounds and contain the complete controller rather than
separate button glyphs. They are materially different from SAMPLE-120 `ButtonImages`, whose
fourteen individual TGA controls plus icon-font strip have a documented content-pipeline contract.

## Consumer audit

A hash scan covered every other PNG in the complete upstream sample collection. None of these
four files has a byte-identical copy elsewhere. A source/project/document scan found no reference
to any exact filename or to the `ControllerImages` directory.

The only generic text hit is this comment in `InputReporter_4_0`:

```csharp
// draw the connected-controller images
```

It does not identify this pack. InputReporter compiles and loads its own eight distinctly named
`connected_controller1`–`4` and `select_controller1`–`4` PNGs. Those are tiny 29×41 through
50×49 HUD fragments; their hashes differ from all four 512×512 renders. The already-complete
SAMPLE-009 port uses the exact official-pipeline XNBs built from those local inputs. Consequently
ControllerImages is not a missing dependency of InputReporter or another audited product.

## Authentic XNA 4 and CNA content evidence

A retained `BuildContent` harness passed every unchanged PNG through the official Microsoft XNA
4.0 Windows/Reach `TextureImporter` → `TextureProcessor` route. All four builds returned true and
produced four 1,048,763-byte `Texture2D` XNBs.

At live CNA HEAD `7712534d3d22`, `cna-content` consumed those four exact XNBs through
`CNA.XnbImporter` → `CNA.TextureProcessor` → `CNA.Texture2DContentWriter`, producing four validated
1,048,960-byte CNBs with zero failures. The representative container reports a Texture2D payload,
the expected Microsoft XNA type identity, 1,048,576 bytes of texture data and no external
references. Nine focused OPENGLES3 tests covering real XNB loading, cache/unload behavior,
container variants and runtime-XNB/transcoded-CNB pixel equality pass.

This proves that the ordinary PNG/Texture2D route exposes no CNA or Sharp Runtime defect. No
framework change or sample workaround was needed.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-123-ControllerImages/`.

- `xna4-original/ControllerImages/` is the complete byte-for-byte upstream snapshot;
- `evidence/file-inventory.txt`, `sha256sum.txt`, `image-metadata.tsv` and the empty
  `snapshot-diff.txt` cover every input;
- `evidence/all-other-png-sha256.txt`, `exact-copy-scan.tsv`,
  `consumer-reference-scan.txt`, `generic-controller-image-phrase.txt` and
  `inputreporter-controller-image-metadata.tsv` retain the complete consumer analysis;
- `xna4-build/Content/` retains all four official XNA 4 XNBs and their log, inventory and hashes;
- `cna-build/` retains all four CNA-transcoded CNBs; the transcode log, focused test log and
  representative CNB structure report are under `evidence/`;
- `scripts/build-content.sh`, `audit.sh` and `qualify.sh` reproduce the offline evidence. CNA
  content conversion is capped at eight workers.

There is no original/native/browser runtime gate because upstream supplies no runnable product.
Creating one would test newly authored behavior rather than this resource delivery.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port/resource-pack boundary for this complete licensed asset
   delivery;
2. classify it as retained shared source/support data for existing or future CNA samples, without
   inventing a standalone target; or
3. authorize a distinct controller-gallery/input-visualizer product, defining its behavior and
   native/WEBGL2 fidelity criteria.

Until that choice, creating a viewer or claiming the already-complete InputReporter port as this
directory's product would violate source fidelity.
