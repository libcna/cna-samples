# SAMPLE-069 audit — Cards Starter Kit

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

No CNA or sharp-runtime source change was needed for SAMPLE-069, and no sample-side framework
workaround remains.
