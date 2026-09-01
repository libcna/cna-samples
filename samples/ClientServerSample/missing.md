# SAMPLE-091 — ClientServerSample_4_0 audit

## Status

**Native port complete and qualified; overall status: 🛑 owner decision pending under
`SAMPLES-DEC-006`.** The faithful client/server game now works through CNA's real native System
Link transport in two independent processes. It cannot be marked complete because the original
create/find/join contract has no browser implementation: CNA's Emscripten discovery route is
explicitly empty and this sample has no direct-address UI or protocol of its own.

No sample-side networking workaround was added. The native defect exposed by this sample was fixed
in CNA and regression-tested there.

## Source and artifact root

- Exact upstream directory: `/rv/tmp/XNAGameStudio/Samples/ClientServerSample_4_0`
- Stable verbatim snapshot:
  `/rv/tmp/samples/SAMPLE-091-ClientServerSample_4_0/xna4-original`
- Audit/build/evidence root:
  `/rv/tmp/samples/SAMPLE-091-ClientServerSample_4_0`
- Selected original: `ClientServer/ClientServerWindows.csproj`, Debug/x86,
  Windows/Reach. The Xbox 360/Reach project uses the same two runtime sources and the same content;
  it adds no separate game branch.
- Runtime sources audited line by line: `ClientServerGame.cs` and `Tank.cs`; the entry point is the
  `Program` class at the end of `ClientServerGame.cs`. Both project files, the content project,
  `AssemblyInfo.cs`, documentation, images, icon and license were also inspected.

## Original XNA 4.0 result

`scripts/build-original.sh` rebuilds the unchanged source through the official XNA 4.0 compiler
and Content Pipeline. It produced `xna4-build/bin/ClientServer.exe` and these exact assets:

| Asset | SHA-256 |
|---|---|
| `Font.xnb` | `62e04ac27f0a10f46424bdae3c53d9371e164e20480aa77f0ee3e88796ac2d59` |
| `Tank.xnb` | `3f171b4448ed8e33767173137073593333b673e65628598b564e0a6ccc35b2c0` |
| `Turret.xnb` | `5c7cae093a829276b215c0d16cb26cb80e829e030c518510efb78bb912db37d9` |

The unchanged executable starts under the established offline Wine/XNA prefix. On the first run I
declined GFWL telemetry, created a local offline `Player1` profile and captured the real sample menu
at `evidence/original-windows-reach/sample-menu.png`. Its A/B layout, CornflowerBlue background,
font size and positions match the source. `NetworkSession.Create` then remained blocked in the
local Wine/GFWL proxy, so this host cannot provide an original multiplayer capture. This is
recorded as a reference-host limitation, not attributed to the sample. The unchanged source and
Microsoft's local XNA documentation establish that synchronous `Join` blocks until completion.

## Port restored from the original

The prior reduced port was corrected rather than patched around:

- restored `Guide.ShowSignIn`, blocking-operation messages and exact error-line formatting;
- restored the original per-local-gamer server/client receive branch;
- restored the fully-qualified runtime type name and exact `Font`, `Tank` and `Turret` identifiers;
- retained the complete A/B, Escape/Back, arrows and WASD input map;
- retained all gamer/session event wiring, gamer `Tag` tank association, packet layouts, server
  simulation order, client state application, labels and talking color;
- removed the non-XNA F1 help overlay and `CNA/Entrypoint.hpp` use;
- removed the loose PNG textures and generated substitute font atlas;
- checked in the three byte-identical official XNBs above;
- moved historical `help.png` to the sample root, where it is neither copied nor loaded.

The only C++ mechanics are ownership/lifetime translations. `NetworkSession` and tanks use RAII;
the session-ended handler defers destruction until `NetworkSession::Update` has returned, avoiding
destroying the object from inside its own callback. These do not add or remove game behavior.

The final sample contains no `CNAEXT`/backend call, no synthesized gamer, no manual session update,
no direct asset loader, no loose content substitute and no sample-specific network address.

## CNA defect found and fixed

### Reproduction before the fix

The host and client discovered each other, but `NetworkSession::Join` returned before the real
ClientHello/ServerWelcome handshake. `NetworkSession.Host` still named the client's own local
gamer. The original sample immediately sends its 16-byte input packet to `Host` before its first
`NetworkSession.Update`; CNA therefore looped that packet back to the client, which read it as a
17-byte server state record and threw `EndOfStreamException`.

### Root cause and implementation

CNA commit `e20749761` (`fix(SAMPLE-091): complete join handshake before returning`) restores the
XNA contract on the native System Link route:

- `ServerWelcome` installs the authoritative remote host identity immediately;
- native `EndJoin` pumps the owner-thread transport until that handshake completes, with bounded
  timeout, result translation and full failed-session cleanup;
- initial host discovery remains silent while genuine host migration raises `HostChanged`;
- departed remote identities survive migration long enough for `PreviousGamers` and queued event
  arguments, removing a previously masked use-after-free.

The browser route deliberately does not block its event loop; it remains governed by the browser
transport decision below.

### Regression evidence

The new native test uses the public `NetworkSession::Join` route and a real ENet peer. It proves
that immediately on return `Host` is the remote `FakeHost`, then sends an i.d. 5 → i.d. 0 packet
through that identity and verifies its options and payload at the server. Results:

- Debug `CnaNetTests`: **294/294 passed**;
- Release `CnaNetTests`: **294/294 passed**;
- focused join regression with ASan + LeakSanitizer: **1/1 passed, no leak**;
- focused join plus two host-migration lifetime paths under ASan: **3/3 passed**.

The full legacy ASan suite still reports pre-existing fixture/subprocess leaks; the new focused
test is leak-clean and the changed migration paths show no memory error.

## Native OPENGLES3 qualification

Both Debug and clean Release OPENGLES3 targets build against active `../cnanext` and
`../sharp-runtimenext`, with at most eight build workers. The retained Release reproduction is
`scripts/capture-cna-native-two-process.sh`.

That script starts two real executables on one isolated X display, selects A on the host and B on
the client, and exercises the public discovery/join path. Both instances load the three official
XNBs and render both gamers. Holding the client's Right input moves its tank through the network to
the authoritative host. After the position reaches the screen boundary, independent host and
client captures are pixel-identical:

- `evidence/native-opengles3/client-after-move.png`
- `evidence/native-opengles3/host-after-client-move.png`
- ImageMagick absolute-error pixel count: **0**
- both captures SHA-256:
  `44bc3840b8668f99adb95929164ce10c46a888c8ae1b5ea4250078cb1a815c80`

An interactive Debug run additionally proved that the host remains alive after the client
disconnects. There was no `EndOfStreamException` or other game/runtime error after the CNA fix.

## WEBGL2 result and decision

`scripts/build-cna-web.sh` cleanly produces the complete threaded WEBGL2 bundle at
`cna-web-webgl2/samples/ClientServerSample/ClientServerSample_cna_samples.{html,js,wasm,data}`.
The browser-control service exposed no connected browser during this audit, so no real-Chrome
runtime claim is made from this run.

More importantly, the required multiplayer gate is structurally unavailable in live CNA:

- under `__EMSCRIPTEN__`, `ENetDiscoveryService::RegisterHost`, `UnregisterHost` and `Poll` are
  no-ops;
- `ENetDiscoveryService::FindSessions` always returns an empty collection;
- a browser cannot listen for the raw UDP broadcast/unicast protocol used by native System Link;
- the original sample exposes only A=create and B=find/join, with no direct address or broker path.

Consequently a browser client must report `No network sessions found.` and two browser instances
cannot reach the defining synchronized-tank behavior. A menu-only or fake local lobby would violate
the porting rules.

`SAMPLES-DEC-006` must therefore choose one of:

1. implement a reusable browser session broker/relay plus discovery/address handoff in CNA; or
2. explicitly accept a native-only scope for this networking sample.

Until the owner chooses, this row remains 🛑 even though the native C++ translation itself is
complete.

## Reproduction scripts

- `scripts/build-original.sh`
- `scripts/capture-original.sh`
- `scripts/build-cna-native.sh`
- `scripts/capture-cna-native-two-process.sh`
- `scripts/build-cna-web.sh`

No artifacts were pruned.
