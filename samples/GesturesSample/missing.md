# SAMPLE-079 — Touch Gestures audit

## Current-head requalification — 2026-09-27 — complete

The full 16-file upstream Phone project still matches `xna4-original/` byte for
byte. Both checked-in Phone/Reach XNBs match the unchanged official content
pipeline byte for byte, and each occurs once in the web `.data`. The original
`Game1.cs`, `Sprite.cs`, Phone entry point and project/content declarations were
compared with the port, including all six gestures, raw touch selection,
sprite order, color progression, drag, flick, pinch, friction and bounce.
The original license, background, icon and thumbnail were restored verbatim.

The earlier numeric RGBA palette, which sidestepped C++ translation-unit static
initialization, is replaced with a function-local array of the original named
`Color::White/Red/Blue/Green` values. It is initialized on first `Sprite` use
after game startup; palette order and color values are unchanged. No CNA or
Sharp Runtime source change, per-sample loader, renderer bypass or substitute
content was needed. `diff.md` records this C++ initialization choice and the
owner-requested desktop mouse input: one general CNA `TouchPanel` opt-in in the
constructor. Game logic still reads only `TouchPanel`; the mouse gives one
contact, while Pinch requires two genuine touches.

The current native and web scripts build from the active `libcna/cna-samples`
checkout with `../cna` at `5572f3ca1` and `../sharp-runtime` at `9e58c955`.
Release OPENGLES3 and nonthreaded Release WEBGL2 compiled successfully. The
WEBGL2 deliverable is exactly four files: HTML 19,621 B, JavaScript 261,225 B,
WASM 7,852,632 B and data 173,641 B. No SharedArrayBuffer, pthread or debug
section is embedded in the JS/WASM. The web build passed real system Chrome on
plain HTTP (`crossOriginIsolated=false`) with WebGL 2, true two-finger browser
touch events and 600 additional animation frames; there were no runtime,
console, rejection or HTTP errors.

The current Release OPENGLES3 run used a qualification-only SDL event adapter
under Xvfb to provide independent finger IDs. It verified Hold creation,
Tap/DoubleTap color changes, FreeDrag, Flick, two-finger Pinch, Hold removal
back to the exact original background, and clean SDL quit (`exit 0`). The
adapter is retained only in the artifact, never built into the sample. A
separate run without the adapter proved the owner-requested mouse bridge:
mouse Hold created the cat (28,511 changed pixels) and mouse Tap turned it red
(24,345 red-dominant pixels). The native first frame is byte-identical to the
retained XNA reference; the web first frame is pixel-identical to both at
800×480. The web semantic results include 28,071 changed pixels for Hold,
24,342 red-dominant pixels for Tap, 35,483 moved pixels for FreeDrag, 113,268
changed pixels for Pinch, and zero pixels after Hold removal.

The unchanged retained XNA executable previously ran under Wine and produced
`evidence/xna-original/01-baseline.png`. A fresh Wine attempt this session
failed before drawing with “No suitable graphics card found”; its dialog is
retained separately in `evidence/current-head-analysis-20260927/` and is not
used as a gameplay reference. The current source, XNBs and retained reference
are unchanged, and the native/web first-frame comparison is exact. Current
gesture behavior was additionally requalified directly in native and Chrome;
Wine has no touch digitizer to exercise the original Phone gestures.

The gallery contains the exact four tested web files, a real in-game cat
screenshot, a detail page and the 74th card. Its copied files passed a second
independent ordinary-HTTP Chrome gesture run with WebGL 2, all six gestures,
600 more frames and zero runtime/HTTP errors. Gallery pages contain
12/12/12/12/12/12/2 cards; all page totals and the previous/next links were
updated. Evidence is in
`/rv/tmp/samples/SAMPLE-079-GesturesSample_4_0/evidence/requal-20260927/`.
No artifact-root prune has been applied for this requalification.

## Historical evidence (September 2026)

The following original audit and captures are retained as history. Its old
statements about numeric color literals and no mouse opt-in are superseded by
the current-head section and `diff.md` above.

**Historical September 2026 status: complete — no known behavior or content differences from the XNA 4.0 original at that time.**

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

---

## Re-audited 2026-09-09: every claim above verified

Checked independently rather than re-read. All four original types have counterparts, both XNBs are
byte-identical to the official pipeline output, and the source carries no mouse path, no `F1` and no
help image.

**The pixel-identical claim holds, on all three products.** `01-baseline` against the XNA reference:

| product | RMSE |
| --- | --- |
| native Debug | **`0 (0)`** |
| native Release | **`0 (0)`** |
| browser WEBGL2 | **`0 (0)`** |

Bit-exact, including the browser. Nothing else in this campaign has matched real XNA exactly on all
three.

**The unresponsive reference is a control, and it works as one.**
`xna-original/02-after-pointer-hold.png` is byte-identical to `01-baseline.png` -- RMSE `0 (0)` --
which is what this document already says it is: proof that the reference does not substitute mouse
input for `TouchPanel`, so the gesture states genuinely cannot be captured from it and are not
silently compared against something that never moved.

**The sequence is internally coherent and deterministic.** `07-hold-remove` returns to
`01-baseline` at RMSE `0 (0)`, exactly as claimed -- the scene comes back bit-for-bit, not
approximately. Every step changes the frame (0.08 to 0.31, no zeros), so no gesture silently failed.
And `01`→`02` and `07`→`08`, which are the same Hold from the same state, produce the identical
delta to seven digits (`0.0817443`): the same gesture gives the same result, twice, in one run.

Nothing needed correcting.
