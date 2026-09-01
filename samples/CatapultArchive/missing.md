# SAMPLE-122 — `Catapult_ARCHIVE_2_0` audit and owner decision

## Status

Fresh audit complete enough to require an older-XNA product decision under `SAMPLES-DEC-005`.
This is a complete, independently runnable XNA 2.0 game, not an earlier copy or teaching stage of
the already-complete XNA 4 CatapultWars. No XNA4 rewrite, partial C++ game or CatapultWars alias was
introduced without the owner's scope ruling.

## Complete product inventory

The entire upstream delivery contains 32 files / 35,112,456 bytes:

- Windows and Xbox 360 XNA 2 solutions/projects;
- four C# files / 1,002 lines (`CatapultGame`, `Catapult`, `CatapultState`, entry point and assembly
  metadata);
- one content project with eight PNG textures, one Tahoma SpriteFont description and one XACT 2
  project;
- nine PCM WAV inputs, including 324.43 seconds of looping music;
- an 18,547-byte HTML game/control/design document, application artwork and the Microsoft
  Permissive License.

The documentation explicitly calls this a complete mini game ready to compile and run. It fixes
the Windows product at 1280x720 and documents these controls:

| Action | Keyboard | Gamepad |
|---|---|---|
| move catapult | Right arrow | left trigger |
| analog/optimal boost | `B` | right trigger |
| launch/reset | Space | A |
| exit | Escape | Back |

## Audited behavior

The game shows a three-second title, opens the storage selector and then drives one catapult through
six states: `Rolling`, `Firing`, `Crash`, `ProjectileFlying`, `ProjectileHit` and `Reset`.

- Movement accelerates while Right/left trigger is held. Launching near X=580/600/620 awards three
  distinct velocity multipliers and boost indicators; X>650 crashes into the log.
- The arm rotates to 81 degrees before release. The pumpkin uses the authored velocity,
  acceleration, rotation, ground bounce and horizontal damping constants until it stops.
- The foreground and sky scroll at different rates. The HUD draws distance, height, power bonus,
  persistent high score, crash/high-score and boost messages.
- `GameData.sav` contains the high score as one binary `Int32` in storage container `Catapult`.
- The XACT project maps nine source waves to exactly nine cues: `ThrowSound`, `Crash`, `Boost`,
  `Bounce`, `HighScore`, `Hit`, `Flying`, `Move` and looping `Music`.

These mechanics, content and controls do not occur in CatapultWars. A full hash comparison against
`CatapultWars_4_0` found no shared source and no shared runtime content. Only the generic XNA game
icon and Microsoft license are byte-identical. CatapultWars is a turn-based two-player/AI artillery
game with menus, touch, health and destructible catapults; this product is a continuous single-run
pumpkin-distance game.

## Authentic reference boundary

The snapshot requires XNA Game Studio 2.0 and XACT 2 authoring output. The established Wine prefix
and offline Win7 VM contain XNA 4 only. The offline XP VM was booted headless with no network and
reached a logged-in desktop, but the known Win7 credentials do not authenticate to XP Guest
Control; no further account-name guessing was performed. XP shut down normally. Its installed XNA
version therefore remains unverified.

No original executable, XNB or XGS/XSB/XWB bank is shipped in the archive, and no locally
accessible XNA 2 compiler/pipeline/XACT builder was found. Consequently this audit does **not**
claim an authentic XNA 2 build or runtime capture. The retained collection thumbnail documents the
expected scene, but is not substituted for a real executable comparison.

## Measured XNA4/CNA migration boundary

The unchanged source was compiled diagnostically against official XNA 4 references without editing
it. It reaches only eight API migration errors:

- three calls to the old `Guide.Begin/EndShowStorageDeviceSelector` route;
- two synchronous `StorageDevice.OpenContainer` calls;
- two uses of the removed public `StorageContainer.Path` property;
- one `SpriteBatch.Begin(SpriteBlendMode.AlphaBlend)` call.

XNA4/CNA has the corresponding selector, async-open/container stream and `BlendState.AlphaBlend`
semantics, so no missing game algorithm or broad framework subsystem was inferred from those
errors. They are nevertheless an XNA2-to-XNA4 source modernization, not an unchanged XNA 4 port.

The unchanged non-XACT assets were also tested through the official XNA 4 Windows/HiDef pipeline:
all eight textures and the SpriteFont produced nine valid XNBs. Current CNA transcoded all nine to
validated CNBs with zero failures. The unchanged `Sounds.xap` did **not** build: XNA4 `BuildXact`
explicitly reports that it was created by an incompatible XACT version. This is honest migration
evidence, not permission to replace the cue graph with loose WAV playback.

At live CNA HEAD `7712534d3d22`, 256 focused XACT/audio tests, 9 storage tests and 13 focused
Texture2D/SpriteFont XNB tests pass. That proves the current XNA4-shaped runtime surfaces are live;
without authentic XACT2 banks it does not prove that CNA parses the exact older binary versions
this game would produce. No CNA or Sharp Runtime source change was needed for the audit.

## Evidence and reproducibility

Artifact root: `/rv/tmp/samples/SAMPLE-122-Catapult_ARCHIVE_2_0/`.

- `xna2-original/` is the complete byte-for-byte upstream snapshot;
- `evidence/file-inventory.tsv`, `sha256sum.txt`, `image-metadata.tsv`, `audio-metadata.tsv`,
  `readme-text.txt`, `xact-cues.txt` and the empty `snapshot-diff.txt` cover every input;
- `evidence/catapultwars-overlap.tsv` proves the two unrelated products share only icon/license;
- `evidence/original-toolchain-audit.txt` records the Wine/Win7/XP reference search and explicitly
  limits the claims;
- `evidence/unchanged-source-vs-xna4.log` retains all eight compiler diagnostics;
- `xna4-diagnostic/Content/` and `cna-diagnostic/` retain the nine successful XNB/CNB products;
- `evidence/xna4-content-build.log` retains both the nine successful builds and exact XACT-version
  rejection;
- three `cna-*-tests.log` files retain the 256/9/13 live test passes;
- `scripts/compile-unchanged-against-xna4.sh`, `build-xna4-content-diagnostic.sh` and `audit.sh`
  reproduce the offline evidence. The diagnostic content script uses one process; CNA content
  conversion uses at most eight workers.

## Owner decision required

Choose one:

1. authorize a faithful port of this distinct older-XNA product, first providing an authentic
   XNA2/XACT2 reference build route or exact XGS/XSB/XWB outputs, then defining XNA2-to-XNA4 API
   migration as the accepted C++ target contract;
2. explicitly authorize an XNA4 modernization, including XACT3 project upgrade/output and the
   measured storage/SpriteBatch API mappings, followed by full OPENGLES3 and WEBGL2 qualification;
3. accept an evidence-backed historical-game archive/non-port boundary, retaining its complete
   sources/assets/documentation without inventing a replacement product.

Until that ruling, omitting audio, substituting nine loose WAVs, relabeling CatapultWars, or calling
an XNA4-upgraded build the authentic XNA2 original would violate the campaign's fidelity rules.
