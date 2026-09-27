# SAMPLE-107 intentional desktop input addition

## Owner-requested mouse input — 2026-09-27

The original Windows Phone game recalibrates its reference down vector while a touch is held.
At the owner's request the constructor opts into CNA's off-by-default
`TouchPanel::setMouseTouchEmulationEnabledEXT(true)` extension. Holding the left mouse button
feeds the existing `TouchPanel::GetState()` path. It does not add a separate mouse handler,
change the accelerometer or emulator algorithm, change GamePad Back exit or replace actual touch
input. The single opt-in line is marked `CNAEXT`.
