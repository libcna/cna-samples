# SAMPLE-068 — Catapult Wars Training: Health Bar

## Result

This is the complete first exercise from `CatapultWarsTrainingKit_4_0`, retained as its own
runnable product because upstream gives it a separate solution, `Program.cs` and content project.
The port preserves the screen stack, human/AI turns, catapult/projectile state machines and the
exercise's three-zone damage calculation and animated health-bar display. No feature is borrowed
from a later exercise.

The entire 469-file upstream training kit is retained at
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. The unchanged C# Phone/Reach
project compiles with XNA 4.0; `CatapultGame.dll` has SHA-256
`30fd73cb0afe39eb80746d0455cc4c4e81f70f716b4b03e1227f7eb7963c6691`. A labelled diagnostic shim
supplies only the unavailable `Microsoft.Devices.Haptic` assembly. There is no local Windows Phone
7 application host, so no visual run of the original XAP is claimed.

## Content

The official XNA 4.0 pipeline built the unchanged original content project. All 34 resulting files
(33 XNBs plus `Textures/Catapults/AnimationsDef.xml`) are checked in, and `diff -qr` between this
`Content/` directory and the retained pipeline output is empty. The original tutorial DOCX is also
retained byte-identical (SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`).

## Qualification

Native Debug and Release builds pass with `CNA_GRAPHICS_RENDERER=OPENGLES3`. The Release run uses a
real OpenGL ES 3.2 context and completes
`menu -> instructions -> gameplay -> drag/fire -> damage/health update -> pause -> menu -> exit`
with exit code 0 and no console error. Captures and the run log are under
`evidence/CatapultWarsTrainingHealthBar/cna-native-opengles3-release-qualified/` in the artifact
root above.

The threaded WEBGL2 bundle runs in real Chrome on an actual WebGL 2 context. Browser touch reaches
gameplay, fires a shot, pauses and returns to the menu; a 600-requestAnimationFrame canary then
reports zero exceptions, unhandled promise rejections and HTTP errors. Evidence is under
`evidence/CatapultWarsTrainingHealthBar/cna-web-webgl2-qualified/`.

No CNA or sharp-runtime workaround is present or was required. `diff.md` records only necessary
C++/AOT representation changes. There are no known active behavioral differences.
