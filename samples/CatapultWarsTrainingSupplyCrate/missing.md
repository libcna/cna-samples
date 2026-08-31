# SAMPLE-068 — Catapult Wars Training: Supply Crate

## Result

This is the complete fifth exercise from `CatapultWarsTrainingKit_4_0`, kept separate because its
upstream solution owns an entry point and content project. It preserves the expanded projectile
state model, animated supply crates, crate collision/destruction, split-ammunition award, weapon
HUD, `SplitProjectile` creation and the two-projectile split behavior.

The entire 469-file upstream kit is retained under
`/rv/tmp/samples/SAMPLE-068-CatapultWarsTrainingKit_4_0/xna4-original/`. The unchanged C# Phone/Reach
source compiles against XNA 4.0; `CatapultGame.dll` has SHA-256
`edc28eb3a22e9420da051b93c3e9159d94f538f752d0377b02d4b0370450b450`. The labelled haptics shim is
only a diagnostic substitute for the missing Phone assembly. A Phone 7 host is unavailable, so a
visual original run is not claimed.

## Content

The unchanged content project was built by the official XNA 4.0 pipeline. The checked-in 39 files
(37 XNBs and two animation XML copy items) compare byte-for-byte with that retained output. They
include the original crate animation, normal/split ammo, weapon HUD and projectile textures. The
tutorial DOCX is retained byte-identical, SHA-256
`b6062cc3becc8bb59b936e7a18d2d6b63cbbd936b6c9311b6d7da195b97dd524`.

## Qualification

Native Debug and Release OPENGLES3 builds pass. The real OpenGL ES 3.2 Release run reaches
gameplay, hits and destroys the enemy supply crate, observes split ammunition in the HUD, fires a
later split projectile, pauses, returns and exits cleanly. `after-first-turn.png` and
`split-projectile.png` plus exit-0/no-error logs are under
`evidence/CatapultWarsTrainingSupplyCrate/cna-native-opengles3-release-qualified/` in the artifact
root.

The threaded WEBGL2 build also runs in real Chrome on WebGL 2. Browser touch exercises normal
gameplay and the menu/pause lifecycle, and the 600-frame post-interaction canary has no exceptions,
unhandled rejections or HTTP errors. The deterministic crate-hit/split-ammo branch is established
by the targeted native run rather than claimed from the random-wind browser capture. Browser
evidence is under `evidence/CatapultWarsTrainingSupplyCrate/cna-web-webgl2-qualified/`.

No CNA or sharp-runtime workaround was needed. Shared ownership in the C++ active-projectile list
preserves C# reference identity; it is a representation detail, not a behavior change. There are
no known active behavioral differences.
