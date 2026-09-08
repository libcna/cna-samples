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

- **Part of the board does not draw in the browser, and it is not the sample's doing.** The
  WEBGL2 build runs, is cross-origin isolated, reports a real `WebGL 2.0 (OpenGL ES 3.0
  Chromium)` context, completes 600 post-interaction rAF callbacks and logs no exception,
  rejection, HTTP error or content-load failure. It walks the whole offline flow: main menu,
  the rules, the name prompt through the Guide overlay, and the board. On the board the
  leaderboard, the SCORE and ROLL buttons and the roll counter draw; the background, the
  score card, the holding tray, the roll border and the dice do not.

  What is established: it is not a content-load failure, because nothing is logged and the
  same assets draw natively; and it is not one draw path, because both the missing and the
  present sprites use the same `SpriteBatch::Draw` overloads. Two things separate them, and
  which one matters is not yet known -- the missing sprites are the **larger** textures, and
  they are the ones submitted **earliest** in a batch of roughly sixteen. The single-texture
  screens draw perfectly, instructions included, which is why this did not show until the
  board.

  Reproducer: `scripts/capture-web.sh`, then `evidence/cna-web-webgl2-qualified/web-board.png`.
  The native build draws the same board correctly, so a native/web comparison of one frame is
  the discriminator to run next.

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
