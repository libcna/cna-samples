# SAMPLE-072 audit — Game State Management

## Result

No known behavioral difference from the selected XNA 4.0 Windows/Reach original remains. The
complete screen-manager architecture, screen transitions, keyboard/gamepad/touch input model,
multicast events, local content ownership, diagnostic tracing and all menu/gameplay flows are
present. The old sample-side substitutions and omissions have been removed.

Artifact root:

```text
/rv/tmp/samples/SAMPLE-072-GSMSample_4_0_WIN_XBOX
```

## Original source and execution

- `xna4-original/` is the exact 32-file upstream directory used for this audit. The retained
  sorted SHA-256 manifest is `evidence/original-sha256.txt`, whose digest is
  `0ea00ddfd0319ed7434e50f4df49d5a9cf684b227c18ddec82e161f977c2db5d`.
- The Windows and Xbox projects contain the same 15 C# source files. The selected runnable product
  is the Windows/Reach project at its declared 853x480 back buffer. Every source file, project,
  content declaration and asset was reviewed; the port retains the same logical 15-file class
  decomposition, split into declarations and implementations for C++.
- `scripts/build-original.sh` compiles the unchanged Windows source and all content through the
  XNA 4.0 compiler and official content pipeline in the isolated Wine prefix. It produces
  `xna4-build/windows-reach/GameStateManagementSample.exe` and all five declared XNBs.
- `scripts/capture-original.sh` runs that unchanged executable with WineD3D on isolated Xvfb.
  `evidence/xna4-original-windows-reach/` records main menu, Options, four changed option values,
  gameplay, movement, pause, quit confirmation, return to the main menu, exit confirmation and
  exit code 0. The original window title is `GameStateManagementSample`.

## Translation and content audit

- The former merged headers were replaced by the original `ScreenManager/` and `Screens/` type
  structure. `GameScreen`, `InputState`, `ScreenManager`, every menu/screen, `PlayerIndexEventArgs`
  and the game class preserve the original transitions, constants, update/draw order, controls
  and state ownership, including the inactive Windows Phone rule that disables selected-entry
  highlighting.
- `InputState` again reads `TouchPanel.GetState()` and drains gesture samples; `GameScreen` and
  `ScreenManager` again maintain the selected screen's `EnabledGestures`. Keyboard and all four
  gamepad slots remain the active controls used by this particular sample.
- `MenuEntry.Selected` and `MessageBoxScreen.Accepted`/`Cancelled` again use
  `System::EventHandler<PlayerIndexEventArgs>` rather than single `std::function` callbacks.
  `TraceEnabled` and `TraceScreens()` again route the original debug-only screen list through
  `System::Diagnostics::Debug`.
- `BackgroundScreen` and `GameplayScreen` again create their own `ContentManager` from
  `Game.Services` and unload it when removed. Gameplay restores the original one-second
  `System::Threading::Thread::Sleep`, elapsed-time reset, random enemy movement, controller
  disconnect pause path and independent screen-stack loading transition.
- The historical F1 overlay and all invented overlay state were removed. `help.png` is retained at
  the sample root beside `CMakeLists.txt`; it is not runtime content, loaded, copied or displayed.
- `Content/` now contains only `background.xnb`, `blank.xnb`, `gamefont.xnb`, `gradient.xnb` and
  `menufont.xnb`. They are byte-identical to the official pipeline outputs; their normalized
  manifest digest is `a54199f1250acfd315d60471dba24c6c958a0327b258f229dcec373157ec8d8a`.
  The loose PNG and font-JSON/atlas substitutes are gone, restoring the exact Segoe UI Mono
  regular/bold glyphs and original `Content.Load<T>()` paths.
- C++'s statically typed content route spells the original preload `Load<object>("gradient")` as
  `Load<Texture2D>("gradient")` and retains the returned value. The original `preloadAssets`
  array and loop remain intact. This loads and caches the same XNB object before transitions; it
  does not change or bypass the content contract.

## Qualification

All builds used `CCACHE_DIR=/rv/cnaccache` and no more than eight parallel jobs.

- Debug OPENGLES3: `cna-native-opengles3/` configures and builds. Its isolated runtime evidence in
  `evidence/cna-native-opengles3-qualified/` contains the same nine-state interaction sequence as
  the original, all five XNB loads and exit code 0.
- Release OPENGLES3: `cna-native-opengles3-release/` configures and builds. The same complete
  interaction sequence passes in `evidence/cna-native-opengles3-release-qualified/` with nine
  captures and exit code 0.
- The original reference was re-captured on 2026-09-08 and the port measured against it, replacing
  an earlier visual-only review. The previous reference was 853x447 rather than the sample's own
  853x480, and the reason recorded here -- "Wine's host-window presentation" -- was wrong. The Wine
  window is exactly 853x480; Wine centred it, and a centred 853x480 window on the 1280x720 Xvfb
  sits at y=273, so its bottom 33 rows were off the screen and `import -window` could only return
  the part that was on it. The reference was truncated, not squashed, and nothing that size can be
  compared with the port at all. `scripts/capture-original.sh` now moves the window to 0,0, uses a
  1280x1024 screen, and refuses rather than capture a window that does not fit.

- Against that corrected 853x480 reference, per step, RMSE native / release / web:

  | step | native | release | web |
  | --- | --- | --- | --- |
  | 01-main-menu | 0.0317 | 0.0315 | 0.0550 |
  | 02-options | 0.0372 | 0.0467 | 0.0712 |
  | 03-options-changed | 0.0285 | 0.0284 | 0.0561 |
  | 04-gameplay | 0.0773 | 0.0894 | 0.0820 |
  | 05-gameplay-moved | 0.0827 | 0.0885 | 0.0983 |
  | 06-pause | 0.0504 | 0.0562 | 0.0785 |
  | 07-quit-confirmation | 0.0144 | 0.0144 | 0.0722 |
  | 08-returned-main-menu | 0.0317 | 0.0289 | 0.0538 |
  | 09-exit-confirmation | 0.0138 | 0.0138 | 0.0691 |

  **The background is bit-exact.** Any region without text -- the top 40 rows, and the 140-row band
  under the menu on the main menu -- compares at RMSE `0 (0)` against real XNA: the gradient, the
  bubble sprites and their alpha blending are identical pixel for pixel. Every difference in the
  table is on glyphs, 0.3 % to 2.5 % of the frame, and a difference image shows nothing else.

  The two largest rows are time-dependent state rather than port error, and the original says so:
  `MenuEntry.cs:152` scales the selected entry by `sin(time * 6) + 1`, so "Play Game" is a
  different size in any two runs that are not phase-locked, and `GameplayScreen.cs` integrates
  `playerPosition` from held keys over elapsed time, so "Insert Gameplay Here" lands a few pixels
  apart after the same keypress. Both show in the difference image as one string doubled against
  itself; the unselected entries beside them show only a one-pixel antialiasing outline.
- WEBGL2: `cna-web-webgl2/` cleanly builds the complete Emscripten bundle with the five XNBs. The
  system Google Chrome test drives the same nine states with browser key events.
  `evidence/cna-web-webgl2-qualified/result.json` records a real
  `WebGL 2.0 (OpenGL ES 3.0 Chromium)` context, cross-origin isolation, the exact title, 600/600
  additional animation frames, and empty exception, HTTP-error and unhandled-rejection lists.
  The web console confirms every XNB load, including screen-local unload/reload behavior.
- The targeted no-workaround scan finds only the required `CNAEXT GetTypeName()` declarations.
  There are no raw content loaders, sidecars, direct renderer calls, invented input paths or help
  overlay references in runtime source.

No CNA or sharp-runtime source change was needed for SAMPLE-072, and no sample-side framework
workaround remains.
