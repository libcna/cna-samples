# SAMPLE-069 audit — Cards Starter Kit

## Current-head qualification — 2026-09-26

**Status: `✅`.** The unchanged Windows/HiDef XNA source was rebuilt with the
official XNA 4.0 pipeline and played under WineD3D on private Xvfb `:270`:
menu, Play, $25 bet, Deal, resolved hand, pause, return and clean Exit. The
fresh original captures are in `evidence/requal-20260926/original/` under the
artifact root below. Original and CNA hands are random, so the comparison is
of the table/card/chip layout and game transitions, not pixel equality.

The 247-file physical upstream directory still matches `xna4-original/` byte
for byte. The previously completed line-by-line 47-file game/framework audit
remains valid: the port source has not changed since 2026-08-31. A renewed
scan of every port source hit found only the C++ AOT/type-name mechanics in
[`diff.md`](diff.md), not a sample-side renderer, loader or audio workaround.
The fresh official build's 89 XNB files are byte-identical to both the checked-in
`Content/` and the new native product. For reproducible tree digests, sort all
regular files by relative POSIX path and feed SHA-256 with, for each file,
`UTF8(relative_path) + NUL + raw_SHA256(file_bytes) + LF`. This gives
`accfd5ed82e6ffbf3f0c6f69e74dd7d31e6f252d0961b7d176e262f5f1012787`
for 247 upstream files and
`45e93de61cbfb295d06e94bc67b3a5eaf0b8e37f76fbe0ee923edd1cfc906317`
for each of the three 89-XNB trees. The older digest numbers below lacked a
recorded algorithm and are retained only as historical notes.

Both port builds used the active `cna/next 8c917a6d` and
`sharp-runtime/next d86adb65`, Release, the shared
`~/.cache/ccache` with `CCACHE_BASEDIR=/rv`, and the single CardsStarterKit
target. The rebuilt OPENGLES3 binary resolves its colocated `libcna.so` via
`$ORIGIN` and active CNA SDL libraries. Its private-Xvfb `:269` capture passed
Play → bet → Deal → result → Escape → Quit Game → menu Exit, code 0, with no
fatal log entry (`evidence/requal-20260926/native/`).

The non-threaded WEBGL2 WASM is **8,304,181 bytes** with no custom/name/DWARF
sections or pthread markers; its `.data` is 22,848,762 bytes. Actual Chrome
played the same route, including Theme Red→Blue, 600 further animation frames,
WebGL 2 and no exception, rejection or HTTP failure. Firefox 140 ESR independently
reached betting, a dealt and resolved hand, 600 frames, WebGL 2, with
`moduleAbort: null` and no page error, on ordinary non-isolated HTTP. Native
and Chrome gameplay both emitted measured stereo sound during the bet/deal
route: 44.1 kHz, maxima **−3.5 dB** and **−3.4 dB** respectively. The first
native monitor capture was silent because PipeWire left the game's sink input
on the desktop sink; `scripts/capture-audio-current.sh` now explicitly moves
that input to its private sink. This was a harness routing issue, not a game
change. Evidence: `evidence/requal-20260926/{chrome,firefox,native-audio,web-audio}/`.

The gallery now has a Cards Starter Kit gameplay card, detail page and exact
four-file WEBGL2 bundle. The image is the Chrome `web-result.png` gameplay
frame, not a menu. All four gallery bundle hashes match the build products.
The staged gallery copy passed a separate real-Chrome full gameplay/theme/
pause/exit run on plain static HTTP: all page, image and bundle requests were
HTTP 200, WebGL 2, 600 frames and no browser error. Evidence:
`evidence/requal-20260926/gallery/`. No CNA or sharp-runtime source change
and no sample workaround was needed. Current build/capture commands, product
hashes and prior prune history are in artifact `MANIFEST.md`. The build trees
remain intact; no new prune or push was performed.

## Previous current-head analysis — 2026-09-26

**Status: `🔎`.** Cards Starter Kit is one runnable Windows/HiDef Blackjack
game with its own reusable `CardsFramework`, not another Catapult Wars
training stage. The physical 247-file upstream tree still matches the
retained `xna4-original/` snapshot (`diff -qr` is empty). The 47 C# game and
framework units have no port-source change after `ef49afb` on 2026-08-31.
All 89 checked-in XNBs remain byte-identical to the retained official XNA
content build and to the retained native product. A targeted scan found no
`NOXNA`, renderer/backend, raw-loader or sample-side workaround path; the
`CNAEXT` occurrences are the AOT/type-name mechanics recorded in `diff.md`.

The original Windows executable, its WineD3D gameplay captures, and the
native/Chrome/Firefox qualifications below are valuable historical evidence.
The retained original and native result captures show the same table, chip
and card layout; their random hands differ, so no pixel-equality claim is
made. These captures
are not a run on the active `cna/next 8c917a6d7` and
`sharp-runtime/next d86adb65` heads. The retained native binary dates from
2026-09-09 and names retired `openeggbert/cnanext` in its RUNPATH;
`scripts/capture-native.sh` defaults to a release build-tree name that the
pruned root no longer has. The artifact `MANIFEST.md` also points its rebuild
commands at retired `openeggbert/cna-samples`.

The WEBGL2 bundle dates from 2026-08-31: its WASM is **101,149,231 bytes**,
including **69,086,886 bytes** of name/DWARF sections. The 2026-09-07 Firefox
run establishes that its non-threaded game avoids the specific SAMPLE-067
background-loader defects, but does not establish current-head parity or an
efficient gallery bundle. The website has no Cards Starter Kit entry.

To restore `✅`, repair the artifact reproduction commands, rebuild native
OPENGLES3 and lean WEBGL2 products against the active chain, repeat the
original/native/real-Chrome and Firefox gameplay path (Play, bet, Deal,
Stand/result, pause/return/exit), check the Theme Red→Blue and four-sound
paths, and publish a real gameplay capture and bundle in the gallery. Record
a reproducible command for any renewed tree digest; the two old digest
numbers below were already flagged as lacking their producing command. No
game, runtime or artifact was changed in this analysis.

## Result

No known behavioral differences from the selected XNA 4.0 Windows/HiDef original remain. The
port contains the complete game, reusable cards framework, Blackjack AI/rules, screen manager,
audio and inactive Windows Phone/Xbox branches. The unavoidable C#-to-C++ language mechanics are
listed separately in [`diff.md`](diff.md); none adds, removes or bypasses game behavior.

Artifact root:

```text
/rv/tmp/samples/SAMPLE-069-CardsStarterKit_4_0
```

## Original source and execution

- `xna4-original/` is the exact 247-file upstream directory selected for this audit. Its sorted
  file-list/content digest is
  `2d955aec70641af9ff813450b4d86c63822a22038871519efb6405bf5e3c7a8c`.
- The selected product is `CardsGame/CardsGame` with `BlackjackHiDefContent`, compiled with
  `DEBUG;TRACE;WINDOWS`. All 47 C# files in the game and `CardsFramework` were reviewed against
  the C++ translation; phone and Xbox conditional branches were reviewed as part of those files.
- `scripts/build-original.sh` builds the unchanged source with the XNA 4.0 C# compiler and the
  official content pipeline in the isolated XNA Wine prefix. The retained output is
  `xna4-build/windows-hidef/Blackjack.exe` plus `CardsFramework.dll` and `Content/`.
- The original was run with
  `CNA_XNA40_WINEPREFIX=/home/robertvokac/.wine-cna-xna40` and WineD3D
  (`WINEDLLOVERRIDES=d3d9=b`) on an isolated Xvfb display. Screenshots under
  `evidence/xna4-original-windows-hidef/qualified/` cover the main menu, Play transition,
  betting, chip placement, Deal, a resolved hand, pause and return to the root menu.

## Translation and content audit

- The old partial desktop-only translation was replaced by the full original surface. Restored
  items include `CardPacket.Remove()`, all card/hand events, every rule and AI branch, the original
  event sender identities, `PlayerIndexEventArgs`, gesture properties, Xbox/phone menu paths and
  hit bounds, phone touch input, screen-stack serialization, every audio overload, button/bet
  touch paths, platform-specific `BlackjackGame` setup, and the original otherwise-unreachable
  `InstructionScreen` and music methods.
- Original spelling, casing and quirks are preserved, including `UIUtilty`,
  `PerformBeforSartArgs`, `backButton_Click`, `shuffle_` and `Images/youlose`. CNA's general XNA
  content-path behavior resolves the original Windows-style case-insensitive requests; the sample
  has no casing workaround.
- `Content/` contains all 89 XNB files produced by the unchanged official HiDef content project.
  Every file compares byte-for-byte with `xna4-build/windows-hidef/Content`; both sorted SHA-256
  manifests reduce to
  `a62136d620199613ea7d0f51ad32d2025d551c61587ff5868af8f589f2f673c8`.
- The historical loose PNG/font-JSON/WAV substitutes were removed. `help.png` is retained beside
  `CMakeLists.txt` and is not loaded. `Microsoft_Permissive_License.rtf` is also retained exactly;
  its SHA-256 is `f321791f1033118625d5fd030a823544a396c9bafe167ec02f81b3516d3f1c8a`.
- The complete port builds with both GCC/libstdc++ and Emscripten/libc++. The final static
  `AudioManager` owner is defined only after the class is complete, avoiding a libc++
  incomplete-type rejection without changing its singleton lifetime.

## CNA qualification

All compilation commands used `CCACHE_DIR=/rv/cnaccache` and at most eight parallel jobs.

- Debug OPENGLES3: `cna-native-opengles3/`; configure and target build pass. The final executable
  completed the full isolated run below and exited with code 0.
- Release OPENGLES3: `cna-native-opengles3-release/`; configure and target build pass. On Xvfb
  `:175`, the final executable completed Play → bet $25 → Deal → Stand → resolved results → Escape
  pause → Quit to menu → Exit. Screenshots and the clean log are retained in
  `evidence/cna-native-opengles3-release-qualified/`.
- A second complete Debug runtime run on Xvfb `:173` passed with the same transitions and clean
  exit; evidence is in `evidence/cna-native-opengles3-qualified/`.
- WEBGL2: `cna-web-webgl2/` cleanly configures and produces the complete Emscripten bundle. The
  system Google Chrome test drives the actual canvas through the same bet/deal/stand/result path,
  pauses and returns to the menu, then opens Theme and switches the card back from Red to Blue.
  `evidence/cna-web-webgl2-qualified/result.json` records real
  `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, cross-origin isolation, 600/600 additional animation-frame
  callbacks, and empty exception, HTTP-error and unhandled-rejection lists. The console confirms
  the XNB textures/fonts and all four original sound effects loaded.

## Firefox verification, 2026-09-07

Added after SAMPLE-067 found two defects that the campaign's Chrome-only browser evidence could
not have seen: Emscripten 6.0.3's mailbox assertion, which aborts in a browser without
`Atomics.waitAsync` (Firefox before 145), and an unserialized loader/frame GL path that emptied
background-loaded textures. Both need a **background loading thread**, and this sample creates
none -- there is no `System::Threading::Thread`, `std::thread` or `std::async` anywhere in `src/`,
including its own `GameStateManagement` copy, which has no `LoadingScreen`. That is a reason to
expect immunity, not evidence of it, so it was measured.

The **unchanged 2026-08-31 bundle** -- the one linked by Emscripten 6.0.3, against the renderer
before `cnanext cde325ecd` -- was run in Firefox 140.10.1esr, headed on a private Xvfb display and
driven over WebDriver BiDi (Firefox 140 starts BiDi rather than CDP, and its CDP shim has no input
command). `Module.onAbort` was installed before any interaction, so a bare `Aborted(...)` could
not pass unnoticed.

It plays: menu, `Enter` for Play, the betting table, a placed chip, Deal, a dealt hand, Stand and
a resolved result with the balance updated, then 600 further animation frames. Recorded in
`evidence/firefox-stale-bundle-run2/result.json`: `moduleAbort: null`, no page error, no
`Aborted(`, `crossOriginIsolated: true`, WebGL 2, canvas 800x480 and `atomicsWaitAsync:
"undefined"` -- that is, the passing run is on precisely the browser configuration that aborts
SAMPLE-067 and every SAMPLE-068 product. **This sample therefore needs no rebuild on the fixed
toolchain for correctness**, unlike those two.

Harness: `scripts/capture-web-firefox.sh` + `scripts/firefox-probe.mjs` in the artifact root. The
first attempt reported a frozen canvas; that was the harness clicking invented coordinates for
Play, which the original selects with `Enter` as the retained Chrome gate does. The corrected run
is the one recorded above.

Three of the record's claims were also re-checked independently on this date: `Content/` holds 89
XNBs and hashes identically to `xna4-build/windows-hidef/Content`; the upstream snapshot is 247
files; and the port still compiles and links against the current `cnanext`/`sharp-runtimenext`
with no error. The two digests quoted earlier in this file could not be reproduced, because the
command that produced them is not recorded -- the claims behind them verify by other means, but
the numbers themselves are not checkable as written.

No CNA or sharp-runtime source change was needed for SAMPLE-069, and no sample-side framework
workaround remains.
