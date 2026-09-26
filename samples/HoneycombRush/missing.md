# SAMPLE-063 — Honeycomb Rush completion evidence

## Status

**Historical completion on 2026-08-31; mouse and Firefox operation corrected
and qualified on 2026-09-05. Current-head requalification is in analysis (`🔎`,
2026-09-26).**
The selected upstream endpoint is
`Sources/EX2_PolishAndMenus/HoneycombRush/HoneycombRush`, the finished Windows Phone/Reach game.
The historical audit covered all 31 C# source units against the C++ port and
found the menu, instructions, gameplay, pause/resume, scoring, high-score,
win/loss, audio, storage and touch paths present. The touch-only phone
interface opts into CNA's owner-approved mouse-to-touch extension for desktop
and browser pointer operation. Current-head fidelity remains under review as
described below.

The original 230-file package, selected endpoint, build scripts and qualification products are
retained under `/rv/tmp/samples/SAMPLE-063-HoneycombRush_4_0/`.

## Current-head preflight — 2026-09-26

The physical 230-file upstream package still matches retained `xna4-original/`
byte for byte. The selected EX2 Windows Phone/Reach project has 31 C# source
units: 29 have corresponding port headers, while `Program` and `AssemblyInfo`
are port `.cpp` files. This is a source inventory, not a fresh line-by-line
current-head qualification. The native product's 53 content files match the
repository exactly. Of the retained Wine/XNA `Content-phone/` files, 45
non-Song XNBs and two XMLs match the repository; all four authentic Win7
SongProcessor XNB/WMA exports also match the repository. `Content-phone/`
itself remains a diagnostic output: it contains one loose
`InGameSong_Loop.wma` that differs from the official stream and no Song XNBs.
The retained `build-original.sh` can skip Wine's unsupported Song importer,
but does not yet restore the verified Win7 pairs into that output. Repair the
helper before claiming a fresh original content rebuild.

The retained native executable's `RUNPATH` points to the removed
`openeggbert/cnanext` checkout, and both CNA build helpers point to the
removed `openeggbert/cna-samples` checkout and old ccache location. The web
helper sets `CNA_ENABLE_EMSCRIPTEN_THREADS=ON`, whereas the current root CMake
option is `CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS`; the root forces the former
from the latter. The artifact `MANIFEST.md` repeats the old checkout paths.
The old threaded WebGL2 bundle and 2026-09-05 Chrome/Firefox
captures are historical evidence, not a test of current CNA `next`
`cefe6c83b` and SharpRuntime `next` `d86adb65`. Neither target was rebuilt or
run during this analysis pass.

Two current fidelity items need explicit requalification. First, both `.oga`
companions are 44,100 Hz stereo FLAC, while both authentic WMA streams are
48,000 Hz stereo and the original source WAVs are 48,000 Hz mono. The recorded
PCM MD5 values match only after decoding the WMA with `ffmpeg -ar 44100`:
`23af50f9a71b59a0d52dc6f0845db5df` (in-game) and
`871792e21fcd2ac462227724e46d7ba5` (menu). Direct native-rate WMA PCM
instead hashes to `41f18a8d5f78ba4c2e87423dca2bfa24` and
`bce44f4709fe4fe600ba78285d7ee093`, respectively. FLAC compression is
lossless, but the 48-to-44.1 kHz conversion is a real sample-rate change. The
next implementation pass should
generate companions from the unchanged official WMA at 48 kHz and verify
decoded PCM equality, as done for SAMPLE-062.

Second, the original `LoadingAndInstructionScreen` and `LevelOverScreen`
create background threads for `GameplayScreen.LoadAssets`. The native port
keeps both; the current `__EMSCRIPTEN__` branches load synchronously on the
WebGL context thread, introduced after Firefox hung during worker-side GL
resource creation. This is a documented historical browser adaptation, but
must be retested against current CNA to determine whether a general framework
solution now permits the original thread behavior. Do not remove it without
first reproducing and understanding the browser path.

There is no `HoneycombRush` bundle, card or detail page in
`../samples.libcna.com`. A newly qualified WEBGL2 build must also satisfy
the gallery's static-host requirements; the historical threaded bundle used
a COOP/COEP test server. Requalification therefore needs active-checkout build
scripts, official Song integration, native and real-browser gameplay/input/audio
and clean-exit tests, and a publishable gallery bundle. No game code or content
was changed during this preflight.

## Original and content evidence

The unchanged project contains 47 compiled content items and two `CopyToOutputDirectory` XML files.
The retained XNA 4.0 pipeline run proves all five Moire SpriteFonts, all 28 textures and all 12
SoundEffects. A diagnostic build also compiles all 31 unchanged C# units. The Phone project is not a
desktop XNA executable, so no desktop-original visual claim is made.

The offline Win7/XNA Game Studio pipeline completed the unchanged content project and supplied the
two SongProcessor products that Wine cannot encode. Its full logs and checksums are in
`/rv/tmp/samples/SAMPLES-DEC-007-Win7-SongProcessor/export/SAMPLE-063/`. The authentic pairs are:

| Song | XNB SHA-256 | WMA SHA-256 |
|---|---|---|
| `InGameSong_Loop` | `a94b2f08d352888da7a2f05f79f4456cce555184246a5154c4fd87a01a4da8bf` | `a79acf09d5e86de8f68bed691e5da8b1d3f8cda1048e63bf524a112f6f822914` |
| `MenuMusic_Loop` | `8d1527e6175b43ca3d8b2bc2d00186449436b958c89c174b3a480007880ccd3f` | `1a20571b2a3b0543bffa524c832b0a1a266d8483f7a56ee6dc089f6d3cfcfe2c` |

Both streams are stereo WMA v2 and 17,589 ms long. The port retains those original XNB/WMA pairs.
It additionally deploys Ogg-FLAC companions because CNA's native/browser media decoder does not
decode WMA. Decoding WMA **with an explicit resample to 44,100 Hz** and companion to PCM produces
the same MD5 for each song: `23af50f9a71b59a0d52dc6f0845db5df` (in-game) and
`871792e21fcd2ac462227724e46d7ba5` (menu). This changes no XNA asset identity or Song metadata.

The other 45 compiled items are official Wine/XNA pipeline outputs; the two Win7 Song XNBs bring
the compiled total to 47. `Configuration.xml` and `AnimationsDefinition.xml` are verbatim original
copy-output inputs. The original help image, `Honeycomb_Rush.doc` and Microsoft Permissive License
are retained at the sample root. The document SHA-256 is
`74ac56603e02810b594faf9f149e1179295b30f5a2a43aedd3713aa956350614`.

Removed historical substitutions include 28 loose PNGs, five DejaVu PNG/JSON font atlases, 14
loose WAVs, hard-coded configuration/animation tables and the synthetic HTML help page.

## Source fidelity

The port restores the original `HoneycombRush` game type and the exact fullscreen, 30 Hz timing,
content root, screen stack and component setup. It also restores:

- runtime `System.Xml.Linq.XDocument` parsing for both XML inputs;
- both original `System::Threading::Thread` background content loads on native targets, with the
  documented WebGL context-thread adaptation on Emscripten;
- `Guide::BeginShowKeyboardInput` / `EndShowKeyboardInput` for high-score names;
- `IsolatedStorageFile`, binary screen-state serialization and persistent high scores;
- genuine `TouchPanel` state and gestures, with CNA's off-by-default mouse-to-touch opt-in and no
  sample-local keyboard/mouse input path;
- all original animation, collision, bee generation, smoke, honey collection/deposit, score,
  countdown, difficulty, menu and transition behavior;
- original `Content.Load<SoundEffect/Song/Texture2D>` call shapes, including temporary managed-style
  value lifetimes now supported by CNA itself;
- original Song/MediaPlayer and SoundEffectInstance audio routes.

The remaining C++ language mechanics are recorded in `diff.md`; none replaces game behavior.
No cnanext or sharp-runtimenext change was needed while completing this row.

## Native OPENGLES3 qualification

Both Debug and Release configurations compile with at most eight parallel jobs. The final Release
artifact is `/rv/tmp/samples/SAMPLE-063-HoneycombRush_4_0/cna-native-opengles3/`.

The refreshed automated Xvfb run traverses title menu, instructions, real background-thread load,
gameplay, movement plus smoke, pause, resume and return to menu. Ordinary X11 mouse events enter
SDL as mouse input and CNA maps the left button through the unchanged `TouchPanel` path. No external
touch shim or `LD_PRELOAD` input bridge is used. The run records:

```text
renderer=OPENGLES3
full_flow=true
clean_exit=true
```

All seven refreshed 800x480 screenshots were inspected and show the expected distinct states,
authentic textures/fonts, animated bees, keeper movement and smoke. The captured stereo 44.1 kHz
audio is 49.041 s with mean volume -18.9 dB and maximum 0.0 dB. There is no fatal runtime/content
error. Evidence:
`/rv/tmp/samples/SAMPLE-063-HoneycombRush_4_0/evidence/cna-native-opengles3-mouse-touch-qualified/`.

## Real-browser WEBGL2 qualification

The clean Release Emscripten WEBGL2 build uses CNA's threaded browser runtime and at most eight
jobs. The two gameplay asset loads execute on the game thread that owns the WebGL context; native
builds retain the original background threads. The standard sample artifact needs no consumer
link override. System `/usr/bin/google-chrome` loads it from a local COOP/COEP HTTP server and
reports:

```text
WebGL 2.0 (OpenGL ES 3.0 Chromium)
crossOriginIsolated=true
requested rAF cycles=600
uncaught exceptions=0
unhandled promise rejections=0
HTTP/content failures=0
```

Sixteen actual DOM mouse events drive Start, instructions/loading, movement, smoke, pause and
resume through CNA's mouse-to-touch mapping. No CDP touch emulation is enabled. Six distinct
800x480 screenshots were inspected; gameplay, smoke and pause semantic assertions all pass.
The console confirms `CNA: graphics renderer: WEBGL2` and audio-mixer initialization; the only 404
is Chrome's unsolicited favicon request and is excluded from asset failures. This refreshed run
deliberately leaves the host's default audio device untouched and does not capture system audio.

Firefox 140.10.1 ESR is separately requalified against the same bundle. Ordinary mouse clicks move
from the title menu to instructions and then, after asset loading, to live gameplay; another mouse
press activates Smoke and produces its visible effect. Four inspected 1080x720 browser captures
record those states, every HTML/JS/Wasm/data request returns HTTP 200, and the browser process log
contains no fatal/runtime exception. This reproduces and closes the reported indefinite loading
failure without a touch shim or sample-local input path.

Evidence:
`/rv/tmp/samples/SAMPLE-063-HoneycombRush_4_0/evidence/cna-web-webgl2-chrome-firefox-fix/` and
`/rv/tmp/samples/SAMPLE-063-HoneycombRush_4_0/evidence/cna-web-webgl2-firefox-mouse-qualified/`.

## Historical assessment and current open items

The 2026-09-05 qualification recorded no remaining gap for the selected EX2
endpoint. The current-head preflight above identifies the stale build helpers,
44.1 kHz Song companions, WebGL loading branch and absent gallery as items to
resolve or verify before renewing that conclusion. The separately numbered
training kit remains governed by `SAMPLE-064` and `SAMPLES-DEC-005`; its three
teaching stages are not collapsed into this port.

## Pruned artifact inventory

The artifact root was pruned on 2026-09-05 from 337.3 MB to 160.2 MB. The retained canonical
products are the corrected post-Firefox-fix builds:

- `cna-native-opengles3/samples/HoneycombRush/`: the stripped native executable and runtime
  content;
- `cna-web-webgl2/samples/HoneycombRush/`: the verified HTML, JavaScript, Wasm and data bundle;
- `xna4-original/`, `xna4-build/Content-phone/`, `scripts/` and `evidence/`: the upstream snapshot,
  official Phone content output, reproducible helpers and qualification record.

The retained capture scripts contain no `pactl`, `parec`, `PULSE_SINK` or default-output changes.
The removed 177.1 MB was reproducible CMake/CNA/dependency scaffolding and unrelated sample target
directories. `MANIFEST.md` in the artifact root records the restoration commands.
