# Missing / Differences from XNA 4.0 original

## Fresh 2026-08-31 audit — distinct training kit blocked on authentic Songs and scope

**Current status: `🛑`; no C++ port or sample workaround was added.** This is not a duplicate of
`SAMPLE-063`. The authoritative 90-minute `Multi-Platform Development.docx` defines a staged lab
whose purpose is to preserve Honeycomb Rush behavior while porting a Windows Phone 7 game to
Windows and Xbox 360. The complete 472-file, 69,378,362-byte package and the audit scripts are
retained at `/rv/tmp/samples/SAMPLE-064-HoneycombRushTrainingKit_4_0/`.

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

The owner-requested Win7 retry on 2026-08-31 verified that VirtualBox, the guest and Guest
Additions now boot and that a narrow export share is configured. It then stopped at a newly
measured access blocker: the saved `vboxuser` automatic-login credential is invalid and empty-
password Guest Control is rejected. No credential was guessed, extracted or reset; the VM is
safely saved pending owner login. Shared evidence:
`/rv/tmp/samples/SAMPLES-DEC-007-Win7-SongProcessor/`.

Two owner inputs are deferred while the campaign continues at SAMPLE-065:

1. obtain authentic XNA-produced XNB/external-stream pairs for both Songs from a real Windows XNA
   4.0 environment (`SAMPLES-DEC-007`); and
2. decide the training-kit representation (`SAMPLES-DEC-005`): port Starter, Ex1 and Ex2 as three
   runnable teaching stages, or qualify the final Ex2 multi-platform endpoint while retaining the
   source/document delta as evidence. Silently calling the kit redundant is not an option.

Do not unblock this row by loading loose WAVs, hand-authoring Song XNBs, disabling music, collapsing
the platform branches, replacing Storage/Guide/debug behavior locally, or treating SAMPLE-063's
historical workaround port as the final endpoint. After the two decisions, port the selected scope,
repair framework gaps upstream, and run unchanged-XNA, native OPENGLES3 and real-Chrome WEBGL2
interaction/audio parity.
