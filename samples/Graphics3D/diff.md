# Differences from the XNA 4.0 original

## Mouse input is supported in addition to touch

**Requested by the project owner** for SAMPLE-021 (`samples/PathDrawing/diff.md`), and applied here
for the same reason and by the same mechanism. This is the second sample to use it.

The original is a Windows Phone title whose only input is touch. Every one of its controls is a
touch control: the four on-screen checkboxes read `TouchPanel.GetState()` through
`Buttons/Clickable.cs`, and the camera is driven entirely by gestures —
`TouchPanel.EnabledGestures = FreeDrag | Pinch | PinchComplete`, with `FreeDrag` rotating the
spaceship and `Pinch` changing the field of view. On a desktop or in a browser without a touch
screen the sample renders but cannot be operated at all: no light can be toggled, the background
and animation cannot be switched on, and the model cannot be turned.

The port therefore enables one CNA extension in its constructor:

```cpp
CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);
```

While that is on, CNA reports the **left mouse button as a touch**: pressing begins a touch at the
cursor, moving with it held reports a moved touch, and releasing ends it. So the sample is used
with the mouse exactly as the original is used with a finger — tap a checkbox, drag across the
model to rotate it.

What this deliberately is **not**:

- It is **not a second input path in the game.** `Graphics3DSampleGame::HandleInput` and
  `Clickable::HandleInput` are statement-for-statement translations of the original and read only
  `TouchPanel::GetState()` and the gesture queue. Neither knows a mouse exists. The one added line
  is the opt-in, nothing else. In particular the previous (2026-07-09) port's substitute scheme —
  a hand-written mouse-position hit test per checkbox, a scroll wheel standing in for `Pinch`, and
  an added `Escape` exit the phone original has no keyboard for — is **gone**.
- It is **not a change to CNA's default behavior.** The emulation is off by default, because XNA and
  FNA both feed `TouchPanel` from real finger events only and neither synthesizes touches from a
  pointer. Every other sample, and this one with the line removed, behaves exactly as before.
- It does **not** displace real touch, and it does **not** fabricate the gestures. The synthesized
  finger travels the same entry points a real one does, so CNA's own `GestureDetector` produces the
  genuine `FreeDrag` and `Pinch` samples the sample reads; a real touch screen keeps working
  unchanged. Pinch needs two fingers and a pointer only has one, so the zoom control is reachable
  on a touch screen and not with a mouse — that is a property of the input device, not a gap in the
  port, and no substitute for it was invented.

The extension lives in `../cnanext` as `TouchPanel::getMouseTouchEmulationEnabledEXT()` /
`setMouseTouchEmulationEnabledEXT()`, implemented in the SDL input bridge and covered by ten tests
(`modules/input/tests/CNA/Internal/Input/SdlInputBridgeMouseTouchEmulationTests.cpp`).

The WEBGL2 verification drives it: `scripts/chrome-smoke.mjs` dispatches mouse events at the four
checkbox corners and across the model, and asserts that each tap changes the frame, that turning a
lamp off darkens the ship (159.82 → 136.97 mean luminance), that the background toggle covers the
sky, and that a drag rotates the model.

## The generated Windows entry point

Not a deviation from the original's behaviour, but a difference in what is compiled, recorded here
because it is visible in the build. The upstream `Program.cs` **does not compile**: inside
`#if WINDOWS || XBOX` it constructs a type named `Sample3DGraphics`, while the game class is
`Graphics3DSampleGame`. The project only ever built for Windows Phone, so that block was never
compiled; `Buttons/Button.cs` is the other half of the same abandoned rename and the `.csproj` does
not list it. The audit's XNA build links a generated entry point (`xna4-build/generated/Program.cs`)
the same way SAMPLE-021 does for a phone project that has none, and the port's `Program.cpp` is that
entry point in C++. `xna4-original/` is untouched. This is reproduced, not repaired — see
[`missing.md`](missing.md).

## Everything else

No other difference from the original. `missing.md` holds the complete audit, including the
measurement that every state the sample has — default, per-pixel lighting, starfield background and
two pinned animation frames — agrees with the real XNA 4.0 executable to 99.99 % of pixels within 8
levels and 100.00 % after a 4 px blur.
