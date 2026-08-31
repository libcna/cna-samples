# SAMPLE-068 — Catapult Wars Training: All Features

## Result

This is the complete sixth exercise and combined endpoint of `CatapultWarsTrainingKit_4_0`. It is
an independent runnable product, matching its own upstream solution, `Program.cs` and content
project. The port combines the original one/two-human selection, health and damage zones,
pullback-derived angle, ballistic shot guide, supply crates, normal/split ammunition, weapon HUD
and split-projectile behavior without replacing the individual exercises.

The whole 469-file upstream directory is retained at
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. The unchanged C# Phone/Reach
project compiles with XNA 4.0; `CatapultGame.dll` has SHA-256
`a7498a49deb386972736b568f176f10aff17f7c696852dea865fd05a97cf0cbd`. Its diagnostic haptics shim
only supplies an unavailable Phone SDK assembly. There is no local Windows Phone 7 host, so no
visual execution of the original XAP is claimed.

## Content

The official XNA 4.0 pipeline built the unchanged original project. All 41 output files (39 XNBs
and two animation XML copy items) are checked in and recursively byte-identical to that retained
build. This includes the combined health, guide, crate and ammunition assets. The original tutorial
DOCX is byte-identical, SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`.

## Qualification

Native Debug and Release OPENGLES3 builds pass. The Release executable uses OpenGL ES 3.2 and
traverses menu, instructions, the two-human option, both turns/shots, combined HUD/guide behavior,
pause, return and clean exit. It finishes with status 0 and no console errors; evidence is under
`evidence/CatapultWarsTrainingAllFeatures/cna-native-opengles3-release-qualified/` in the artifact
root.

The threaded WEBGL2 product runs in real Chrome with a genuine WebGL 2 context. Browser touch
selects the two-human path, exercises both turns and pause/return, and completes 600 later animation
frames. The probe records zero page exceptions, unhandled rejections and HTTP errors. Evidence is
under `evidence/CatapultWarsTrainingAllFeatures/cna-web-webgl2-qualified/`.

No framework/runtime fix or sample workaround was required. `diff.md` describes only lossless
C++/AOT representations. There are no known active behavioral differences.
