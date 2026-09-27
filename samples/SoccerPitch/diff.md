# SAMPLE-073 intentional desktop input addition

## Owner-requested mouse input — 2026-09-27

The original Windows Phone game changes pitch-line rendering only when `TouchPanel` reports a
released touch. At the owner's request the constructor opts into CNA's off-by-default
`TouchPanel::setMouseTouchEmulationEnabledEXT(true)` extension. A left mouse click now supplies
the existing press and release events to `HandleTouchInput()`. It does not change the toggle
algorithm, rendering, GamePad Back exit or genuine touch behavior. The single opt-in line is marked
`CNAEXT`; there is no sample-local input workaround.
