# SAMPLE-136 — `PaddleBattle_4_0_Mango` audit and owner decision

## Status

Fresh audit complete enough to require an owner product/platform decision under
`SAMPLES-DEC-005`. Paddle Battle is a complete Windows Phone 7.1 **Silverlight/XNA hybrid** whose
documented purpose is mixing Silverlight pages/UI with XNA rendering. It is not an ordinary XNA
`Game` with an optional menu. No standalone Pong game, sprite-font score replacement, fake
checkbox, keyboard/mouse controls, loose-content route or HTML/CSS rewrite was added.

The five content items and the self-contained gameplay math are fully usable. The blocker is the
defining Phone/Silverlight/XNA host and lifecycle, a decision-sized compatibility or modernization
scope rather than a narrow CNA API defect.

## Complete inventory and project contract

The exact upstream delivery contains 29 files / 374,569 bytes. Its nine C# units contain 972
lines, three XAML units contain 130 lines, two C# projects contain 202 lines, the content project
contains 82 lines and the one solution contains 54 lines. Every file and SHA-256 digest matches
the retained snapshot.

`PaddleBattle.sln` describes one deployed product through three projects:

- `PaddleBattle.csproj` is an AnyCPU Silverlight library/XAP targeting `WindowsPhone71`, with
  `App.xaml`, landscape `MainPage.xaml` and landscape `GamePage.xaml`. It references the Phone,
  Silverlight, XNA Framework/Graphics/Touch and XNA interop assemblies and produces
  `PaddleBattle.xap`.
- `PaddleBattleLib.csproj` is the standard XNA Windows Phone/Reach bridge used to make an XNA
  content project consumable by a Silverlight application. It intentionally compiles only its
  assembly metadata; its readme explicitly says no code is required.
- `PaddleBattleContent.contentproj` owns three textures and two SoundEffects.

The manifest targets Phone OS 7.1, launches `MainPage.xaml`, declares runtime type `Silverlight`
and carries the template's eleven Phone capabilities. The 800×480 layout hides the system tray and
requires landscape orientation.

## Measured upstream packaging defect

The exact C# application project references:

```text
../PinballBattleLib/PaddleBattleLib.csproj
```

but the physical directory is `PaddleBattleLib`. The reference GUID still matches the real
solution project. The real library also retains the internal `PinballBattleLib` root namespace and
assembly name, showing that this is an incomplete historical rename rather than a different
library. The adjacent official Visual Basic delivery uses a consistent `PaddleBattleLib` path.

The original snapshot was not edited to hide this defect. A future authorized compatibility port
must record any project repair as an original-build prerequisite; it cannot silently present a
fixed project as the unchanged source.

## Defining Silverlight/XNA behavior

The six-paragraph Microsoft documentation explicitly calls Paddle Battle a demonstration of the
new Silverlight/XNA application model. Silverlight renders both the menu and scores; XNA renders
the play field; the page is rotated to landscape in the Phone emulator.

`App` owns the mixed runtime:

- XAML installs both `PhoneApplicationService` and `SharedGraphicsDeviceManager` as application
  lifetime objects;
- a shared `ContentManager` loads precompiled XNA content;
- a continuously running `GameTimer` pumps `FrameworkDispatcher.Update()` so XNA audio/events
  work inside a Silverlight application;
- launch/activate load settings and deactivate/close save them;
- `IServiceProvider.GetService` resolves the shared graphics service from the lifetime-object
  collection.

`MainPage` is real Silverlight UI rather than a painted approximation. It displays the title, a
400×100 Play button and a two-way-bound “Play sounds?” checkbox. `GameSettings` is a
`DependencyObject`; its `PlaySounds` dependency property defaults true and round-trips through
`IsolatedStorageSettings.ApplicationSettings`. Play navigates to `/GamePage.xaml` through the
Phone page stack.

`GamePage` changes the shared graphics device to sharing mode on entry and back on exit. A 30 Hz
page `GameTimer` drives Update/Draw. `UIElementRenderer` rasterizes the live page — the two score
`TextBlock`s — into a `Texture2D`, which `SpriteBatch` composites after the XNA play field. Layout
changes dispose/recreate that renderer at the page's actual dimensions.

This sharing/composition/page transition is the sample's subject. Replacing it with one CNA
`Game::Draw` method and a font is not a mechanical C#→C++ translation.

## Complete gameplay behavior

The gameplay itself is small but complete:

- the 36×39 ball uses collision bounds `(4,4,28,28)` and starts at `(400,240)` with speed 300 at
  a random angle from -45 through 44 degrees, randomly aimed toward either paddle;
- blue/player and red/AI 43×121 paddles use bounds `(6,1,31,109)` and start centered at x=100 and
  x=700;
- only the first active touch controls the player's vertical center; no mouse, keyboard or
  gamepad fallback exists;
- the AI tracks the ball at 120 pixels/second and both paddles clamp to the 480-pixel height;
- top/bottom impacts resolve penetration and reverse Y; paddle impacts preserve speed but curve
  the angle quadratically up to 60 degrees according to hit offset, then resolve X penetration;
- leaving x=800 scores for the player, leaving left of the ball width scores for the computer,
  updates the corresponding Silverlight text and resets the ball;
- `plink` plays for wall/paddle impacts and `score` for goals, gated by the persistent checkbox;
- Draw clears CornflowerBlue, paints a four-pixel white center line, the two paddles, ball and then
  the freshly rendered Silverlight score texture.

## Authentic content evidence

The unchanged five-item content project completed through the official XNA Game Studio 4.0
Content Pipeline for `WindowsPhone/Reach`:

| Asset | Processor | Exact output |
|---|---|---:|
| `ball.png` (36×39 RGBA) | TextureProcessor | `ball.xnb`, 5,803 bytes |
| `paddle_blue.png` (43×121 RGBA) | TextureProcessor | `paddle_blue.xnb`, 20,999 bytes |
| `paddle_red.png` (43×121 RGBA) | TextureProcessor | `paddle_red.xnb`, 20,999 bytes |
| `plink.wav` (48 kHz stereo PCM16, 23,560 frames) | SoundEffectProcessor | `plink.xnb`, 94,345 bytes |
| `score.wav` (44.1 kHz stereo PCM16, 28,352 frames) | SoundEffectProcessor | `score.xnb`, 113,513 bytes |

All five outputs are uncompressed XNB version 5 with the Windows Phone platform byte (`XNBm`).
Live `cna-content` converts all five to three Texture2D and two SoundEffect CNBs; every CNB validates.
Seventeen focused runtime/pipeline tests pass, including real Texture2D/SoundEffect XNB loads,
runtime-XNB versus transcoded-CNB semantic comparisons and native SoundEffect round trips.

Content, texture rendering and audio decoding are therefore not the reason no port was added. No
loose PNG/WAV copy is needed or allowed.

## Original host boundary

The exact solution was submitted to .NET 4 MSBuild in the local offline XNA 4 Wine prefix using
`Rebuild`, `Debug|Mixed Platforms`, `/m:1` and diagnostic logging. It selects the correct projects,
then stops before C# compilation:

- the application lacks `Microsoft.Silverlight.WindowsPhone71.Overrides.targets`;
- the bridge library lacks `Microsoft.Xna.GameStudio.targets` in this prefix.

The owner's offline Win7 installation was independently proven during SAMPLE-132 to lack the same
Phone 7.1 Silverlight target set. Together with the measured project-reference typo, no unchanged
XAP, Phone emulator run, shared-device page transition or original frame is claimed. The successful
content-only route does not masquerade as a running hybrid application.

## Relationship to SAMPLE-137

SAMPLE-137 is the official Visual Basic delivery of this product and remains a separate plan row.
This audit establishes, but does not pre-decide that row:

- all five content sources and all three shell images are byte-identical;
- `App.xaml`, `MainPage.xaml` and `GamePage.xaml` are semantically identical XML after ignoring
  formatting/comments;
- both manifests target Phone 7.1 with the same eleven capabilities and entry page, but the VB
  product has a distinct ProductID and spells its app/tile title `PaddleBattle` rather than
  `Paddle Battle`;
- the VB project corrects the C# delivery's broken library path.

The next audit must still compare all VB control flow and language-visible behavior before the
owner chooses a shared canonical product, separate language identity or retained support data.

## Live CNA / Sharp Runtime boundary

At live CNA HEAD `5347b52eae13` and Sharp Runtime HEAD `9cc96cd57cde`, source scans find no
`PhoneApplicationPage`, `PhoneApplicationFrame`, `PhoneApplicationService`,
`SharedGraphicsDeviceManager`, `GraphicsDevice.SetSharingMode`, `UIElementRenderer`, Phone
`GameTimer`, `DependencyObject`/`DependencyProperty` or `IsolatedStorageSettings` application
dictionary.

A truthful implementation requires the complete two-page navigation/lifecycle, live Silverlight
layout/data binding, persistent setting, graphics-device sharing and UI-to-texture composition
contract. Adding those only for Paddle Battle would be a large platform subsystem; painting
equivalent-looking text directly in the sample would be a workaround. No CNA or Sharp Runtime
source was changed.

## Owner choice required

Choose one product boundary:

1. Accept an evidence-backed historical Phone 7.1 Silverlight/XNA non-port while retaining the
   exact source, content products, project defect and C#/VB relationship evidence.
2. Authorize a faithful retired-platform compatibility product, including a working Phone 7.1
   SDK/emulator reference, the corrected original project prerequisite, Phone page/lifecycle and
   settings stack, shared GraphicsDevice mode and real UIElementRenderer composition.
3. Authorize an explicit complete native/WEBGL2 modernization. Define the accepted page/navigation,
   UI toolkit, two-way checkbox/persistence, suspend/resume, graphics/UI composition and Back
   semantics. Preserve touch-only gameplay, exact timings/math/draw order and all five XNB assets.

Do not authorize or label as complete a Pong-only `Game`, headless physics test, always-on sound,
hard-coded score sprites, invented keyboard/mouse input or uncomposited HTML overlay.

## Reproduction and evidence

Artifact root:

`/rv/tmp/samples/SAMPLE-136-PaddleBattle_4_0_Mango/`

Important retained material:

- `xna4-original/` — exact 29-file delivery;
- `xna4-build/phone-reach/Content/` — five authoritative XNA outputs;
- `cna-diagnostic/` — five validated CNBs;
- `evidence/original-sha256.txt`, `file-inventory.tsv` and empty `snapshot-diff.txt` — complete
  identity evidence;
- `evidence/original-msbuild.log` and `reference-boundary.txt` — unchanged host result and exact
  claim boundary;
- `evidence/xna4-content-build.log`, `xna4-content-sha256.txt`,
  `cna-content-transcode.log`, `cna-content-validation.log` and
  `cna-focused-content-tests.log` — reproducible content qualification;
- `evidence/documentation-text.txt`, content metadata and `sample137-relationship.tsv` — product
  and adjacent-variant evidence;
- `scripts/build-original-content.sh`, `XnaPipelineRunner.cs`, `audit.py` and `qualify.sh` — offline
  reproduction.

Re-run with:

```bash
/rv/tmp/samples/SAMPLE-136-PaddleBattle_4_0_Mango/scripts/qualify.sh
```

The only parallel stage is `cna-content --workers 8`; every other build/test is serial and the
session-wide eight-core limit is respected. Native OPENGLES3 and WEBGL2 product gates do not apply
until the owner authorizes a faithful product rather than a sample-local substitute.
