# SAMPLE-014 — Spacewar_4_0 audit

## Result

The faithful C++ port is complete. The title, information and mode-selection screens, Retro and
Evolved games, ship upgrades, victory flow, scene graph, particles, custom effects, models,
cubemaps, render targets and XACT audio all use their original XNA 4.0 source behavior and content
contracts. Native OPENGLES3 and browser WEBGL2 builds run the port without a sample workaround.
No active missing XNA behavior was found in this audit.

## Original reference

- Exact source snapshot: `/rv/tmp/samples/SAMPLE-014-Spacewar_4_0/xna4-original/`.
- The unchanged Windows/x86 sources were compiled with the local XNA 4.0 assemblies by
  `scripts/build-original.sh`; the executable is
  `xna4-build/bin/SpaceWarWindows.exe` (SHA-256
  `36a345da4895cf6ffd8cbc582a475e02e43375401ee330d7420416269201385b`).
- The real original was run successfully for 40 seconds on the Linux desktop with
  `WINEPREFIX=/home/robertvokac/.wine-cna-xna40 WINEDLLOVERRIDES=d3d9=b wine SpaceWarWindows.exe`.
  Its identified 1280x720 `Spacewar` window is retained as
  `evidence/original-full-title.png` (SHA-256
  `1535a608901392f5c7c198d1d48bc0461ae444e78f04b21ee3733e7d37ce7957`).
- The port was reviewed against every original game source file. `camera.cs` maps to `Camera.*`;
  the other logical C# types retain the corresponding class/file decomposition. Only
  `Properties/AssemblyInfo.cs`, which contains assembly metadata rather than game behavior, has no
  C++ source counterpart.

## Content provenance

`scripts/build-content.sh` runs the retained Microsoft XNA 4.0 Content Pipeline and `XactBld3.exe`
against the exact original project. It produced 157 runtime files. A path-by-path and byte-by-byte
comparison with `samples/Spacewar/Content` found 157 matches and zero differences. This includes
the original Models, Shaders, textures, sprite fonts and XACT XGS/XSB/XWB products; there are no
`.model.json`, `.shader.json`, loose-image or other substitute sidecars.

Representative SHA-256 values are:

| Runtime asset | SHA-256 |
|---|---|
| `Audio/spacewar.xwb` | `2c5803458a9527e587bb9190dc27b957d67554a6e472d840d41a6d15a85b3043` |
| `Models/p1_pencil.xnb` | `e2daeae03faa95edbbd77ee1cebdf185caecb42861dbe54f8982b8bfe49cedf2` |
| `Shaders/ship.xnb` | `40ea0c53fd5fd290bc79bb9c2c32d3f976ba5f0d484d680aaaccda8ede1cb543` |
| `textures/Spacewar_Title_FINAL.xnb` | `7152779ddbaecf9925d39670b50818f17e16766943b0b1a243536f24ec3df17c` |

The historical `help.png` is preserved beside this file's sample `CMakeLists.txt`; it is not in
Content and is not loaded or displayed.

## Fidelity and workaround review

- Original identifiers, settings XML, timings, update/draw order, both rendering styles and XACT
  cue behavior remain in the translation.
- The original source itself performs procedural `SetData` calls in `EvolvedBackdrop`,
  `RetroStarfield`, `Shape` and `VectorShape`; those calls are retained and are not content
  substitutes.
- Mechanical scans found no `RawMesh`/`RawModel`, renderer helper, loose model/shader sidecar,
  help-overlay, invented input or omitted-mode bypass. The one CNA extension marker is the required
  runtime `GetTypeName()` plumbing, not graphics behavior.
- Player-one title controls are the original settings: `G` chooses Retro (B), `V` chooses Evolved
  (A), and `F` opens Info (X). In Chrome the canvas must have focus; Enter and Space are not title
  controls in the original sample.

## General CNA fixes exposed by this sample

The old placeholder incorrectly assumed models, compiled effects and XACT required per-sample
conversion. The faithful port instead exercised and repaired shared framework paths:

- XNB `Texture3D`/`TextureCube`, model/effect and case-insensitive content lookup, plus strong
  `ContentManager` cache semantics for texture resources;
- copy/value-wrapper reconstruction for cached `Texture2D` and `TextureCube` resources;
- XGS/XSB/XWB parsing and XACT category/variable/cue behavior needed by the original banks;
- XSB complex-track cursor semantics matching FACT: after parsing a complex sound, the shared
  sound-table cursor remains after its final event block. The previous parser restored the cursor
  to the track metadata, so Spacewar's first complex sound made 27 later sound offsets invalid;
  `title_music` and most effects therefore resolved to a deliberately silent cue. The general
  parser fix resolves `title_music` to original sound 19 / wave 24 (`Theme`) and `menu_music` to
  sound 20 / wave 25, without any sample-specific branch;
- EasyGL compiled-effect null sampler behavior matching FNA/FNA3D;
- stable EasyGL sampler resource ownership when a faithfully cached `TextureCube` value wrapper is
  replaced between frames.

The fixes are general XNA/FNA behavior and have focused regression tests. No code checks for
Spacewar and no dummy texture, load hoist or hand-written shader behavior was added.

## Verification

- Native: reusable build tree
  `/rv/tmp/samples/SAMPLE-014-Spacewar_4_0/cna-native-opengles3/`; executable
  `samples/Spacewar/Spacewar_cna_samples`. It was built with at most six jobs, selected
  `CNA_GRAPHICS_RENDERER=OPENGLES3`, loaded the original XGS/XWB/XSB, models and effects, rendered
  the title and Evolved game, accepted original input and exited normally. Captures and logs are in
  the artifact `evidence/` directory.
- The 2026-08-24 post-audit audio regression check rebuilt the same OPENGLES3 target and confirmed
  the title music on the real desktop/audio device; the owner heard it from the identified native
  `Spacewar_cna_samples` process while no Wine process was running. An additional SDL disk-audio
  run isolated CNA from every other desktop stream and produced
  `evidence/cna-native-title-music-isolated.wav`: 7.73 seconds of stereo 44.1-kHz PCM, mean volume
  -31.3 dB and peak -12.8 dB. The focused XACT parser/cue/bank/engine/category suites pass 280/280.
- Web: publishable bundle
  `/rv/tmp/samples/SAMPLE-014-Spacewar_4_0/cna-web-webgl2/samples/Spacewar/` containing HTML, JS,
  WASM and the preloaded 157-file content bundle. It was built with
  `CNA_GRAPHICS_RENDERER=WEBGL2`, served over local HTTP and tested in the system Google Chrome.
  After selecting Evolved with `V`, a 25-second observation remained alive, rendered moving ships,
  asteroids and sun, and advanced the score. Start/end captures have distinct SHA-256 values
  `c788402b8b7c0dc31c7009ec758d1161ba993a10bf7f390be4994a988adcfea1` and
  `c9d0d68c6cebfc53e17b166a57ef646720fc9af7562809fe2cd6699bb9f5f99a`.
- The final browser console contained only the expected EasyGL WEBGL2 and XACT load messages. Two
  HTTP 404 entries were both Chrome's automatic request for `/favicon.ico`; no game content was
  missing, so a sample-specific favicon was intentionally not invented.
- The same post-audit pass relinked the complete WEBGL2 bundle successfully against the corrected
  shared XACT parser. The previously recorded real-Chrome rendering/input gate remains valid; no
  renderer or sample source changed in this audio correction.
- Focused content/cache tests passed 19/19. The bounded EasyGL compiled-effect suite passed 30/31;
  its sole skip is the pre-existing pinned-MojoShader GLES `sampler3D` precision limitation, which
  Spacewar does not use. The SAMPLE-014 null-sampler and cube-wrapper lifetime regressions pass.

## Artifact root

All generated source snapshots, original pipeline/build output, reusable native and web build
trees, reproduction scripts, logs and captures are retained under:

`/rv/tmp/samples/SAMPLE-014-Spacewar_4_0/`
