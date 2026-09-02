# SAMPLE-137 — `PaddleBattle_4_0_Mango_VB` audit and owner decision

## Status

Fresh audit complete enough to require an owner decision under `SAMPLES-DEC-005`, jointly with
SAMPLE-136. This is Microsoft's complete Visual Basic delivery of the same Windows Phone 7.1
Silverlight/XNA hybrid product, not an independent new game. It nevertheless has a measured
language-visible collision-coordinate behavior, corrected project packaging and distinct Phone
and assembly identity. No duplicate C++ target, C#-canonical alias, standalone Pong `Game`, fake
Silverlight UI or loose-content route was invented.

The owner must first choose the shared retired-Phone compatibility versus explicit complete
native/WEBGL2 modernization versus historical non-port boundary. If a product is authorized, the
owner must additionally choose whether its C# or VB semantics are canonical or whether both
language identities warrant separate targets/reference products.

## Complete inventory and project contract

The exact upstream directory contains 27 files / 316,125 bytes. Its nine VB units contain 918
newline-counted lines, three XAML units contain 130 lines, two VB projects contain 283 lines, the
content project contains 60 lines and the solution contains 54 lines. Every file and SHA-256
digest matches the retained snapshot.

Unlike the adjacent C# package, this physical delivery contains no separate HTML article or RTF
license file. It does contain the complete three-project build graph:

- `PaddleBattle.vbproj` is an AnyCPU Silverlight XAP targeting `WindowsPhone71`. It embeds the VB
  runtime, uses `Option Explicit On`, `Option Strict Off`, `Option Infer On`, references Phone,
  Silverlight, XNA Framework/Graphics/Touch/Interop, and produces `PaddleBattle.xap`.
- `PaddleBattleLib.vbproj` is the standard XNA Windows Phone/Reach bridge. It has no gameplay code;
  it exists so the Silverlight application can consume the content project.
- `PaddleBattleContent.contentproj` owns the same three textures and two SoundEffects as
  SAMPLE-136.

The manifest targets Phone OS 7.1, launches landscape `MainPage.xaml`, declares the same eleven
template capabilities and uses an 800×480 layout. The app installs `PhoneApplicationService` and
`SharedGraphicsDeviceManager`, owns the shared `ContentManager`, pumps `FrameworkDispatcher` with
a `GameTimer`, persists `GameSettings`, and provides graphics services through
`IServiceProvider`.

## Exact relationship to SAMPLE-136

A full asset, XML, project and source comparison establishes one shared product:

- all three shell images and all five content sources are byte-identical;
- all three XAML documents have identical element/attribute/text trees after ignoring formatting
  and comments;
- the five content items have identical names, importers and processors (only declaration order
  and project metadata differ);
- the two-page Play/settings/game flow, Phone lifecycle, shared-device transitions,
  `UIElementRenderer` composition, 30 Hz update loop, touch input, AI, collision algorithm,
  scoring, sounds and draw order have the same control flow and constants;
- all five rebuilt WindowsPhone/Reach XNBs are byte-identical.

The VB delivery is not byte-level packaging duplication:

- it has distinct solution/project/content GUIDs and Phone `ProductID`;
- the shell and tile title is `PaddleBattle`, while C# spells it `Paddle Battle`;
- its assembly company/copyright text, COM typelib GUID, embedded VB runtime and compiler options
  differ;
- its bridge assembly/root namespace is consistently `PaddleBattleLib`, whereas the C# bridge
  retains the incomplete historical `PinballBattleLib` rename;
- its application reference correctly names `..\PaddleBattleLib\PaddleBattleLib.vbproj`; the C#
  project references the nonexistent `..\PinballBattleLib\PaddleBattleLib.csproj`.

These are recorded rather than silently normalized.

## Measured Visual Basic gameplay difference

The translated VB source does not preserve C# numeric conversion semantics exactly. In
`Sprite.Bounds`, the C# variant offsets collision bounds with:

```text
(int)Position.X
(int)Position.Y
```

while VB uses:

```text
CInt(Position.X)
CInt(Position.Y)
```

C# floating-to-integer casts truncate toward zero. VB `CInt` rounds to the nearest integer using
banker's rounding for midpoint ties. This affects real fractional positions produced by the ball
and AI every update, so collision rectangles can differ by one pixel. `GamePage` has the same
conversion difference for `ActualWidth`/`ActualHeight`, although the normal 800×480 layout is
integral.

This was measured through the actual Microsoft compilers, then through the exact original
gameplay classes rather than inferred from syntax alone:

| Probe | VB result | C# result |
|---|---:|---:|
| scalar `1.6` conversion | `2` | `1` |
| scalar `-1.6` conversion | `-2` | `-1` |
| exact `Sprite.Bounds` at `Position=(1.6,-1.6)` | `(2,-2)` | `(1,-1)` |

The unchanged VB `Sprite.vb`, `Paddle.vb` and `Ball.vb` compile successfully together against
authentic XNA 4 references before the direct probe runs. A shared C++ port cannot simultaneously
claim exact C# truncation and VB rounding. Selecting one as canonical is a product decision, not a
mechanical translation choice for this audit.

## Complete hybrid behavior

Except for the conversion rule above, the VB delivery preserves SAMPLE-136's complete behavior:

- a real Silverlight Play button and two-way-bound “Play sounds?” checkbox navigate to the game;
- `GameSettings` is a `DependencyObject` with a true-default `PlaySounds` dependency property,
  saved and loaded through `IsolatedStorageSettings.ApplicationSettings` on Phone lifecycle
  transitions;
- `GamePage` enables graphics sharing on navigation in and disables it on navigation out;
- a 30 Hz `GameTimer` updates the XNA play field while `UIElementRenderer` rasterizes the two live
  Silverlight score `TextBlock`s into a texture drawn last by `SpriteBatch`;
- the 36×39 ball starts at `(400,240)`, launches at 300 pixels/second in a random -45 through 44
  degree direction, and uses collision bounds `(4,4,28,28)`;
- the 43×121 paddles start at x=100/x=700, use `(6,1,31,109)` bounds, and the computer tracks at
  120 pixels/second;
- only the first active touch controls the player; there is no keyboard, mouse or gamepad route;
- quadratic hit offset produces up to 60 degrees of paddle deflection, wall/paddle impacts play
  `plink`, goals play `score`, and the setting gates both;
- Draw clears CornflowerBlue, paints the four-pixel center line, paddles, ball and then the live UI
  texture.

Replacing this with a font-rendered Pong loop would remove the sample's defining mixed
Silverlight/XNA behavior.

## Authentic content and CNA evidence

The unchanged content project completed through the official XNA Game Studio 4.0 Content
Pipeline for `WindowsPhone/Reach`. Its exact outputs match SAMPLE-136:

| Asset | Exact XNB | SHA-256 |
|---|---:|---|
| `ball.png` | 5,803 bytes | `c141140421e8bb89cbd55838e102f323991bdf190740733426b5b68174847a71` |
| `paddle_blue.png` | 20,999 bytes | `013d34c63b6705028792a9beadf37c5942ddb8646d3efb1b3ed29070489f8df7` |
| `paddle_red.png` | 20,999 bytes | `00ca841bdf45d8d8ffc1a4ac48d15f59799365137721870f34949de5a0425068` |
| `plink.wav` | 94,345 bytes | `6c45adafe02c4195fc83969419fe5090b4bd3a9edcdac461063e037ec151d30a` |
| `score.wav` | 113,513 bytes | `b860305d73036032ef6e10c760f967a13aa2d238b11ecac0dc1d1b271b31910f` |

All are uncompressed XNB version 5 with the Windows Phone platform byte (`XNBm`). Live
`cna-content` converts 5/5 to three Texture2D and two SoundEffect CNBs, every CNB validates, and
17/17 focused runtime/pipeline tests pass. These cover real texture/sound XNB loading, XNB-versus-
CNB semantic comparison and native SoundEffect round trips.

Content and host-independent gameplay source are therefore not the blocker. No loose PNG/WAV
copy, substitute font or sample-local loader is needed or permitted.

## Original host and live dependency boundary

The unchanged full solution was submitted to .NET 4 MSBuild using `Rebuild`,
`Debug|Mixed Platforms`, `/m:1` and an offline Xvfb display. It selects the correct VB projects,
then stops before application compilation:

- `Microsoft.Silverlight.VisualBasic.targets` is absent;
- `Microsoft.Xna.GameStudio.targets` is absent from the Wine prefix.

The result is exactly zero warnings and two errors. The owner's offline Win7 installation was
already proven during SAMPLE-132 to lack the Phone 7.1 Silverlight target set. No unchanged XAP,
Phone emulator run, shared-device page transition or original frame is claimed.

At live CNA HEAD `5347b52eae13` and Sharp Runtime HEAD `9cc96cd57cde`, source scans still find no
`PhoneApplicationPage`, `PhoneApplicationFrame`, `PhoneApplicationService`,
`SharedGraphicsDeviceManager`, `GraphicsDevice.SetSharingMode`, `UIElementRenderer`, Phone
`GameTimer`, dependency-property system or `IsolatedStorageSettings` application dictionary. No
CNA or Sharp Runtime source was changed.

## Owner choice required

Choose the joint SAMPLE-136/137 product boundary:

1. Retain both deliveries as evidence-backed historical Phone 7.1 non-ports, including their
   packaging and numeric differences.
2. Authorize one faithful retired-platform or complete native/WEBGL2 hybrid product and declare
   C# truncation/identity canonical; retain VB as reference/support data.
3. Authorize one such product with VB rounding/identity canonical; record the deliberate departure
   from the C# delivery.
4. Authorize two separately qualified language-faithful targets/products. Both still require the
   same complete page/navigation/settings/lifecycle/shared-device/UI-composition stack; the VB
   target additionally preserves `CInt` behavior and shell/project identity.

Do not label a duplicated C++ executable complete merely because its content is identical. Do not
replace the missing hybrid host with a Pong-only `Game`, fake checkbox, always-on sound,
hard-coded score sprites, invented desktop controls or an uncomposited HTML overlay.

## Reproduction and evidence

Artifact root:

`/rv/tmp/samples/SAMPLE-137-PaddleBattle_4_0_Mango_VB/`

Important retained material:

- `xna4-original/` — exact 27-file delivery;
- `xna4-build/phone-reach/Content/` — five authoritative XNA outputs;
- `cna-diagnostic/` — five validated CNBs;
- `evidence/original-msbuild.log` and `reference-boundary.txt` — unchanged host result and claim
  boundary;
- `evidence/variant-relationship.tsv` — exact C#/VB common and distinct identities;
- `evidence/numeric-*.txt` and `gameplay-bounds-*.txt` — Microsoft-compiler and direct gameplay
  conversion evidence;
- content build/hash/conversion/validation/test logs plus complete inventory and snapshot hashes;
- `scripts/build-original-content.sh`, `XnaPipelineRunner.cs`, `NumericSemantics.*`,
  `GameplayBoundsProbe.cs`, `audit.py` and `qualify.sh` — offline reproduction.

Re-run with:

```bash
/rv/tmp/samples/SAMPLE-137-PaddleBattle_4_0_Mango_VB/scripts/qualify.sh
```

The only parallel content stage uses `--workers 8`; the unchanged solution uses `/m:1` and all
other compilers/tests are serial, respecting the session-wide eight-core limit. Native OPENGLES3
and WEBGL2 product gates do not apply until the owner authorizes a faithful hybrid product and its
language identity.
