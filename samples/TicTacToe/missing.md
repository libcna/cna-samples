# SAMPLE-015 — TicTacToe_4_0 audit and owner decision

## Status

`SAMPLE-015` is blocked at `SAMPLES-DEC-004`. The existing CNA program is not a faithful port and
must not be treated as one. No new workaround has been added during this audit, and no large
Windows Phone compatibility subsystem has been started without the owner's decision.

## Sources and artifacts

- Exact upstream directory: `/rv/tmp/XNAGameStudio/Samples/TicTacToe_4_0`
- Stable audit root: `/rv/tmp/samples/SAMPLE-015-TicTacToe_4_0`
- Unchanged copied source: `/rv/tmp/samples/SAMPLE-015-TicTacToe_4_0/xna4-original`
- Original server build log:
  `/rv/tmp/samples/SAMPLE-015-TicTacToe_4_0/evidence/xna4-server-xbuild.log`
- Original server run log:
  `/rv/tmp/samples/SAMPLE-015-TicTacToe_4_0/evidence/xna4-server-mono-run.log`
- Original client build log:
  `/rv/tmp/samples/SAMPLE-015-TicTacToe_4_0/evidence/xna4-client-xbuild.log`

The audit covered the documentation, both solutions, all projects and configuration files, the
client's `TicTacToe.cs` and `Button.cs`, the generated WCF proxy, all service/server sources, the
third-party push-message assembly, and the complete content project. The original contains 2,447
lines of checked-in C# across the client, generated proxy, service and host, excluding generated
`obj` files and including assembly metadata. The current CNA implementation has 329 lines of C++.

## What the original sample is

The upstream documentation describes a Windows Phone 7 multiplayer and push-notification sample,
not merely a local Tic-Tac-Toe game. The phone client registers an `HttpNotificationChannel`
callback URI with a self-hosted WCF service. `Register` is request/response; later operations are
one-way calls, and game-state changes return through Windows Phone push notifications encoded as
XML. The service owns move validation, turn order, win/tie detection, restart state, subscriptions
and random AI moves.

The client also has observable XNA behavior that the current port omits: a 480x800 portrait,
fullscreen, approximately 30-fps game; `TouchPanel` tap gestures; rendered `Send Move`, `New Game`
and `Exit` buttons; a separate selected-move confirmation step; `GamePad` Back handling; the
original seven-state asynchronous lifecycle; and the original 40/60 human/AI first-turn choice.

## Original build and run evidence

- The unchanged `TicTacToeServer.sln` builds successfully in Release using Mono xbuild. The output
  remains under the stable audit root.
- Running the unchanged server as the normal user reaches WCF host startup and fails while binding
  its HTTP listener with `SocketException: Access denied`. This agrees with the upstream
  documentation, which says `Server.exe` must run administratively for self-hosted WCF.
- The unchanged Windows Phone client cannot be built in the current Linux reference environment.
  Its project targets `XnaPlatform=Windows Phone`, references `Microsoft.Phone` and
  `Microsoft.Phone.Interop`, and imports the XNA Game Studio project targets. The prepared
  `.wine-cna-xna40` prefix contains the desktop XNA 4.0 runtime (including the XNA touch assembly),
  but not the Windows Phone 7 SDK, `Microsoft.Phone.*`, or XNA Game Studio MSBuild targets. The
  xbuild log records the missing import. A faithful live reference therefore needs the retained
  Windows 7/Visual Studio 2010 Windows Phone environment or an equivalent WP7 SDK/emulator setup.

## Existing CNA port is a workaround

The current `samples/TicTacToe` program replaces the defining platform/service behavior instead of
porting it:

- all WCF, push channel, callback URI, XML message and subscription behavior is removed;
- server-owned validation, state and AI are reimplemented locally in the client;
- touch plus rendered confirmation/action buttons are replaced by direct mouse moves and keyboard
  shortcuts;
- the portrait phone layout is replaced by an invented 800x600 desktop layout;
- asynchronous states are reduced to three local states and invented AI delays are added;
- the original two Moire `SpriteFont` assets are replaced by one unrelated loose bitmap font;
- fullscreen, 30-fps timing, GamePad Back, and 40/60 first-turn behavior are changed;
- an original-nonexistent F1 help overlay is loaded from `Content/help.png`.

The three checked-in `blank`, `Circle` and `X` PNG source files are byte-identical to upstream, but
the current target loads them as loose images instead of the original content-pipeline XNBs. That
content provenance is recoverable if the owner selects a porting option; it does not make the
current local-game implementation faithful.

These are active differences, not accepted platform adaptations. The old statements that they were
"not planned" or acceptable substitutes are preserved below only as rejected historical evidence.

## Current runtime capability result

No CNA defect justifies the input, timing or fullscreen substitutions: current `cnanext` already
implements `TouchPanel` gesture queuing/`ReadGesture`, `GraphicsDeviceManager::IsFullScreen`,
`Game::TargetElapsedTime`, and `GamePad`. Current `sharp-runtimenext` has `System::Guid`,
`System::Uri`, XML reader/writer facilities and an HTTP client, but the audit found no
`System.ServiceModel` WCF host/client stack, `IXmlSerializable`/`XmlSerializer`, or Windows Phone
`HttpNotificationChannel` contract. Microsoft Push Notification Service itself is a retired
external platform dependency.

## Owner decision required

Choose one boundary before implementation continues:

1. Authorize a reusable, faithful emulation of the retired platform contract. Port the original
   phone UI/game flow and service protocol while implementing the missing WCF/push/XML surface in
   the owning runtime or a reusable platform-compatibility layer. Native and browser builds must
   preserve the original asynchronous registration, one-way calls, notifications and server-owned
   state; no sample-local networking or gameplay substitute is allowed.
2. Retain the unchanged Windows Phone client and WCF server as the target-specific original and
   accept an evidence-backed CNA non-port/ignored conclusion for this directory. Remove the current
   misleading local CNA game rather than presenting it as the sample.
3. Define another explicit, measured platform boundary. Any replacement transport or local
   emulation is a deliberate owner-approved deviation and must not be described as XNA/WP7 parity.

Until that decision is made, native OPENGLES3 and WEBGL2 completion gates are intentionally not
claimed and the row remains `🛑`.

## Rejected historical pre-audit notes

The following observations helped identify the old substitutions, but their former "not planned"
and "acceptable substitute" conclusions have no authority under the current campaign rules.

## Networking and WCF web service removed
**XNA behaviour:** The original is a networked Windows Phone 7 game.  All game logic
(move validation, win detection, AI moves) is delegated to a WCF web service
(`TicTacToeService`) running on a separate server.  The client sends moves and receives
game state updates via asynchronous WCF calls and Windows Phone push notifications
(`HttpNotificationChannel`).
**CNA port behaviour:** All game logic runs locally.  No networking.  Win detection
and random-AI moves are implemented client-side.
**Root cause:** Windows Phone APIs (`Microsoft.Phone.Notification`, WCF client proxies,
`IsolatedStorage`) are not available on desktop CNA.
**Tracked in:** not planned (phone-specific feature)

## Touch input replaced with mouse input
**XNA behaviour:** The original uses `TouchPanel.IsGestureAvailable` / `ReadGesture()`
with `GestureType.Tap` to detect where the player taps the board.
**CNA port behaviour:** Left mouse-button click on a board square is used instead.
**Root cause:** `TouchPanel` / touch gestures are phone-specific; not in CNA.
**Tracked in:** not planned (platform adaptation)

## Button widget replaced with keyboard shortcut
**XNA behaviour:** Three touch-operated `Button` widgets appear during gameplay:
"Send Move" (confirms a selected square), "New Game", and "Exit".
**CNA port behaviour:** The player clicks directly on the desired square to place a mark
(no separate "Send Move" confirmation needed).  After game over, **N** starts a new game
and **ESC** exits.  No rendered button widgets.
**Root cause:** The original buttons rely on `SpriteFont`-rendered labels and touch
gesture events; the simpler direct-click model fits a desktop better and removes the
need for a separate confirmation step.
**Tracked in:** not planned (platform adaptation)

## Portrait phone layout adapted to landscape desktop window
**XNA behaviour:** Designed for a 480×800 portrait phone screen.  Board centred in
the middle third; status text and buttons occupy the upper and lower thirds.
**CNA port behaviour:** 800×600 landscape window; board centred horizontally and
slightly above centre; status text displayed below the board.
**Root cause:** Phone portrait layout does not translate directly to a desktop window.
**Tracked in:** not planned (layout adaptation)

## SpriteFont buttons omitted
**XNA behaviour:** Button labels ("Send Move", "New Game", "Exit") are drawn using
`SpriteFont` (`ButtonFont`).
**CNA port behaviour:** No button widgets rendered; keyboard shortcuts used instead
(see "Button widget" entry above).
**Root cause:** Follows from the input model change.
**Tracked in:** not planned

## Font: "Moire" substituted with a generated bitmap font
**XNA behaviour:** `TextFont.spritefont` and `ButtonFont.spritefont` both specify
`<FontName>Moire</FontName>` (a Windows-only font), at 40pt and 20pt respectively —
`TextFont` for the status text, `ButtonFont` for the three button labels.
**CNA port behaviour:** A single `Content/font.font.json` + `font.png` atlas is
generated via `tools/make_font.py` (substituting an available TrueType font), at a
much smaller pixel size suited only to the single-line status text at the bottom of
the board — the second, button-sized font was dropped along with the button widgets.
**Root cause:** "Moire" is a proprietary Windows font not freely distributable, and
CNA has no `.spritefont`/TTF-at-runtime pipeline; `make_font.py` bakes a static atlas
instead, per this repo's standard SpriteFont substitution convention.
**Tracked in:** not planned (acceptable substitute); DEFERRED.md item 2 (SpriteFont
pipeline) already resolved for the atlas mechanism itself.

## Fullscreen and 30 fps target omitted
**XNA behaviour:** The constructor sets `graphics.IsFullScreen = true` and
`TargetElapsedTime = TimeSpan.FromTicks(333333)` (~30 fps), commented "Frame rate is
30 fps by default for Windows Phone."
**CNA port behaviour:** `TicTacToeGame`'s constructor sets neither — the game runs
windowed at 800x600 and at CNA's `Game` default timestep (~60 fps), twice the
original's frame rate.
**Root cause:** Phone-specific settings outside the scope of a desktop port; several
other Windows-Phone-derived samples in this repo do port the explicit 30 fps
timestep, but this sample's port did not.
**Tracked in:** not planned.

## GamePad Back button dropped, keyboard-only exit
**XNA behaviour:** `Update` unconditionally polls
`GamePad.GetState(PlayerIndex.One).Buttons.Back` every frame (in addition to the
touch-driven "Exit" button) and exits the game when pressed.
**CNA port behaviour:** Only `Keyboard::GetState().IsKeyDown(Keys::Escape)` is
polled; no gamepad equivalent is wired up (CNA's `GamePad` API exists and is used
elsewhere in this repo, e.g. `GamePad::GetState`/`IsButtonDown`).
**Root cause:** Simplification for a desktop-only port; not a CNA limitation.
**Tracked in:** not planned

## First-turn probability changed from 40/60 to 50/50
**XNA behaviour:** `GameInitialize` (and `newGameButton_Click`'s restart) picks the
first player with `random.Next(0, 10) > 5` — true for 4 of the 10 possible values
(6,7,8,9), so the human player goes first only 40% of the time and the AI goes first
60% of the time.
**CNA port behaviour:** `InitBoard()`/`ResetGame()` use `random_.Next(0, 2) == 0` to
pick the AI, giving a fair 50/50 split between player-first and AI-first games.
**Root cause:** Port simplification/oversight — the original's skewed 40/60 odds
were not reproduced; CNA's `System::Random::Next(min, max)` has the same exclusive-
upper-bound semantics as .NET's, so the exact odds could be replicated with
`random_.Next(0, 10) > 5`.
**Tracked in:** not planned (cosmetic gameplay-balance difference, not a CNA
limitation).
