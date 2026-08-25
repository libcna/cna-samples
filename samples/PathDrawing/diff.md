# Differences from the XNA 4.0 original

## Mouse input is supported in addition to touch

**Requested by the project owner.** The original is a Windows Phone 7 title whose only
input is touch: you drag a path from the tank with a finger, and nothing else does
anything. On a desktop or in a browser without a touch screen that makes the sample
unplayable — there is no way to give the tank a path at all.

The port therefore enables one CNA extension in its constructor:

```cpp
CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);
```

While that is on, CNA reports the **left mouse button as a touch**: pressing begins a
touch at the cursor, moving with it held reports a moved touch, and releasing ends it.
So the sample is played with the mouse exactly as the original is played with a finger —
press on the tank, drag the path you want, release.

What this deliberately is **not**:

- It is **not a second input path in the game.** `PathDrawingGame::Update` is a
  statement-for-statement translation of the original and reads only
  `TouchPanel::GetState()` and the `FreeDrag` gesture queue. It has no idea a mouse
  exists. The one added line is the opt-in, nothing else.
- It is **not a change to CNA's default behavior.** The emulation is off by default,
  because XNA and FNA both feed `TouchPanel` from real finger events only and neither
  synthesizes touches from a pointer (`SDL2_FNAPlatform.cs`, `SDL3_FNAPlatform.cs`).
  Every other sample, and this one with the line removed, behaves exactly as before.
- It does **not** displace real touch. The synthesized finger travels the same two entry
  points a real one does, so `GetState()`, the gesture recognizer and
  `TouchPanelCapabilities` cannot tell them apart, and a genuine touch screen keeps
  working unchanged. The browser build is driven by real touch events in this sample's
  own verification and is unaffected by the opt-in.

The extension lives in `../cnanext` as
`TouchPanel::getMouseTouchEmulationEnabledEXT()` / `setMouseTouchEmulationEnabledEXT()`,
implemented in the SDL input bridge and covered by ten tests
(`modules/input/tests/CNA/Internal/Input/SdlInputBridgeMouseTouchEmulationTests.cpp`),
including one proving a pointer drag produces the `FreeDrag` gesture this sample needs.

## Everything else

No other active difference from the original. `missing.md` holds the complete audit,
including the measurement that the native OPENGLES3 frame is byte-identical to the XNA
original's, 384000 of 384000 pixels.

The previous port diverged from the original in five documented ways — invented mouse
controls wired directly into the game loop, an omitted `SpriteFont`/`DrawString`, manual
ground tiling instead of `LinearWrap`, a reversed tank/path draw order, and dropped 30 Hz
and fullscreen settings. All five are resolved; four of their stated root causes were
claims about CNA's Vulkan backend that do not hold on this campaign's EasyGL renderer.
Note that the mouse support described above replaces none of that: it sits under the XNA
API rather than inside the game.
