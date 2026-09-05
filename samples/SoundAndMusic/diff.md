# Differences from the XNA 4.0 original

## Mouse input is supported in addition to touch

**Requested by the project owner on 2026-09-05.** The original is a Windows Phone title whose
eleven controls read a single contact from `TouchPanel.GetState()`. On a desktop or in a browser
without a touch screen, the UI renders but cannot be operated with a pointer.

The port therefore enables one CNA extension in its constructor:

```cpp
CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);
```

While enabled, CNA reports the left mouse button as a touch: pressing begins a contact at the
cursor, moving with the button held moves that contact, and releasing ends it. Buttons and sliders
therefore receive exactly the same touch sequence as they do from a finger.

This deliberately does not add a second input path to the game. `SoundAndMusicSampleGame::Update`
continues to read only `TouchPanel::GetState()`, and the eleven original components continue to use
that single `TouchLocation`. No `Mouse` state, keyboard control or alternate UI behavior was added.
The extension is off by default in CNA and is enabled only by the marked line above, so it changes
no other sample. Real touch input continues through the same path unchanged.

The shared extension lives in `../cnanext` as
`TouchPanel::getMouseTouchEmulationEnabledEXT()` /
`setMouseTouchEmulationEnabledEXT()` and is covered by its focused SDL input-bridge tests.

## Everything else

No other active difference from the original is introduced by this decision. The authentic Song,
SoundEffect, content, timing, presentation and device/emulator volume contracts remain as recorded
in [`missing.md`](missing.md).
