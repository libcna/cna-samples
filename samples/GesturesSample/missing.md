# SAMPLE-079 — Touch Gestures audit

**Status: complete — no known behavior or content differences from the XNA 4.0 original.**

The historical port was not an acceptable endpoint. It substituted loose PNG/font sidecars for
the original content, changed the content names, forced an invented desktop size while omitting
fullscreen, added a parallel mouse gesture implementation, added Escape/F1 behavior and loaded the
documentation image at runtime. All of those workarounds are removed.

Artifact root:
`/rv/tmp/samples/SAMPLE-079-GesturesSample_4_0/`

## Original surface audited

All 16 files in the Windows Phone XNA 4.0 product were retained and reviewed, including:

- `Game1.cs`, `Sprite.cs`, `Program.cs` and `Properties/AssemblyInfo.cs`;
- the Phone project, application manifests, solution and assembly metadata;
- the content project, `Font.spritefont` and `cat.tga`;
- the HTML documentation, documentation screenshot, icons and thumbnails.

The port restores the original `TouchGestureSample::Game1` and `TouchGestureSample::Sprite`
surface and preserves the original program flow. It requests fullscreen, uses the platform's
implicit presentation dimensions, runs at 30 Hz, enables exactly Hold/Tap/DoubleTap/FreeDrag/
Flick/Pinch, reads only `TouchPanel` plus GamePad Back, loads the original `cat` and `Font` content
names, and draws the exact helper text at `(10, 32)` over CornflowerBlue.

Raw primary-touch Pressed state still selects the topmost hit sprite, stops it and moves it to the
end of the draw list. Gesture behavior, hit-bound inflation, palette order, scale clamp, friction,
wall collision and bounce formulas are line-for-line equivalents of the C# source. C++ ownership
uses `unique_ptr`; public XNA value fields and property names otherwise remain represented directly.

`Sprite::Colors` uses explicit values for White, Red, Blue and Green. This is the same observable
palette as XNA, while avoiding C++ cross-translation-unit static initialization order between an
inline array and CNA's named `Color` objects. The earlier named-color initializer produced a
transparent first sprite in this executable; the explicit value initialization is the correct
value-type translation, not a runtime workaround.

There is no mouse/keyboard gesture emulation, manual TouchPanel display-size publication, Escape
path, F1 overlay, runtime help image, loose content path or sample-specific platform hook. The
repository's legacy documentation-only `help.png` stays beside `TouchGestureSample.htm` and is not
included in `Content/`; the HTML itself is byte-identical to the upstream document.

## Authentic content

The unchanged official XNA Game Studio 4.0 content pipeline built both Windows/Reach fixtures for
the desktop reference executable and Windows Phone/Reach fixtures for the authentic target. The
two checked-in XNBs are byte-identical to the retained Phone output:

| File | Bytes | SHA-256 |
|---|---:|---|
| `Font.xnb` | 21,678 | `939d4eb8ba9d2216055c52ea311086f5fb897a72d47e880e922f27ac394bae9b` |
| `cat.xnb` | 151,963 | `ff48be4d653c426a1995cce80b4d02a8ef537a9801fc493a2653c6be5fd4c914` |

`Content/` contains only those exact official-pipeline artifacts. The converted cat PNG, generated
font atlas PNG and font JSON sidecar are gone.

## Qualification

All CNA builds used `CCACHE_DIR=/rv/cnaccache` and no more than eight parallel jobs.

- The unchanged original `Game1.cs` and `Sprite.cs` compiled with the XNA 4.0 Windows assemblies
  and the official Windows/Reach content. It ran on WineD3D at 800x480 and rendered the exact
  initial helper screen. The Wine/X11 host has no digitizer; an ordinary pointer hold changed zero
  pixels, confirming that the reference does not substitute mouse input for `TouchPanel`.
- Debug and Release OPENGLES3 builds both ran on a real Mesa OpenGL ES 3.2 context. An external
  qualification-only SDL adapter inserted two genuine finger identities below CNA; it is retained
  only in the artifact and is not linked into or shipped with the sample.
- In both native builds, Hold created the 153x248 cat, Tap changed it to red, DoubleTap advanced the
  palette, FreeDrag moved it, Pinch expanded it from about 28,511 to 114,074 changed pixels, Hold
  removed it back to the exact baseline, and Flick produced subsequent motion. Debug and Release
  result metrics agree; both loaded the authentic XNBs through ordinary `Content.Load`.
- The complete Release WEBGL2 bundle ran in system Google Chrome. CDP delivered real browser touch
  start/move/end input with two simultaneous contacts for Pinch. Chrome obtained
  `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, exercised the same create/color/drag/pinch/remove/flick
  sequence, completed 600 additional `requestAnimationFrame` callbacks and reported no runtime
  exception, unhandled rejection, fatal console message or relevant HTTP error.
- Browser semantic metrics recorded 28,071 changed pixels for creation, 24,342 red-dominant pixels
  after Tap, 35,483 pixels changed by FreeDrag, 113,268 pixels after Pinch, an exact zero-pixel
  difference after Hold removal and 56,118 pixels changed by Flick relative to its starting frame.
- The XNA, native CNA and browser initial frames are pixel-identical at 800x480 (normalized RGB
  RMSE `0.0`). No CNA or Sharp Runtime change was required for this sample.

## Retained evidence

- Exact source snapshot and hashes: `xna4-original/`, `original-manifest.txt`,
  `original-sha256.txt`
- Official pipeline outputs and original executable: `xna4-build/`,
  `evidence/xna-content-sha256.txt`
- Original run: `evidence/xna-original/`
- Debug and Release native runs: `evidence/cna-native-opengles3/`,
  `evidence/cna-native-opengles3-release/`
- Real-browser result and ten captures: `evidence/cna-web-webgl2-qualified/`
- Reproducible original/native/web build and capture drivers: `scripts/`
