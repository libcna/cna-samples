# SAMPLE-063 — Honeycomb Rush completion evidence

## Status and scope

**Complete on 2026-09-26 on cna-samples `develop`, CNA `next` `cefe6c83b`, and
SharpRuntime `next` `d86adb65`.** The selected endpoint is the finished
`Sources/EX2_PolishAndMenus/HoneycombRush/HoneycombRush` Windows Phone/Reach game.
The separately numbered training kit remains governed by `SAMPLE-064`; its
three stages are not folded into this port.

The physical 230-file upstream package matches retained `xna4-original/` byte
for byte. The endpoint has 31 C# source units. Twenty-nine map to port headers;
`Program` and `AssemblyInfo` map to `.cpp` files. The source and content audit
retains the original title/menu, instructions, gameplay, pause/resume,
difficulties, honey collection/deposit, bees, smoke, scoring, high scores,
storage, touch, music and effects. `diff.md` records the necessary C++ language
adaptations and the owner-approved, off-by-default CNA mouse-to-touch opt-in.
There is no sample-local replacement input path or remaining sample workaround.

Artifact root: `/rv/tmp/samples/SAMPLE-063-HoneycombRush_4_0/`. Rebuild and
capture helpers are in its `scripts/`; fresh logs, hashes, audio and screenshots
are in `evidence/requal-20260926/`. The prior 2026-09-05 results remain in
separate historical evidence directories.

## Original XNA and content

`scripts/build-original.sh` builds the unchanged selected project using the
official XNA 4.0 Windows Phone/Reach content pipeline and compiles all 31
original C# units into the diagnostic Phone `xna4-build/HoneycombRush.dll`.
Wine cannot run XNA's WMA Song importer, so the helper verifies SHA-256 for the
four offline Win7/XNA SongProcessor exports, skips only those two pipeline
items under Wine, and copies the verified products into `Content-phone/`.
The offline export is in
`/rv/tmp/samples/SAMPLES-DEC-007-Win7-SongProcessor/export/SAMPLE-063/`.

| Song | Official XNB SHA-256 | Official WMA SHA-256 |
|---|---|---|
| `InGameSong_Loop` | `a94b2f08d352888da7a2f05f79f4456cce555184246a5154c4fd87a01a4da8bf` | `a79acf09d5e86de8f68bed691e5da8b1d3f8cda1048e63bf524a112f6f822914` |
| `MenuMusic_Loop` | `8d1527e6175b43ca3d8b2bc2d00186449436b958c89c174b3a480007880ccd3f` | `1a20571b2a3b0543bffa524c832b0a1a266d8483f7a56ee6dc089f6d3cfcfe2c` |

The rebuilt `Content-phone/` has 47 official XNBs, two original XML copy-output
files and two authentic WMA streams: all 51 files match the repository content
byte for byte. The repository and fresh native product have 53 files, adding
only two portable Ogg-FLAC companions. Both are encoded at the WMA streams'
original 48 kHz stereo rate, with no resampling. Direct decoding to signed
16-bit PCM produces the same byte count and SHA-256 for each pair:

| Song | WMA and Ogg-FLAC decoded PCM SHA-256 |
|---|---|
| `InGameSong_Loop` | `ece096b83c5031c1c88e5fc5ec8d3b97fde571b27687aaf17ecd3aab7914a85a` |
| `MenuMusic_Loop` | `bf49ca0d740b01848f3eb476dfaae8b165ca3bfd7404803301e48e43a671a7ee` |

`evidence/requal-20260926/audio-verification.json` records stream metadata,
file hashes, decoded byte counts and both PCM hashes. Game code still loads the
authentic Song XNBs. The original document, help image and Microsoft
Permissive License remain in the upstream snapshot. The original Phone DLL
is not a desktop XNA executable, so this Linux host cannot run it as a visual
reference; no direct pixel or live XNA audio comparison is claimed.

## Thread and runtime fidelity

The original `LoadingAndInstructionScreen` and `LevelOverScreen` each start a
background `System.Threading.Thread` to execute `GameplayScreen.LoadAssets`.
The port now does this on both native and Emscripten. The old Emscripten-only
synchronous `LoadAssets()` branches and their `assetsLoaded_` flags were
removed. Current CNA EasyGL's general renderer-thread context lease and WebGL
proxying handle graphics creation on the loading thread; no CNA or SharpRuntime
source change was required. The WEBGL2 build enables pthreads through the
current `CNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=ON` option.

The game's remaining C++-specific differences are explained in `diff.md`:
compile-time screen type registration for serialized CLR names, deferred
screen destruction across a frame boundary, managed-object ownership in
callbacks, header colocation, and the faithful portable Song companions.
The sole user-facing extension is the previously approved CNA
`TouchPanel::setMouseTouchEmulationEnabledEXT(true)` for this touch-only Phone
game on desktop and browser. Normal left mouse input still travels through
the original touch state and gesture pipeline.

## Fresh native OPENGLES3 qualification

`scripts/build-cna-native.sh` builds Release against the active libcna CNA and
SharpRuntime checkouts with two compile jobs. It places `libcna.so` beside the
native executable so the retained product works after build-tree pruning; its
`RUNPATH` resolves that library from `$ORIGIN`. The product's 53 `Content/`
files exactly match the repository.

The private Xvfb run using `scripts/capture-cna-audio.sh native` traversed menu,
instructions, real background loading, gameplay, movement, smoke, pause,
resume, menu return and clean exit. Seven inspected 800×480 screenshots show
the distinct states, authentic art and live bees. Ordinary X11 mouse input
uses CNA's touch mapping. The second run captured 48.67 seconds of 44.1 kHz
stereo output through a private null sink, with mean −18.6 dB and peak 0.0 dB;
the host's default sink was untouched. `renderer=OPENGLES3`,
`full_flow=true`, and `clean_exit=true` appear in
`evidence/requal-20260926/native-audio/result.txt`. There is no fatal
content/runtime error. Xvfb logged a nonfatal fullscreen mode-switch timeout
and used the normal 800×480 window.

## Fresh WEBGL2 and gallery qualification

`scripts/build-cna-web.sh` builds a threaded Release WEBGL2 product from the
same source with two jobs. The real system Chrome run using
`scripts/capture-cna-audio.sh web` loaded the 800×480 game on WebGL 2,
traversed menu, instructions, background loading, gameplay, movement, smoke,
pause and resume through ordinary DOM mouse events, and completed 600 animation
frames. `crossOriginIsolated=true`; uncaught exceptions, unhandled rejections,
content HTTP failures and fatal console messages are all zero. Six inspected
canvas screenshots show the states. The private browser capture contains
102.91 seconds of 44.1 kHz stereo audio, mean −19.0 dB and peak 0.0 dB.
Evidence: `evidence/requal-20260926/web/`.

Firefox 140.15.0esr independently reached active gameplay through the original
loading-thread path and rendered a visible smoke effect after a real pointer
press. Four inspected browser captures, all four bundle requests returning
HTTP 200 and the absence of fatal runtime messages are recorded in
`evidence/requal-20260926/web-firefox/`. The first Firefox attempt used a
port already occupied by another process; it did not reach the game. The
successful run used an unused port.

The gallery at `../samples.libcna.com` now has the exact Release four-file
bundle, a scoped COOP/COEP service worker and launcher, an active-gameplay
screenshot, a thumbnail, detail page and the 61st card. The exact staged
six-file bundle was tested through `HoneycombRush/launch.html` over a plain
Python HTTP server in a clean Chrome profile. The service worker gave
`crossOriginIsolated=true`; gameplay, movement, smoke and pause rendered,
600 frames completed and there were zero runtime, rejection or content HTTP
failures. The first static test's CDP forced reload bypassed the worker; the
test helper now preserves it on plain HTTP. The successful result and six
canvas captures are in `evidence/requal-20260926/gallery-plain-chrome/`.

## Remaining difference and artifact cleanup

No active game-code workaround or known native/WEBGL2 behavioral gap remains
for the selected EX2 Phone/Reach endpoint. Direct Phone-original visual and
audio comparison is unavailable on this Linux host; the unchanged Phone DLL,
exact official content, source audit and interactive port runs are the
available evidence. The artifact root has been rebuilt and **has not been
pruned in this pass**. `MANIFEST.md` records the current rebuild route; pruning
requires the owner's separate instruction.
