# SAMPLE-068 — Catapult Wars Training: Second Human Player

## Result

This is the complete second exercise from `CatapultWarsTrainingKit_4_0`, kept as a separate runnable
product to match its own upstream solution, entry point and content project. It preserves the
common Catapult Wars screen/gameplay implementation while replacing the AI side with the original
second `Human` player; turn ownership, touch input, prompts and both human firing paths remain
distinct.

The exact 469-file source kit is retained at
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. Its unchanged C# Phone/Reach
project compiles with XNA 4.0, producing `CatapultGame.dll` with SHA-256
`54c211d5a0a1f03836606afc7558e31fd36e87d2685aa8a7d839560bcab7fd54`. The build-only diagnostic
haptics shim supplies the unavailable Phone assembly; a local Phone 7 host is unavailable, so the
original is not claimed as visually executed.

## Content

The checked-in content is the official output of the unchanged XNA 4.0 content project: 33 XNBs
and `Textures/Catapults/AnimationsDef.xml` (34 files). Its recursive byte comparison with the
retained pipeline output is empty. The upstream tutorial DOCX is byte-identical, SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`.

## Qualification

Native Debug and Release OPENGLES3 builds pass. The Release binary runs on OpenGL ES 3.2 through
menu, instructions, both human turns and shots, pause, return and clean exit; the result is exit 0
with no console error. Evidence is under
`evidence/CatapultWarsTrainingSecondHuman/cna-native-opengles3-release-qualified/` in the artifact
root.

The threaded WEBGL2 bundle runs in real Chrome with WebGL 2. Real browser touch completes the same
two-human branch and both turns, then pause/return; the post-interaction 600-frame canary reports no
exceptions, rejections or HTTP errors. Evidence is under
`evidence/CatapultWarsTrainingSecondHuman/cna-web-webgl2-qualified/`.

No CNA or sharp-runtime change was required. There is no sample workaround and no known active
behavioral difference; necessary C++/AOT mechanics are listed in `diff.md`.
