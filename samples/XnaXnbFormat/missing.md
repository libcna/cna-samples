# SAMPLE-153 — `XNA_XNB_Format` audit and owner decision

## Status

Fresh audit complete enough to require an owner product/licensing decision under
`SAMPLES-DEC-005`. This delivery is not an XNA game and not a C# sample waiting to be translated:
it is Microsoft's 27-page XNA 4 compiled-content format reference plus an already-C++ Win32
console inspector. No invented `Game`, reduced loader, duplicate CNA parser or repackaged Microsoft
document was added.

Artifact root: `/rv/tmp/samples/SAMPLE-153-XNA-XNB-Format/`.

## Complete delivered product

The exact delivery contains **28 files / 127,060 bytes**:

- `XNB Format.docx`, Microsoft's 2011 **Microsoft XNA Game Studio 4.0 Compiled (XNB) Content
  Format** specification;
- one Visual Studio 2010 Win32 console solution, `Example XNB Parser`;
- 24 C++ headers/sources / 2,842 lines, plus the solution/project/filter files;
- no C#, `Game`, Content project, XNB fixture, executable, screenshot or separate licence file.

The document describes the complete XNA4 container contract: header/platform/profile/compression
fields, 7-bit integers, type-reader manifests, raw versus polymorphic values, deferred shared
resources, primitives and generic collections, `DateTime`/`Decimal`, XNA math and `Curve`, textures
and mip levels, vertex/index/declaration data, compiled and stock effects, `SpriteFont`, `Model`,
`SoundEffect`, `Song`, `Video` and reflective content. It renders cleanly as 27 US-letter pages;
the rendered PDF, extracted text and page metadata are retained as evidence.

The parser registers **57** concrete/generic reader factories and logs field structure rather than
constructing XNA runtime objects. It is useful historical example code, but it is deliberately not
a complete modern runtime loader:

- it recognizes only the original Windows/Phone/Xbox platform letters;
- it rejects every compressed XNB with `Don't support reading the contents of compressed XNB
  files.`;
- `ReflectiveReader` always throws and tells a consumer to implement/register a manual reader;
- shared-resource IDs and later objects are logged, not correlated into a resolved object graph;
- it uses VS2010/MSVC-specific `fopen_s`, `sprintf_s` and `for each` syntax;
- the delivery supplies no input fixture or automated behavioral test.

## Original-tool qualification and current Win7 blocker

An immutable 28-file snapshot and a separate `win7-work/` copy were prepared. Two authoritative
fixtures were staged: an uncompressed SpriteFont built by the owner's offline XNA4 toolchain and a
real LZX-compressed MonoGame SpriteFont already covered by CNA. The retained scripts build the
unchanged `Release|Win32` solution, expect the uncompressed file to parse, and expect the example's
documented LZX refusal.

The owner-provided Win7 VM was kept headless and fully offline; all eight adapters remained
`none`. After the host restart, however, normal Windows boot returned to WinRE before Guest
Additions or VS2010 became available. A read-only WinRE `DiskPart` inspection established the exact
host blocker:

```text
Disk 0  Online  32 GB
Volume 1  C:  RAW   Partition  31 GB  Healthy

Disk 1  Online  10 GB
Volume 2  D:  NTFS  Partition   9 GB  Healthy
```

WinRE reports `Unknown on (Unknown) Local Disk`; `C:` is the Win7 volume and the healthy 9GB NTFS
volume is the separately attached XP disk. No `chkdsk /f`, Startup Repair, BCD edit, restore,
detach or other write was performed. The VM was cleanly shut down with `wpeutil shutdown` and is
`poweroff`, still with all adapters `none`. Therefore no unchanged VS2010 build/run result is
claimed in this audit. The screenshots and exact diagnostic transcript are retained so the owner
can decide how to recover or restore the VM without an agent risking its data.

This environmental failure does not manufacture a missing CNA feature and does not change the
product-scope conclusion: even a successful build would demonstrate the documented limitations of
an already-C++ educational logger, not identify a C# game port.

## Live CNA boundary

Live CNA already owns the reusable product this archive describes: real XNB ingestion through
`ContentManager`, `ContentReader`, `ContentTypeReaderManager` and built-in/custom readers. Relative
to the example, CNA presently has:

- uncompressed, LZX and MonoGame raw-block LZ4 container support;
- FNA's exact 16 accepted platform identifiers;
- deferred shared-resource fixups and complete runtime object construction;
- primitive, math, curve, texture, font, audio, effect, model and generic-collection readers;
- explicit custom-reader registration plus `ReflectiveTypeReaderBuilder<T>` for declared C++
  member lists, including shared-resource fields;
- externally produced fixtures, exact bounds checks, deterministic fuzzing and sanitizer-derived
  hardening.

Focused live qualification passes **263/263 tests from 43 suites** across XNB headers/containers,
LZX/LZ4, `ContentReader`, content-type readers, reflective/custom content, shared resources and
real renderer-backed assets. This is regression evidence for CNA's loader; it is not falsely
presented as a port/run of the old inspector.

The audit found one small documentation drift rather than a loader defect. CNA's support matrix
still described only PCM16 `SoundEffect` and omitted newer normalized/DXT texture behavior.
`cnanext e3e72bcac` refreshes the matrix to the live tested contract: PCM8/16, IEEE float,
MS/IMA-ADPCM with honest XMA2 refusal; `NormalizedByte2/4` and DXT1/3/5; and native compressed DXT
retention for the opt-in WebGPU path versus lossless software decoding elsewhere. The current
263/263 focused tests pass after that documentation-only change; the retained pre-correction log
records the earlier 257/257 test binary separately rather than rewriting historical evidence.

## Licensing and product boundary

The document states that it is provided as-is for informational purposes and may be copied or
modified only for internal/reference purposes; it explicitly says that furnishing it grants no
other intellectual-property licence. The directory has no Ms-PL file or separate redistribution
licence for the example sources. Consequently, copying the DOCX or its parser wholesale into the
public CNA samples tree cannot be assumed permissible.

Independently of licensing, translating this row as a normal C#→C++ game would be category error:
the only program is already C++, and faithfully preserving its known missing compression,
reflection and graph-resolution behavior would create an inferior second loader beside CNA's
canonical implementation. Conversely, wrapping CNA's loader in a newly designed CLI could be a
useful project, but it would be a new CNA tool with a new output contract, not a faithful port of
this delivery. No such scope expansion was inferred.

## Evidence and reproducibility

- `original/` — exact byte-identical 28-file upstream snapshot;
- `evidence/source-inventory.tsv`, `original.sha256`, `snapshot-diff.txt` and
  `snapshot-hash-check.log` — inventory and integrity evidence;
- `evidence/docx-render.pdf`, `docx-text.txt` and `docx-pdfinfo.txt` — complete 27-page read-only
  document review;
- `win7-work/`, `fixtures/`, `scripts/build-win7-release.cmd` and
  `scripts/run-win7-parser.cmd` — isolated exact build/run route ready after VM recovery;
- `evidence/win7-diskpart-disks.png`, `win7-diskpart-volumes.png`, `win7-raw-volume.txt` and
  `win7-offline-state.txt` — exact VM blocker, clean shutdown and offline proof;
- `evidence/cna-xnb-tests-before-doc-fix.log` and `cna-xnb-tests.log` — focused live XNB regression
  evidence before and after the documentation correction;
- `scripts/qualify.sh`, `evidence/qualification.log` — repeatable snapshot, limitation and focused
  CNA gates.

No original executable result, sample-native executable, browser bundle or visual comparison is
claimed because this row contains no game/runtime visual product and the authentic tool build was
blocked by the RAW VM system volume.

## Owner decision required

Choose one:

1. retain the measured archive as internal historical/reference evidence and classify it as a
   non-port, without redistributing the DOCX or example sources;
2. provide a redistribution licence and explicitly scope preservation of the original VS2010
   inspector as an archival support product after the Win7 volume is recovered; or
3. authorize a newly designed, separately licensed CNA XNB inspection CLI built on CNA's canonical
   decoder, defining its stable machine/human output, compressed/custom-reader behavior and native
   test fixtures while acknowledging that it is a new tool rather than this parser's port.

No option should introduce an invented `Game`, fork XNB decoding into cna-samples, or regress CNA
to the example parser's compression/shared-resource/reflective limitations.
