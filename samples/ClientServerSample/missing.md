# SAMPLE-091 — ClientServerSample_4_0 audit

## Status

**✅ Complete on the native-only scope accepted by the owner on 2026-09-09 under
`SAMPLES-DEC-006`.** The faithful client/server game works through CNA's real native System Link
transport in two independent processes. WEBGL2 multiplayer is outside the accepted scope: CNA's
Emscripten discovery route is explicitly empty and the original has no direct-address UI or
protocol of its own.

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
- both captures have the same SHA-256; the latest 2026-09-27 current-head rebuild capture produced
  `d8073a40de4f8bf60d147bd180e0af874777bb8fa541ff430024c0111c955f0b`.

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

At the initial audit, `SAMPLES-DEC-006` offered these alternatives:

1. implement a reusable browser session broker/relay plus discovery/address handoff in CNA; or
2. explicitly accept a native-only scope for this networking sample.

The owner selected native-only scope on 2026-09-09; the earlier pending-decision status is
superseded by the accepted scope below.

## Reproduction scripts

- `scripts/build-original.sh`
- `scripts/capture-original.sh`
- `scripts/build-cna-native.sh`
- `scripts/capture-cna-native-two-process.sh`
- `scripts/test-cna-native-clean-exit.sh`
- `scripts/build-cna-web.sh`

The artifact root was subsequently pruned on 2026-09-09; see its `MANIFEST.md` for the retained
original snapshot, runnable native and WEBGL2 products, scripts and evidence.

---

## Re-audited 2026-09-09: the native captures were truncated; fixed and re-taken

Verified first: all three original C# units have counterparts, all three checked-in XNBs are
byte-identical to the official pipeline output, and the source carries no `F1` overlay or
sample-side network workaround.

**The native captures were 1040×600 where the sample's own backbuffer is 1067×600.** Both the
original and the port declare `screenWidth = 1067`, `screenHeight = 600`. Running one instance and
reading the window back:

```
Absolute upper-left X:  240      Width:  1067
Absolute upper-left Y:  120      Height: 600
```

The window is exactly right — CNA honours the requested backbuffer. But 240 + 1067 = **1307** on a
**1280**-wide Xvfb, so 27 columns lay off the right edge and `import -window` returned what was on
the screen: 1280 − 240 = **1040**. `capture-cna-native-two-process.sh` placed no windows and
recorded no geometry, so the truncation was captured and kept as though it were the frame. This is
the third instance of the same defect in this campaign, after SAMPLE-072 (a centred window losing
its bottom 33 rows) and SAMPLE-078 (a screen sized exactly to the window).

The script now uses a 1280×1280 screen, moves the host to `0,0` and the client to `0,640` so both
fit whole, records both geometries, and **refuses rather than record** a capture that is not
1067×600. The three captures were re-taken and are now 1067×600.

**The sample's central claim holds on the corrected captures.** After the client drives the tank to
the clamp, the client's frame and the host's frame are **bit-identical** — RMSE `0 (0)`, ImageMagick
`AE` 0 — which is what authoritative state replicated to a remote peer is supposed to look like.

**Documentation.** The headers carry no `@brief` at 15 % comment density; one of the 24 samples in
that position, recorded as a single deferred item in `plan.md`.

**The browser half is unchanged and is not this sample's to solve.** It needs the session
directory plus inbound-capable peer that `SAMPLE-075`'s re-audit reduced both of its blockers to,
and that the owner declined to build on 2026-09-09. The native half is complete and now has evidence
that measures what it claims.

## NATIVE-ONLY SCOPE ACCEPTED by the owner, 2026-09-09

SAMPLE-091 is complete on a native-only scope and the plan row is `✅` on that basis. The browser
half is explicitly out of scope, not outstanding: a browser cannot open a raw datagram socket or
accept an inbound peer, and the session directory that would work around it is the capability the
owner declined to build when cancelling `SAMPLE-075`.

## Current-head re-analysis, 2026-09-27

- SAMPLE-090 remains owner-cancelled; this review did not reopen it.
- The 16-file upstream directory and retained `xna4-original/` snapshot are byte-identical.
  Both original project files still compile the same two game sources and the same content.
- The port's source and three checked-in XNBs have not changed since `3fe6fc5` (2026-09-01).
  `Font.xnb`, `Tank.xnb` and `Turret.xnb` still match the official XNA build byte for byte.
- The C++ source still uses `NetworkSession::Create/Find/Join`, `NetworkSession::Update`, the
  original per-gamer packet paths and official XNB names. No `CNAEXT`, F1 overlay, manual
  address, synthesized gamer or sample-local network workaround was found.
- On current CNA `next` (`5cc244f23`), native `EndJoin` still waits for `ServerWelcome` and the
  remote host identity. Emscripten `ENetDiscoveryService` still has empty host registration,
  polling and search. The two network-module commits since 2026-09-09 only add exception
  serialization and adjust Windows example linkage; neither changes this transport route.
- The retained OPENGLES3 executable, built 2026-09-09, was run again through
  `capture-cna-native-two-process.sh`: real host/client discovery and join succeeded, both full
  1067×600 post-movement frames matched with absolute pixel difference zero, and both files hash
  to `12675e071725549e369419653e2d284afbac71c1c8c6b002b1f2baf33656417c`.
  This was a rerun of the retained product, not a rebuild against that day's CNA head. The captures
  at these paths were subsequently replaced by the current-head build run below. Its controlled
  teardown killed Xvfb after capture, which left an expected XIO line in both process logs.
- The native gameplay capture labels the framework-provided local identity `Stub Gamer`; the
  original offline Wine profile used `Player1` at its menu. The sample draws the gamertag supplied
  by `GamerServices` in both versions, so this is host identity data rather than a sample-side
  label replacement. Wine/GFWL still has no original multiplayer capture for a pixel parity claim.
- The retained WEBGL2 bundle is present, but no browser multiplayer run is claimed. The owner's
  native-only exception remains the completion basis. No sample/runtime source or gallery change
  was needed. File hashes and product inventory are retained at
  `evidence/current-head-analysis-20260927/inventory.json`.
- The already-pruned artifact root passed a fresh `prune-completed-sample.sh` dry run:
  19.4 MB retained, zero paths proposed for removal at that point. The later native rebuild
  reopened its build tree, as documented below.

## Native OPENGLES3 rebuild on current heads, 2026-09-27

The owner reiterated the existing native-only scope: rebuild and qualify OPENGLES3, with no
WEBGL2 build or test. The sample source, official XNBs, CNA source and Sharp Runtime source were
not changed. Starting heads were cna-samples `develop 19f4a94`, CNA `next 5cc244f23` and
Sharp Runtime `next 9e58c955`.

- The retained `scripts/build-cna-native.sh` was updated from an obsolete checkout path to the
  active three repositories. It configures only `ClientServerSample` in Release/OPENGLES3 with
  the required shared ccache. `CNA_SHARED_LIBRARY=OFF` makes the retained executable independent
  of a `libcna.so` inside the disposable build tree. The final build completed successfully;
  see `evidence/build-native-static-20260927.log`.
- The new executable is
  `cna-native-opengles3/samples/ClientServerSample/ClientServerSample_cna_samples`, SHA-256
  `61738bf4ca31f95597cdc37eb77ad3a6b3672d7e607663d8fa6904ea2d1c3b7a` before stripping.
  Its CNA runtime is statically linked; SDL3 and system libraries remain dynamic. All three
  deployed XNBs remain byte-identical to the checked-in and official XNA files.
- The initial 1280-wide capture attempt revealed that SDL can recenter a window after `xdotool`
  moves it. The capture script now uses a 1920-wide isolated Xvfb screen and continues to reject
  any image smaller than the original 1067×600 backbuffer. Final two-process create/find/join,
  client movement and host/client state comparison passed with absolute pixel difference **0**;
  both complete frames hash to
  `d8073a40de4f8bf60d147bd180e0af874777bb8fa541ff430024c0111c955f0b`.
- A separate fresh-window run held the original Escape input across several frames and confirmed
  a clean exit with process status 0. See
  `evidence/native-opengles3/{capture-result-current.txt,clean-exit-result.txt}`. The network
  capture harness still terminates its two game processes after taking the synchronized frames;
  its XIO lines reflect that controlled Xvfb teardown.
- `evidence/native-rebuild-20260927/inventory.json` records source heads, build configuration,
  product/content hashes and test results. The earlier WEBGL2 bundle was left untouched. A prune
  dry run proposes 13 intermediate paths, about 130.8 MB; no prune was applied in this task.
