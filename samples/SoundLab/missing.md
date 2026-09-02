# SAMPLE-145 — `SoundLab` audit and owner decision

## Status

Fresh audit complete enough to require an owner representation decision under
`SAMPLES-DEC-005`. The delivered directory is a licensed audio resource pack, not the standalone
audio authoring tool described by the old plan. No player, browser soundboard, editor or alias of
an existing audio sample was invented around it.

## Complete delivered product

The entire upstream directory contains **174 files / 85,922,233 bytes**:

- 173 WAV files grouped into Body Hits (43), City Ambience (3), Explosions (4), Footsteps (9),
  Guns (21), Nature Ambience (30), UI (54) and Whoosh (9);
- one Microsoft Permissive License RTF, byte-identical to the copy shipped with many other XNA
  samples (`07ed83b...c8d11`);
- zero solution, project, content-project, source, XAML, executable, XACT graph, entry-point,
  usage-document or UI files.

The audio ranges from the 50 ms `Body_Hit_34.wav` to the 96.264 s
`AfternoonAmbienceSimple_02.wav`. Of the 173 inputs, 172 are ordinary PCM16 (mono or stereo,
22,050/44,100/48,000 Hz). `Guns/Minigun/MinigunWindup_7_21.wav` is the sole PCM24 file. Every
stream is accepted by `ffprobe`; the full codec/channel/rate/bit-depth/duration table is retained.

There is therefore no authentic application run, frame sequence, interaction or authoring
workflow to reproduce. Calling this directory an editor or constructing a viewer from the assets
would define a new product.

## Collection-wide consumer audit

A SHA-256 scan covers all 709 WAV files in the complete upstream collection. None of the 173
SoundLab files has a byte-identical copy outside this pack. A source/project/document scan using
whole filename boundaries finds no reference to any exact SoundLab filename. This corrects the
initial broad substring scan in which generic `fire.wav` appeared inside Spacewar names such as
`hax2_fire.wav`; those Spacewar files have different hashes and are not consumers of this pack.

Consequently SoundLab is useful reusable source material, but it is not a missing runtime asset
dependency of another numbered sample.

## Authentic XNA 4 and CNA content evidence

Because upstream supplies no content project, the retained build is explicitly a diagnostic
inventory route rather than a reconstructed application. An offline harness feeds each unchanged
WAV to Microsoft's official XNA 4.0 Windows/Reach `WavImporter` → `SoundEffectProcessor`:

- the unfiltered build stops exactly at `MinigunWindup_7_21.wav`, reporting that XNA supports only
  8-bit and 16-bit input audio;
- a second diagnostic excludes only that already-proven unsupported PCM24 file and builds all
  other **172/172** assets successfully into 85,625,372 bytes of authentic SoundEffect XNBs;
- current CNA consumes those exact XNBs through `CNA.XnbImporter` →
  `CNA.SoundEffectProcessor` → `CNA.SoundEffectContentWriter`, producing **172/172** valid CNBs
  with zero failures;
- all 172 containers pass `cna_tool_cnb_info`; 44 focused content/XNB tests and 154 focused
  SoundEffect/SoundEffectInstance tests pass with the dummy audio backend.

A separate direct-source CNA audit is retained rather than hidden. It builds 166 files and
truthfully refuses seven: the PCM24 file, plus six Whoosh WAVs whose `data` chunks physically fit
the file but exceed the RIFF form's declared end by 68 bytes. XNA/FNA-style readers ignore the
RIFF length and the official XNA pipeline accepts those six; CNA's native authoring importer
deliberately enforces the stricter `CNBF-117` container-integrity contract. The official XNB route
proves runtime/content compatibility without rewriting any source asset. If the owner chooses a
new source-packaging product, whether to retain that strict policy or normalize those six files is
part of the new authoring contract, not a sample-side workaround to apply silently here.

No CNA or Sharp Runtime source change was needed for the product actually delivered.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-145-SoundLab/`.

- `xna4-original/` is the complete byte-for-byte 174-file upstream snapshot;
- `evidence/source-inventory.tsv`, `original.sha256`, `source-shape.tsv` and the empty
  `snapshot-diff.txt` cover every input;
- `evidence/wav-metadata.tsv`, `wav-format-summary.txt` and `wav-category-counts.tsv` retain the
  complete audio inventory;
- `evidence/collection-wav-sha256.tsv`, the empty `exact-copy-consumers.tsv` and the empty
  whole-name `consumer-reference-scan.txt` retain the collection-wide consumer proof;
- `xna4-build/Content-compatible/` retains all 172 official XNA 4 XNBs, while
  `evidence/xna4-content-build.log` preserves the unfiltered PCM24 failure and
  `xna4-content-compatible-build.log` preserves the successful remainder;
- `cna-diagnostic/` retains 172 XNB-transcoded CNBs; `cna-source-diagnostic/` and both CNA build
  logs preserve the distinct 166/173 direct-source result;
- `evidence/cna-content-tests.log`, `cna-audio-tests.log` and `qualification.log` retain the
  passing focused qualification;
- `scripts/audit-wavs.sh`, `XnaPipelineRunner.cs`, `build-xna4-content.sh`,
  `build-cna-content.sh` and `qualify.sh` reproduce the offline evidence, with every parallel
  operation capped at eight workers.

There is no native-render or browser gate because the upstream pack has no runnable product.
Adding such a gate would measure newly authored behavior rather than SoundLab.

## Owner decision required

Choose one:

1. accept an evidence-backed non-port/resource-pack boundary for the complete licensed delivery;
2. retain it as shared source/support data for existing or future CNA samples, without inventing a
   standalone endpoint; or
3. authorize a distinct sound gallery/editor/product and define its full asset set, playback and
   authoring behavior, direct-source normalization policy, native gate and WEBGL2 gate.

Until that decision, a soundboard, content project or authoring UI would be invented scope rather
than a faithful port.
