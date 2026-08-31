# SAMPLE-068 — Catapult Wars Training: Shot Guide

## Result

This is the complete fourth exercise from `CatapultWarsTrainingKit_4_0`, retained as a separate
product in accordance with its own solution, entry point and content project. It includes the
Shot Angle behavior and the original dotted ballistic guide, including its prediction loop,
spacing and guide-dot rendering during pullback.

The whole 469-file upstream kit is retained at
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. The unchanged C# Phone/Reach
project compiles with XNA 4.0 and produces `CatapultGame.dll` SHA-256
`850869217604338b86c15e750546c06abf237577803d0d37da22bf23ea58cf10`. A diagnostic-only haptics shim
supplies the absent Phone SDK assembly. Since this machine has no Phone 7 application host, no
visual run of that original is asserted.

## Content

All official content products from the unchanged XNA 4.0 project are checked in: 34 XNBs,
including `guideDot.xnb`, plus the catapult animation XML (35 files total). Recursive comparison
against the retained pipeline output is empty. The common tutorial DOCX remains byte-identical,
SHA-256 `b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`.

## Qualification

Native Debug and Release OPENGLES3 builds pass. Release runs on OpenGL ES 3.2, reaches gameplay,
shows the predicted guide dots while dragging, fires, pauses, returns and exits with status 0 and
no console error. Dedicated `shot-guide.png` evidence is under
`evidence/CatapultWarsTrainingShotGuide/cna-native-opengles3-release-qualified/` in the artifact
root.

The threaded WEBGL2 product runs in real Chrome on an actual WebGL 2 context. Browser touch exposes
the guide and completes the shot/pause/return path. The 600-frame canary reports no exceptions,
unhandled rejections or HTTP errors; `web-shot-guide.png` and the result are under
`evidence/CatapultWarsTrainingShotGuide/cna-web-webgl2-qualified/`.

No CNA/sharp-runtime fix or workaround was required. There are no known active behavior
differences; `diff.md` records only necessary host representations.
