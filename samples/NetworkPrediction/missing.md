# Missing / Differences from the XNA 4.0 Original

## Status

**Current status: `🛑`, reanalyzed on 2026-09-28.** Retained Release native multiplayer
still works; current-chain rebuild/sign-in qualification and browser multiplayer remain open.
The analysis below distinguishes the old products from current CNA, whose Gamer Services backend
changed during this session. `SAMPLES-DEC-006` still applies; no native-only exception is approved
for this row. The earlier 2026-09-01 implementation and qualification are preserved as history.

The old sample-local `PacketKind` / options-packet substitute has been removed.
`NetworkPredictionGame` now uses the original XNA
`NetworkSession.SessionProperties` contract directly. CNA commit
`c195fe8ce` implements the missing mutable, host-authoritative, automatically
replicated properties behavior and covers it with native codec, session and real-ENet
regressions.

The complete unchanged original, build products, scripts and evidence are retained at:

```text
/rv/tmp/samples/SAMPLE-100-NetworkPredictionSample_4_0
```

Authoritative source:

```text
/rv/tmp/XNAGameStudio/Samples/NetworkPredictionSample_4_0
```

## Source and package audit

The port was checked line by line against all three original runtime sources:

- `NetworkPredictionGame.cs` (694 lines);
- `Tank.cs` (418 lines);
- `RollingAverage.cs` (109 lines).

The Windows and Xbox projects, content project, assembly metadata, six documentation
figures, HTML documentation, screenshot and Microsoft Permissive License were also
audited. The selected reference product is the original Windows/Reach build.

The C++ source now preserves the original namespace and type decomposition:
`NetworkPrediction::NetworkPredictionGame`, `Tank` and `RollingAverage`.
Declarations and implementations are separated into matching headers and source
files. Update/draw ordering, packet cadence, interpolation, prediction, smoothing,
rolling averages, tank/turret input, session lifecycle, gamer events, text, constants,
defaults and all keyboard/gamepad mappings follow the C# source.

The prior F1 overlay is gone. Historical `help.png` is retained only at the sample
root and is not copied or loaded. The original package documentation and artwork are
retained.

## CNA framework repair: live SessionProperties replication

The original host writes these four values every frame:

```text
SessionProperties[0] = network quality
SessionProperties[1] = frames between packets
SessionProperties[2] = prediction enabled
SessionProperties[3] = smoothing enabled
```

Clients read the same indices without any application packet. The former port instead
invented an explicit options packet, modified the tank packet envelope and maintained
sample-local copies. That violated the zero-workaround rule.

CNA commit `c195fe8ce` fixes the owning layer:

- adds the mutable getter required by XNA's get-only mutable collection property;
- serializes a complete nullable `NetworkSessionProperties` snapshot;
- includes the authoritative snapshot in the server welcome;
- detects host mutations during `Update()` and broadcasts reliable snapshots;
- applies updates only from the transport host;
- preserves count, null and signed `Int32` values;
- rejects non-host publication attempts.

Regression evidence from the CNA task:

- 29/29 focused codec, session and real-ENet tests passed;
- 289/289 broad network tests passed;
- the real two-process loopback test observed post-join property mutation at the
  client without an application packet;
- 14/14 enabled strict XNA/module/C-ABI gates passed (two configuration-disabled
  gates skipped);
- the full Debug build completed in 998 steps;
- Release OPENGLES3 C API + Net completed in 165 steps, including shared/static
  outputs and the export audit.

The sample now contains no `PacketKind`, options packet, manual replication or other
network substitute. `UpdateOptions()` is once again the direct translation of the
original.

## Content

The old loose PNG/font-sidecar substitutes were removed. The sample loads the exact
official Windows/Reach products through the original identifiers:

| Asset | SHA-256 |
|---|---|
| `Content/Font.xnb` | `62e04ac27f0a10f46424bdae3c53d9371e164e20480aa77f0ee3e88796ac2d59` |
| `Content/Tank.xnb` | `3f171b4448ed8e33767173137073593333b673e65628598b564e0a6ccc35b2c0` |
| `Content/Turret.xnb` | `5c7cae093a829276b215c0d16cb26cb80e829e030c518510efb78bb912db37d9` |

The retained original `Font.spritefont`, `Tank.tga` and `Turret.tga` inputs are
byte-identical to the corresponding ClientServerSample inputs. A fresh run of the
official XNA 4.0 Windows/Reach content pipeline reproduced all three hashes exactly.
There is no font substitution or offline image conversion in the current port.

## Necessary C++ translation mechanics

The only source-shape differences are lossless C++ mechanics:

- caller-owned XNA objects use `std::unique_ptr` / `std::optional`;
- gamer tags keep non-owning `Tank*` values inside `std::any`;
- session destruction raised from `SessionEnded` is deferred until the current
  `NetworkSession::Update()` callback returns, avoiding destruction of the object
  whose method is on the C++ stack;
- `GetTypeName()` uses the exact logical name
  `NetworkPrediction.NetworkPredictionGame`.

No gameplay branch, packet field, renderer state, input or screen was added or
removed.

## Original XNA qualification

`scripts/build-original.sh` builds the unchanged C# sources with the official XNA
4.0 Windows/Reach references and content pipeline. It produced
`NetworkPrediction.exe` and the three byte-identical XNBs above.

The executable was run offline under isolated Xvfb and WineD3D with:

```text
CNA_XNA40_WINEPREFIX=/home/robertvokac/.wine-cna-xna40
WINEDLLOVERRIDES=d3d9=b
```

It reached and rendered the authentic A/B System Link menu. Wine's offline
Games-for-Windows-LIVE host cannot complete the System Link create/join UI, so this
run is not claimed as an original two-machine LAN qualification. The unchanged source
and original menu are the behavioral reference; CNA's transport is tested separately
with real peers below.

## Native CNA qualification

All builds used `CCACHE_DIR=/rv/cnaccache` and at most eight parallel jobs.

- the existing Debug `NetworkPrediction_cna_samples` target passed;
- a clean Debug OPENGLES3 artifact build completed in 748 steps;
- a separate clean Release OPENGLES3 artifact build completed in 748 steps;
- two real sample processes on isolated Xvfb created and discovered a System Link
  session, joined, exchanged tank input/state and remained synchronized;
- holding Right on the client for four seconds moved the client-controlled tank;
- the host changed Typical -> Poor -> Perfect, 10 -> 20 packets/s, prediction on ->
  off and smoothing on -> off;
- the client independently displayed all four replicated values
  (`0 ms`, `0% packet loss`, `20`, `off`, `off`);
- the complete 1067x300 gameplay/tank area of host and client captures was
  pixel-identical (absolute error 0). The expected full-frame difference was only the
  host-only input hints.

This is direct regression evidence for the defining
`NetworkSession.SessionProperties` behavior, not merely a successful link.

## Browser qualification and remaining boundary

The clean Release WEBGL2 build completed all 643 steps. The generated bundle was
served over local HTTP and exercised in system Google Chrome:

- all `.html`, `.js`, `.data` and `.wasm` assets returned HTTP 200;
- CNA reported renderer `WEBGL2`;
- Chrome returned `WebGL 2.0 (OpenGL ES 3.0 Chromium)`;
- the exact 1067x600 A/B menu rendered;
- pressing A created the local session state and rendered the tank, gamertag and all
  four option lines;
- 600 further browser animation frames completed;
- no runtime exception, fatal console message, relevant HTTP failure or unhandled
  rejection occurred.

This does **not** qualify the sample's defining browser multiplayer behavior.
`ENetDiscoveryService::FindSessions()` is deliberately an empty result under
Emscripten, and a browser cannot expose CNA's inbound System Link host endpoint.
Therefore a second browser instance cannot execute the original B=find/join route,
receive the host's properties or participate in prediction/smoothing gameplay. The
single-tab host screen is useful render/input evidence but is not a substitute for a
real peer.

Under `SAMPLES-DEC-006`, completion requires either a reusable browser
directory/broker/relay and multi-peer qualification, or an owner-approved native-only
scope boundary. No fake lobby, manual address control or sample-local WebSocket path
was added.

## Controls

- Menu: A creates a System Link session; B finds and joins one.
- Tank: arrows move; K/O/L/semicolon aim the turret.
- Host options: A changes network quality; B changes packet rate; X toggles
  prediction; Y toggles smoothing.
- Gamepad mappings remain the original A/B/X/Y and thumbstick controls.

## Historical classification — 2026-09-01

`SAMPLE-100` is therefore **native-complete and workaround-free**, but remains
`🛑` solely for the browser multiplayer decision recorded in
`SAMPLES-DEC-006`.

## Current-head analysis — 2026-09-28

### Reference and faithful translation

All **22 physical upstream files / 392,357 bytes** match `xna4-original/` byte for byte.
The three runtime C# units, assembly metadata, both solutions/projects, content declarations,
font definition and original documentation were reopened against the complete C++ port.
Windows/x86 Debug Reach is the retained reference; Xbox uses the same game sources. There is
no Phone/touch path, application thread, audio, custom content processor/reader or XML serializer.
The original `Game.ico` is present in the snapshot but absent from the port root; restoring it and
including it in the original compiler invocation is a remaining metadata repair.

This is a **2D network-motion demonstration**, not a tank combat game. Up to 16 gamers, four
local, drive and aim tanks on a 1067×600 blue field. It compares uncorrected remote motion with
prediction and smoothing. The original 100-sample rolling clock average, three tank states,
60 Hz prediction, inertia/friction, screen clamp and linear smoothing are preserved.
Each original application packet contains eleven floats (44 bytes): timestamp, position,
velocity, body/turret rotations and both input vectors. Its `InOrder` cadence cycles between
10, 20 and 60 packets/s. Host quality cycles through 100 ms/10%, 200 ms/20% and 0 ms/0% loss;
prediction and smoothing toggle independently. Four ordinary session properties replicate the
options. No extra options packet or network envelope exists in sample code.

The manually checked bypass scan contains only logical type identity and the original three
`Content.Load` calls. The session-ended deferral is C++ lifetime handling. No sample workaround
was found. All three checked-in XNBs still match both native deployments and the official
original deployment. Their reader tables contain only stock Texture2D/SpriteFont/container
readers. The retained 169,508-byte web `.data` is exactly the sorted original XNB concatenation.
No new content generation is claimed in this analysis.

### Fresh execution of retained products

The retained original executable, SHA-256
`47830ccf69719d044ad9bd40aa3f9d9f4c564973052a2ea3c3f1465e79d78b56`, again runs with the
established Wine prefix and `WINEDLLOVERRIDES=d3d9=b` on an owned Xvfb display. It renders the
authentic menu. A/create returns **“An error occurred while accessing the network.”** after
GFWL sign-in; Escape exits with code 0. Original multi-machine gameplay is not qualified.

Two retained Release OPENGLES3 processes on separate owned displays create/discover/join,
replicate client movement and exercise all three qualities, all three packet rates and both
prediction/smoothing states. All four options appear independently at the client. The complete
1067×300 gameplay area is pixel-identical in all five recorded stationary comparisons; full-frame
host-only option hints differ as expected. Both processes exit on Escape with code 0.
Original/native screenshots were visually inspected. This verifies the **retained** products,
not a fresh build of current CNA. The old binary still shows its old generic “Stub Gamer” identity.

The initial small-display capture clipped 27 pixels, and raw OCR misread the outlined font.
Those probe failures remain as history. A sufficiently large owned display and an OCR derivative
of the unchanged screenshot resolve the harness gates; actual final windows are 1067×600.
Neither game geometry nor input providers were changed.

### Current CNA is newer than the retained binary

Analysis started at CNA `next b2fd47a45`; a concurrent Gamer Services integration advanced it
to **`9473f5c8972027d0a5f3e484dc5e4374bd095669`**, the commit pinned by this source analysis.
Sharp Runtime remains `fc033a0e`. `modules/net` is unchanged across those heads: authoritative
welcome/property broadcasts and nullable property snapshots remain implemented generally.
The earlier 29/29 and 289/289 test results above are historical; this analysis reruns no framework
test suite and makes no new framework test claim.

Gamer Services **did** change. `GamerServicesDispatcher` no longer fabricates signed-in profiles.
The default `OnlineBackend` uses configured service accounts, libcurl/TLS and a worker thread.
`Guide.ShowSignIn(4, false)`, which the original game calls with no signed-in gamer, now throws
`GamerServicesNotAvailableException` if no account service is configured. Guest/offline sign-in
remains unfinished in the framework plan. Thus a fresh native port needs actual Guide sign-in
and externally configured endpoint/title/accounts, followed by a real two-process test. No
sample-local fake identity or sign-in bypass is acceptable. The original game's own source
does not require Xbox LIVE PlayerMatch or invitations; this account dependency belongs to the
current CNA backend, separately from its native System Link transport.

The module's unconditional CURL dependency, worker and documented unverified browser control
transport also need qualification before asserting a fresh nonthreaded web build works. The
original has no application thread requirement, but that alone no longer proves the new service
backend can run in the ordinary static-host ABI. The in-progress general service plan tracks
directory/matchmaking at GS-007 and relay at GS-008; neither implies a measured browser
System Link create/find/join route for this unchanged sample.

### Two independent browser boundaries

1. **Old bundle deployment:** ordinary HTTP fails before game startup with `DataCloneError`:
   SharedArrayBuffer transfer requires `crossOriginIsolated`. Canvas stays 300×150, with no graphics
   context. This is the retained threaded bundle's deployment/configuration problem, not a tank
   algorithm defect. Its build scripts still use retired openeggbert paths, omit `CCACHE_BASEDIR`
   and cap jobs at eight; refresh them to the active chain/shared cache/all cores when work resumes.
2. **Actual peer transport:** with COOP/COEP, two independent real Chrome pages render WebGL 2
   at 1067×600. A/create produces the local tank/options. B/find on the second page renders
   **“No network sessions found.”** Host rendering continues for 600 frames; both pages dispose
   their graphics context on Escape. There are zero required HTTP errors, runtime exceptions or
   unhandled rejections in this isolated-header run. It is a negative multiplayer test, not a pass.

Current `ENetDiscoveryService` still makes registration/polling no-ops and returns an empty
`FindSessions` vector under Emscripten. `NetworkSession.EndFind` calls that implementation.
The browser host branch still requests a bound ENet endpoint; its outbound-only client path
needs an address that the original public find/join flow cannot obtain. Removing pthread flags
cannot fix this. A general directory plus browser-capable relay/address handoff must preserve
logical host authority, gamer events, `InOrder` tank packets and automatic properties. Native
host packet forwarding already exists; it is not the missing browser-facing broker/relay.

### Next implementation boundary and evidence

Refresh metadata/build scripts and rebuild against the selected committed chain; qualify real
service sign-in plus native peers. For browser completion, integrate and test the general service
control transport and session directory/relay with two real peers, or obtain an explicit
SAMPLE-100-only native scope decision. Existing 62/91 exceptions do not extend here. No game,
framework, content or retained product was changed/rebuilt/pruned during this analysis, and no
menu-only gallery entry was published.

Evidence: `evidence/current-head-analysis-20260928/` contains `inventory.json`, `xnb-readers.json`,
`current-network-source.json`, `no-workaround-scan.txt`, `review.json`, `desktop/`, `web-plain/`
and `web-isolated/`, plus retained probe setup failures. Safe reproduction helpers are
`scripts/analyze-retained-desktop-20260928.py` and
`scripts/analyze-retained-web-20260928.{py,mjs}`; the latter's `--isolate` flag serves COOP/COEP
and tests both actual pages. Browser profiles and only the helpers' own processes/displays are
cleaned up. Closing documentation/publication heads are in `final-heads.json`.
