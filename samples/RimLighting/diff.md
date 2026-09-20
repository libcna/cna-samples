# Differences from the XNA 4.0 original

## Mouse input in addition to touch

**Explicitly approved by the project owner for SAMPLE-037.** The original
Windows Phone game reads only `TouchPanel`; a desktop mouse does not control
its sliders, arcball or mode button. The earlier CNA OPENGLES3 executable
already enabled mouse-as-touch; its removal during the 2026-09-20 audit
temporarily regressed desktop control. The owner approved restoring it to
make both the native build and gallery usable without a touchscreen. The port
enables CNA's existing opt-in in the game constructor:

```cpp
CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);
```

While enabled, holding the left mouse button produces the same pressed, moved
and released touch states as a finger. The original game logic remains a
translation of its `TouchPanel::GetState()` path; there is no separate mouse
branch, synthetic keyboard control or changed effect behavior in the sample.

The extension is **off by default in CNA**. Other games remain untouched, and
removing this one marked line restores the original touch-only behavior. Real
touch input remains supported: mouse emulation adds a way to supply a touch,
not a replacement for a touchscreen. The shared mechanism and its tests live
in the sibling `cna` repository, where SAMPLE-021 introduced it; this task changes no CNA or
sharp-runtime code.

The `missing.md` audit records native mouse and browser mouse/touch tests for
this owner-approved behavior. Other differences are lossless C#-to-C++
mechanics documented there.
