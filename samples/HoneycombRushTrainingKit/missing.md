# Missing / Differences from XNA 4.0 original

## Fresh 2026-08-31 audit — authentic Songs obtained; owner-cancelled non-port

**Current status: `⛔`; no C++ port or sample workaround was added.** On 2026-09-05 the owner
explicitly decided that none of the three teaching stages will be ported. This is not a duplicate of
`SAMPLE-063`; it is an audited, deliberately cancelled multi-stage product. The authoritative
90-minute `Multi-Platform Development.docx` defines a staged lab whose purpose is to preserve
Honeycomb Rush behavior while porting a Windows Phone 7 game to Windows and Xbox 360. The
complete 472-file, 69,378,362-byte package and the audit scripts are retained at
`/rv/tmp/samples/SAMPLE-064-HoneycombRushTrainingKit_4_0/`.

Every supplied stage and project was inspected:

- `Sources/Starter` is a Windows Phone/Reach project with 41 compiled C# units (10,647 lines). It
  starts from the performance-heavy Honeycomb Rush endpoint and adds ten `GameDebugTools` units.
- `Sources/Ex1 - Windows Version` contains the 43-unit phone/Reach and Windows/HiDef endpoints
  (11,474 source lines). It adds keyboard control, asynchronous XNA Storage high scores, HD
  assets, `ScaledSpriteBatch`/`ScaledAnimation`, per-platform UI layout and PC instructions.
- `Sources/Ex2 - Xbox Version` is the 43-unit final endpoint (11,741 source lines) with separate
  phone/Reach, Windows/HiDef and Xbox 360/HiDef projects. It adds gamepad input, title-safe-area
  layout, Guide-visible pausing, Xbox prompts and final movement tuning. Its debug console exposes
  the documented `ToggleBounds` and `ToggleCollisionAreas` commands on Windows.
- The five content projects contain 47/50/48/50/47 compiled items depending on stage/profile,
  two verbatim copied XML inputs apiece and exactly two `SongProcessor` inputs apiece. The final
  HD project contains 50 compiled items, including the PC/Xbox instruction textures and the
  debug-tool `Smoke`/`Square` textures.

The retained `scripts/build-original.sh` drives the unchanged final Ex2 Windows/HiDef content
project through XNA 4.0's official `BuildContent` task, then compiles all 43 unchanged final C#
units. The exact content build fails at the first required Song:

```text
Loaded 50 exact compiled content-project assets.
Processing Sounds\InGameSong_Loop.wav with Microsoft.Xna.Framework.Content.Pipeline.Processors.SongProcessor
Could not convert audio file InGameSong_Loop.wav to WindowsMedia format.
BuildContent (Windows/HiDef) result: False
```

`MenuMusic_Loop.wav` uses the same route. Both source WAVs are byte-identical to the already
blocked SAMPLE-063 inputs, so this is the measured `SAMPLES-DEC-007` Windows Media encoder gap,
not a new format issue. A separately labelled diagnostic run that omits only those two conversions
builds all other 48 final HD assets, copies both XML files and produces the unchanged 43-unit
Windows executable. That proves the remainder of the selected endpoint, but it is not acceptable
runtime content: the original loads both names as `Song` and plays them with `MediaPlayer`.

After the owner supplied guest access, the offline Win7 XNA pipeline rebuilt the unchanged final
50-item Windows/HiDef content project and returned zero. It exported authentic XNB/WMA pairs for
both Songs; their 17,589 ms XNB durations exactly match valid stereo WMA v2 streams. These are
distinct Windows (`XNBw`) products from SAMPLE-063's Phone (`XNBm`) products. The XNB SHA-256
values are `2a93512fae3c167e69b95b87227bcf17033af27f60a6af6a2116f7de51d05858` and
`4e76ab0a2435a9f010713f9fff49951c64c71d099880d696c3494f5a93b4a3d9`; logs, streams and full
checksums are under `/rv/tmp/samples/SAMPLES-DEC-007-Win7-SongProcessor/export/`.

## Owner decision — `SAMPLES-DEC-005` resolved for this row

On 2026-09-05 the owner explicitly cancelled `SAMPLE-064`: Starter, Ex1 and Ex2 will not be
ported. The retained source, document, official content-build outputs and audit evidence preserve
what this distinct training kit contains. No loose-WAV substitution, hand-authored Song XNB,
collapsed platform branch or repurposing of SAMPLE-063 was accepted as a replacement port.
