# SAMPLE-071 — Yacht

## Status

`✅` — **both products are ported.** The Windows Phone client and the WCF game server are
each present in full, and each is verified against the original rather than against a
checklist.

`SAMPLES-DEC-009` was decided by the project owner on 2026-09-07: port both products, and
explicitly do not modernize the transport. The measurement behind that decision is recorded
below, because it changed the question: what Microsoft retired is MPNS, and MPNS was a relay
between two halves of this sample. The WCF service is nobody's hosted product — it is
`Server.exe`, which the user runs, and which builds and answers SOAP on this machine today.

## What was here before

19 files and 3,010 lines against the original's 30 files and 8,925 — 27%. Fourteen of the
thirty-eight client types were absent, including the entire online client and the whole
lobby screen. The types that were present averaged about half the original's line count.
`Content/` held 42 hand-made substitutes — 23 PNG, 14 WAV, five fonts rewritten as PNG plus
a JSON descriptor — while the official pipeline's 45 exact XNBs sat unused in the artifact
root. Three things had been invented outright: an F1 help overlay, a parallel mouse input
path beside the original's gestures, and a `MessageBoxScreen` class the original does not
have.

## What is here now

The client is 40 files and 8,711 lines against the original's 30 and 8,925, and a basename
sweep over the original finds no type without a counterpart. The server is its own
executable, as it is in the original.

`Content/` is the official pipeline's 45 XNBs and nothing else.

## How each half was verified

### The wire, measured rather than assumed

The original service runs here under mono, so the SOAP contract was taken off it instead of
read out of the WSDL: its schema fetched from `?xsd=xsd0`, a hand-written `Register`
accepted, and the replies to `Register`, `NewGame` and `GetAvailableGames` captured. Those
files are in `evidence/soap-oracle/` with a README explaining what each settles.

Two findings a specification would not have given:

- **The payload's `Message` element is doubled** — `<Message><Message ContentType=…>`. The
  service serializes the Message through `XmlSerializer`, whose root wrapper is the outer
  element, while `Message.WriteXml` writes its own inside it. A "corrected" writer produces
  a document the original client cannot read, which is why the ported data model reproduces
  it.
- **`byte[]` does not mean one thing.** `NewGame` returns a raw sixteen-byte Guid;
  `GetAvailableGames`, `GetGameState` and `GetScoreCard` return a serialized Message. A
  client that trusted the contract's type would misread one of them.

### The client, against the original service

`System.ServiceModel`'s encoder tests assert byte-for-byte against the captured request, and
`SoapChannelLiveTests` completes the loop against a service that is running: it registers,
creates a game, and reads that game back out of the available list.

### The server, against the original's own answers

Given the very request the original service accepted, the ported server answers with the
original's reply **character for character**. Its `GetAvailableGames` payload is
structurally identical to the captured one — same byte order mark, same declaration, same
doubled `Message`, same attributes; only the Guid differs, as it must.

### Both halves together

Played through: main menu, Online Game, the name prompt, the lobby connecting to the ported
server and reporting no games, New Game, and the board showing Player1 beside the server's
own AI1/AI2/AI3 with "Waiting for other players to join" and the start-against-the-computer
button. The offline game plays too — new game, the rules, the board, and a press on ROLL
that sends five dice tumbling and drops the counter from X3 to X2.

## What the framework needed, none of it worked around in the sample

- **sharp-runtime:** `IXmlSerializable` and an `XmlSchema` stub; `WriteBase64`,
  `ReadContentAsBase64`, `WriteFullEndElement`, `WriteBinHex` and `ReadContentAsBinHex` in
  the XML stack — the sample writes one score card as Base64 and another as BinHex in
  adjacent classes of one file; and `System.ServiceModel`, both the client channel and the
  service host.
- **CNA:** a `Microsoft::Phone` module — `PhoneApplicationService` for the application
  lifecycle, and `HttpNotificationChannel` with its sending half for the push path.

## The two places CNA needs a line the phone did not

Both are one call each, both are recorded where they are made, and both replace something
the phone's operating system did for the application:

- `TouchPanel::setMouseTouchEmulationEnabledEXT` — the game is driven by touch gestures and
  a desktop has no touch screen, so the platform turns the mouse into one rather than the
  game growing a second input path. Verified: a mouse press on "Offline Game" opens the New
  Game sub-menu through the gesture path alone.
- `Guide::RenderPendingMessageBoxEXT` / `RenderPendingKeyboardInputEXT` — the phone's shell
  drew these over whatever was running. Without the call a Guide dialogue is pending and
  invisible, and the game looks frozen; that is exactly how it looked before the line
  existed.

`PhoneApplicationService::AttachEXT` is the third, and it is the one the game itself calls,
in the same constructor the original subscribes from.

## Still open

- ~~**Part of the board does not draw in the browser.**~~ Closed on 2026-09-08, and it was
  never a drawing defect. Every texture was always being drawn; the gate was photographing
  the wrong rectangle.

  The game sets `IsFullScreen = true`, as the original does. A browser may only enter
  fullscreen on a user gesture, so the request sat pending until the gate pressed Enter to
  confirm the player name -- and the canvas then resized from 480x800 to the headless
  browser's **screen** size, 800x600. `chrome-smoke.mjs` measured its screenshot clip once,
  at the menu, so from the board onwards it cropped a stale 480x800 rectangle out of a canvas
  that had moved and changed shape. What looked like missing sprites was the part of the
  board outside that crop.

  Re-measuring the clip on every capture is what made it visible, and it turned up two real
  defects behind it:

  - **CNA killed the game outright on a screen with no matching video mode.** Fixed in
    cnanext, `Sdl3Window::SetFullscreenMode`/`SetSize`: a 480x800 backbuffer is no desktop
    display's video mode, `SDL_GetClosestFullscreenDisplayMode` found none, and the
    `PlatformException` unwound out of `Game::Run()`. Yacht died before its first frame on an
    800x600 screen with `Couldn't find any matching video modes`. The Web and Android
    branches already read "this display has no mode list" as a reason to take the desktop
    mode; a desktop display answering the same way now does too, which is where FNA has
    always been. Regression test: `Sdl3WindowTest.FullscreenAtAPhoneShapedSizeDoesNotEndTheGame`.
  - **`GraphicsDevice.Viewport` is not the backbuffer when the window has another shape.**
    Measured from the running browser build: `viewport=0,0 1067x800  backbuffer=480x800`.
    EasyGL's default presentation mode is `FixedHeightDynamicWidth`, which pins the logical
    height to the backbuffer's and widens the logical width to the window's aspect --
    `800 x 800/600 = 1067`. XNA and FNA both report the backbuffer, always. The game lays its
    leaderboard and buttons out against `Viewport.Width`, so they went to logical x 1067 and
    landed outside the 480-wide area the rest of the board draws in.

    Not worked around here, and not changed in CNA either: the mode is a deliberate CNA
    extension with a public opt-out, `GraphicsDeviceManager::setPreferredPresentationModeProperty`,
    and its default governs every existing game and sample. Calling it from this sample would
    be exactly the workaround `rules.md` forbids, and changing the default is the project
    owner's decision, not this ticket's. **Open for the owner: should the default be
    `Letterbox`, which is what XNA and FNA do, for a game that fixed its backbuffer size?**

  The gate now runs Chrome with `--screen-info={480x800}`, which is the screen this game was
  written for; the viewport is then `480x800`, equal to the backbuffer, and the browser board
  matches the native one sprite for sprite. Evidence:
  `evidence/cna-web-webgl2-qualified/web-board.png` against
  `evidence/cna-native-opengles3-qualified/native-phone-board.png`, and `web-rolled.png`
  against `native-phone-rolled.png` for a roll that actually took (`ROLLS X3` to `X2`).

- **The online half is native-only.** Emscripten cannot open a raw socket, so neither the
  SOAP channel nor the notification channel works in a browser. The web build carries the
  offline game; the online menu entry is reachable and its connection attempt fails as a
  server-unavailable error, which is the same path a native client takes when no server is
  running. Making it work in a browser needs a WebSocket relay, which is a scope decision
  rather than a defect.
- **What MPNS provided and this cannot** is recorded in `HttpNotificationChannel`'s own
  header: reaching a device that is not directly addressable, and delivering to an
  application that is not running. Neither applies to two processes on one machine, which is
  the arrangement the sample's own documentation describes.

## Deviations

Recorded in `diff.md`.
