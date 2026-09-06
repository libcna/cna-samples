# Differences from the XNA 4.0 original

## Mouse input is supported in addition to touch

**Requested by the project owner (2026-09-06).** The original is a Windows Phone 7 title whose
only input is touch: you tap the menu and you drag anywhere to aim and fire the catapult. On a
desktop the sample was therefore unplayable -- the menu did not respond to a click at all, and
there was no way to fire.

The port enables one CNA extension in its constructor:

```cpp
CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);
```

While that is on, CNA reports the **left mouse button as a touch**: pressing begins a touch at the
cursor, moving with it held reports a moved touch, and releasing ends it. So the game is played
with the mouse exactly as the original is played with a finger -- click `Play`, then press, drag to
aim and release to fire.

What this deliberately is **not**:

- It is **not a second input path in the game.** Every screen is a statement-for-statement
  translation of the original and reads only `TouchPanel::GetState()` and the gesture queue --
  `Tap` for the menus, `FreeDrag` and `DragComplete` for aiming and firing. None of them knows a
  mouse exists. The one added line is the opt-in, nothing else.
- It is **not a change to CNA's default behavior.** The emulation is off by default, because XNA
  and FNA both feed `TouchPanel` from real finger events only and neither synthesizes touches from
  a pointer. Every other sample, and this one with the line removed, behaves exactly as before.
- It does **not** displace real touch. The synthesized finger travels the same entry points a real
  one does, so `GetState()`, the gesture recognizer and `TouchPanelCapabilities` cannot tell them
  apart. The browser build is driven by real browser touch events in this sample's own
  verification and is unaffected by the opt-in.

Verified with a real pointer rather than injected events: clicking `Play` and then the
instructions advances to gameplay, a press-move-release drag raises the aiming arrow and the
`Release to Fire!` prompt (`FreeDrag`), and releasing fires the boulder (`DragComplete`).

The extension lives in `../cnanext` as `TouchPanel::getMouseTouchEmulationEnabledEXT()` /
`setMouseTouchEmulationEnabledEXT()`, implemented in the SDL input bridge; `samples/PathDrawing`
and `samples/NinjAcademy` are the precedents.

## Representation adaptations required by C++ and CNA

- C# `Type.GetType` plus `Activator.CreateInstance` restores screen types from isolated storage.
  C++ has no CLR reflection, so `ScreenManager` keeps an explicit registry for the same four
  concrete screen types. It stores the original fully-qualified names and creates the same types;
  serialized data and lifecycle behavior are unchanged.
- Concrete `System::Object`-derived classes expose CNA's `CNAEXT GetTypeName()` metadata. This is
  the established AOT counterpart used by the screen registry and does not add game behavior.
- `Program.cpp` supplies the native `main`, while `Properties/AssemblyInfo.cpp` retains the
  assembly title used for the game window. These replace CLR executable metadata only.

The desktop qualification harness is external evidence, not sample code. It feeds SDL touch
events so the native run exercises the original `TouchPanel` gesture path; the browser probe uses
real browser touch events. No mouse or keyboard gameplay branch was added to the game -- the
mouse support above is CNA reporting a pointer as a touch, not a branch in the sample.
