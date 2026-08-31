# SAMPLE-068 — Catapult Wars Training: Scrolling Screen

## Result

This is the complete independent advanced product from `CatapultWarsTrainingKit_4_0`. Its separate
upstream solution, entry point and content project are preserved as their own CNA sample. The port
retains the larger world, draw scale/offset, clamped camera, direct pan, flick/inertia, pinch zoom,
catapult hit testing in transformed coordinates and automatic projectile-follow camera.

The complete 469-file source kit is retained at
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. Its unchanged C# Phone/Reach
project compiles with XNA 4.0 and yields `CatapultGame.dll` SHA-256
`caeaf095204514e430ebcde234f16158a86bf18afc991cd44c8f32c1943e0d32`. A clearly labelled build shim
only supplies the unavailable Phone haptics assembly. No local Phone 7 application host exists, so
original visual execution is not claimed.

## Content

The official XNA 4.0 pipeline output is checked in exactly: 33 XNBs plus the unchanged catapult
animation XML, 34 files total. A recursive byte comparison with the retained unchanged-project
output is empty. The upstream tutorial DOCX remains byte-identical, SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`.

## Qualification

Native Debug and Release OPENGLES3 builds pass. The real OpenGL ES 3.2 Release run exercises manual
camera movement, a transformed catapult drag/fire, projectile-follow across the world, return to
the next turn, pause/menu and clean exit. The result is exit 0 with no console error; captures
including `camera-pan.png`, `camera-following-projectile.png` and `after-first-turn.png` are under
`evidence/CatapultWarsTrainingScrollingScreen/cna-native-opengles3-release-qualified/` in the
artifact root.

The threaded WEBGL2 bundle runs in real Chrome on WebGL 2. Real browser touch exercises the
scrolling-camera path and pause/return lifecycle, then 600 animation frames complete without page
exceptions, unhandled rejections or HTTP errors. The specialized transformed catapult hit,
fire and follow sequence is proven by the targeted native run rather than inferred from the
browser camera-pan capture. Browser evidence is under
`evidence/CatapultWarsTrainingScrollingScreen/cna-web-webgl2-qualified/`.

No CNA/sharp-runtime fix or sample workaround was required. The C++ pointer back to the owning
gameplay screen represents the original C# object reference used by the catapult to control the
camera. There are no known active behavioral differences.
