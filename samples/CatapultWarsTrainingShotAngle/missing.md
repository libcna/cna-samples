# SAMPLE-068 — Catapult Wars Training: Shot Angle

## Result

This is the complete third exercise from `CatapultWarsTrainingKit_4_0`, with its own target because
the original has an independent solution, entry point and content project. It retains the common
game plus the exercise's minimum/maximum shot velocity, pullback-derived launch angle, aim state
and angle-sensitive ballistic launch behavior.

The complete 469-file upstream snapshot is retained under
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. The unchanged C# Phone/Reach
project compiles with XNA 4.0; its `CatapultGame.dll` SHA-256 is
`2e8a3511003ecd3c4dc3ed2f9007551f9987b94390c414dfa1579b5adba7436a`. The diagnostic haptics shim
only supplies the missing Phone assembly. No local Phone 7 host exists, so original visual
execution is not claimed.

## Content

The official XNA 4.0 pipeline output is used verbatim: 33 XNBs and the original catapult animation
XML, 34 files total. `diff -qr` against the retained unchanged-project output is empty. The tutorial
DOCX is also byte-identical (SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`).

## Qualification

Debug and Release native OPENGLES3 builds pass. A Release OpenGL ES 3.2 run traverses menu and
instructions, performs the original pullback/angle shot, advances the turn, pauses, returns and
exits with status 0 and no console error. Evidence is under
`evidence/CatapultWarsTrainingShotAngle/cna-native-opengles3-release-qualified/` in the artifact
root.

The threaded WEBGL2 build runs in real Chrome on WebGL 2. Browser touch exercises the angle/shot
path and pause/return flow; 600 subsequent animation frames complete with zero page exceptions,
unhandled rejections and asset errors. Evidence is under
`evidence/CatapultWarsTrainingShotAngle/cna-web-webgl2-qualified/`.

No framework/runtime fix or sample workaround was needed. `diff.md` contains only lossless C++/AOT
adaptations, and there are no known active behavioral differences.
