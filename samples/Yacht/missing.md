# SAMPLE-071 — Yacht

## Current-head qualification — 2026-09-27

**Status: `✅`.** Both original products remain in the port: the Windows
Phone/Reach client and the separate WCF game server. The owner chose this
boundary and the original transport in `SAMPLES-DEC-009`; the browser can play
offline, while native client/server exercise the complete online path. The
artifact root is `/rv/tmp/samples/SAMPLE-071-Yacht_4_0/`; its updated
`MANIFEST.md` and `scripts/` reproduce the products and gates. No artifact
prune or push was authorized for this completion.

The unchanged physical original has 109 files and still matches
`xna4-original/` exactly. `scripts/build-original.sh` rebuilt the Windows
Phone/Reach content, original `Server.exe`, and a **diagnostic type-check** of
the unchanged phone client with a missing-SDK shim. This is not an original
phone gameplay run; the offline Win7 VM has no `/dev/vboxdrv` on this host.
All 45 checked-in XNBs match the freshly rebuilt Microsoft pipeline and
current native product byte for byte (`evidence/requal-20260926/content-identity.json`).

The current OPENGLES3 build from the active CNA and SharpRuntime checkouts
produces `Yacht_cna_samples`, `YachtServer_cna_samples` and `libcna.so` under
`cna-native-opengles3/samples/Yacht/`. Its RUNPATH names the current CNA
checkout and artifact, not retired `openeggbert/cnanext`. On private Xvfb,
the client passed Main Menu → Offline Game → rules → name prompt → board →
ROLL, where the five dice changed and `ROLLS X3` became `X2`.
`scripts/check-native-online.sh` passed Online Game → registration → lobby →
New Game → game-name prompt → board with Player1 and the server's AI1–AI3;
the original waiting/start state appeared. The original Exit menu entry ended
the native process with status zero. Captures and logs are in
`evidence/requal-20260926/{native,online,exit}/`.

`scripts/check-soap-current.py` ran both original and ported servers on
localhost with the same Register, NewGame and GetAvailableGames requests.
Both returned session 100, a sixteen-byte game GUID and the original doubled
`Message` payload. The source port now supplies the original WCF host's AOT
contract metadata through SharpRuntime's existing `ServiceHost::SetMetadata`:
`?wsdl`, `?xsd=`, `?xsd=xsd0` and `?xsd=xsd1` returned **byte-identical**
responses from both servers (WSDL 10,100 bytes). The metadata header was
generated from fresh captures of the unchanged original; its SHA-256 values
and the paired replies are in `evidence/requal-20260926/soap/`.

The threaded Release WEBGL2 bundle is in `cna-web-webgl2/samples/Yacht/`.
Original `System.Threading.Timer` calls require pthreads. The active CNA
threaded WasmFS default could not link its IDBFS storage module, so CNA now
offers `CNA_EMSCRIPTEN_USE_WASMFS=OFF` for games needing persistent storage.
That general configuration mounts `/cna-storage` and SharpRuntime's `/save`
over IndexedDB before `main`; a separate configure probe confirmed the WasmFS
default omits IDBFS rather than producing an invalid link. Yacht selects the
persistent configuration in `scripts/build-web-current.sh`. Real system
Chrome passed offline menu/rules/name/board/ROLL and 600 more animation frames
with WebGL 2, `crossOriginIsolated=true`, both IndexedDB stores present, and
zero uncaught exceptions, rejected promises or HTTP errors. The exact
four-file bundle is staged in `samples.libcna.com/Yacht/`. Its ordinary-HTTP
`launch.html`/scoped service worker also passed the same Chrome gameplay gate;
the gallery card and detail image show actual rolled gameplay.

Audio was measured instead of inferred from XNB loading: the native dice-roll
recording reached −0.4 dB peak and the final real-Chrome recording reached −0.3 dB
peak. Each game stream was identified and moved to a temporary virtual sink,
so the owner's speakers were untouched. Evidence:
`evidence/requal-20260926/{audio,web-audio}/volumedetect.log`.

The original `WINDOWS`, `XBOX` and `WINDOWS_PHONE` input arms are preserved
under matching port macros; the selected Yacht target defines only
`YACHT_WINDOWS_PHONE`. Both inactive arms passed separate syntax checks.
The port still contains only the documented mouse-to-touch, Guide overlay
and PhoneApplicationService platform seams; it has no raw-content bypass or
sample-local renderer workaround. `diff.md` records the AOT metadata and
other necessary C++/phone differences. The browser online path retains its
owner-approved raw-socket boundary: online play requires the native client
and server, because the original transport was deliberately not modernized.

## Pre-implementation analysis — 2026-09-26

**Status at this checkpoint: `🔎` pending current-head qualification.** The September 2026 port
and its owner decision (`SAMPLES-DEC-009`: keep both client and server, without
modernizing the transport) remain the baseline. This analysis changed no Yacht
source, content or runtime code. It does not treat the old binaries as tests of
CNA `next 8a67da536` or SharpRuntime `next d86adb65`.

The physical upstream `Yacht_4_0` has 109 files; all 109 match the preserved
`xna4-original/` snapshot byte for byte. The selected client is a Windows
Phone/Reach product and the separate `Server.exe` is a WCF console service.
All 45 checked-in XNBs match both the official retained
`xna4-build/Content-phone/` output and the retained native product byte for
byte, with no loose assets in `Content/`. The original server still runs here:
on 2026-09-26 its WSDL returned HTTP 200 and 10,100 bytes, including the
checked Register, NewGame, GetAvailableGames, GameStep, GetGameState and
GetScoreCard operations. Evidence:
`evidence/current-head-analysis-20260926/{inventory.json,original-server-wsdl.json,original-server.log}`
under `/rv/tmp/samples/SAMPLE-071-Yacht_4_0/`. The unchanged phone client
has historically only been type-checked as `Yacht.dll` using a labelled
Windows-Phone-SDK diagnostic shim; no original phone gameplay run is claimed.
The offline Win7 VM is unavailable on this host (`/dev/vboxdrv` is absent).

The retained 2026-09-09 native client and server binaries are 7,725,016 and
950,416 bytes; both still carry an SDL `RUNPATH` into the retired
`openeggbert/cnanext` checkout. The 2026-09-08 web bundle is 9,469,886-byte
WASM plus 11,506,286-byte data and its JS has 36 `PThread` markers. Its old
Chrome gate required `crossOriginIsolated=true`; the old script serves special
COOP/COEP headers, and the evidence establishes offline board/roll and 600
frames on that historical build, not the current source chain. Threads are
required by the original game's `System.Threading.Timer` paths in `Dice` and
`GameplayScreen`; today's SharpRuntime explicitly refuses Timer on an
Emscripten build without pthreads. The gallery already has a scoped
`launch.html`/`coi-sw.js` pattern for threaded games, but has **no Yacht
entry**. Its online browser path remains limited by raw socket and incoming
notification transport; the native client/server path is the complete one.

The current CNA phone/Guide/touch APIs and SharpRuntime ServiceModel/XML APIs
still exist. A targeted scan found the three documented `CNAEXT` phone-shell
seams, not a renderer-specific loader or loose-content bypass. One source
fidelity item needs review before renewed `✅`: original `MenuScreen.cs` has
inactive Windows and Xbox input branches, while the port retains only the
selected phone arm. Other shared screen/input files also have phone-guarded
source to compare against the current rule to preserve relevant conditional
branches. The historical native gate used dummy audio and the Chrome gate
muted audio, so neither measured audible PCM for the 14 sound XNBs.

For implementation: repair the old build/capture helpers and manifest (they
still name retired `openeggbert` paths; the native helper writes a scratch map
to `/tmp/claude-1000`, and two web-helper comments/profile names say sample
70), rebuild both native targets and the threaded WEBGL2 bundle on active CNA
and SharpRuntime, rerun the original and ported SOAP exchange plus native
offline/online, timer and sound paths, and re-gate the web offline path with
real Chrome/Firefox and measured sound. Then add a real gameplay gallery card
and the established scoped isolation launcher, subject to the documented
browser online boundary. The old `✅` evidence remains below; it does not
replace these current-head checks.

## Historical qualification — 2026-09-08

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

    Not worked around here: the mode is a deliberate CNA extension with a public opt-out,
    `GraphicsDeviceManager::setPreferredPresentationModeProperty`, and calling it from this
    sample would be exactly the workaround `rules.md` forbids. **The project owner decided on
    2026-09-08 that the default should be `Letterbox`**, which is what XNA and FNA do, and it
    landed in cnanext `f13701188`: the manager's default, `GraphicsRendererCreateArgs` and every
    renderer's own fallback now agree, with the exact 480x800-into-800x600 numbers pinned by
    `EasyGLSurfaceState.LetterboxKeepsTheLogicalSizeAtTheBackbufferAndCentresTheRect`. The other
    four modes stay reachable through the setter. Both of this sample's products were
    re-captured afterwards and are unchanged.

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
