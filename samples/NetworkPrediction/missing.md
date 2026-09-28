# Missing / Differences from the XNA 4.0 Original

## Status

**Current status: `⏸` deferred by explicit owner instruction on 2026-09-28.**
The owner requested complete removal from the web gallery and then deferred SAMPLE-100.
The previous native-port/status-page acceptance is superseded. Keep the faithful source, native
products, exact original/content and evidence for future resumption; no artifact prune was requested.

The qualified native run used two real accounts and a temporary private TLS service that was
stopped after testing. Direct startup without `CNA_GAMER_SERVICES_ENDPOINT` and a provisioned
`CNA_GAME_ID` throws `GamerServicesNotAvailableException`: general CNA offline/guest profiles are
unfinished. The sample itself uses System Link. Browser account transport, directory/discovery,
relay/handoff and real-peer behavior also remain unqualified. No sample workaround was added.

The gallery card, detail page and both screenshots are removed; its seven pages again contain
82 entries and Custom Model Importer is the final detail page. Historical qualification below
records the earlier accepted scope and does not claim present completion or publication.
See `diff.md` for the revised owner decision. Removal verification is retained in
`evidence/gallery-removal-20260928/`: all 82 unique cards/counts, adjacent navigation,
real-Chrome desktop/mobile layout, no runtime/resource errors, and HTTP 404 for the deleted
page and images. The gallery tree exactly matches its pre-SAMPLE-100 state.

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

## Historical original XNA qualification — 2026-09-01

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

## Historical native CNA qualification — 2026-09-01

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

## Historical browser qualification and remaining boundary — 2026-09-01

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

## Historical current-head analysis — 2026-09-28 (before implementation)

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


## Historical qualification — 2026-09-28, earlier native scope accepted

### Owner decision and source fidelity

The owner explicitly selected **“Native port + a page with an exact list of browser shortcomings”**
for SAMPLE-100. This resolves this row's DEC-006 scope decision. It approves neither a fake web
lobby nor a menu-only browser release, and does not extend to other samples.

All 22 upstream files / 392,357 bytes still match the exact original snapshot. The complete
runtime translation and Windows/Xbox branches remain as audited above. The original Game.ico
was restored byte for byte. No C++ game logic, packet, input, screen, asset or renderer state
changed in this qualification. `diff.md` records the accepted platform/deployment boundary.

### Fresh products and content

The build helpers now select the active libcna samples/CNA/Sharp Runtime chain, the shared
`/home/robertvokac/.cache/ccache`, `CCACHE_BASEDIR=/rv` and all detected cores. A retired CMake
home is refreshed once; subsequent builds remain incremental. Canonical native output is
**Release**, not the old Debug executable. The original compiler again uses the unchanged C#,
official Windows/x86 Debug Reach references and the exact original icon. The official Windows/Reach
pipeline freshly reproduces all three XNB hashes in the Content table; checked-in, original and
native deployed copies are identical. No conversion or custom reader is needed.

| Current product | Bytes | SHA-256 |
|---|---:|---|
| `xna4-build/bin/NetworkPrediction.exe` | 23,552 | `556d54f166258190d4571342bd340b10dedb6e92b714789254e2ee243ba9dc27` |
| `cna-native-opengles3/samples/NetworkPrediction/NetworkPrediction_cna_samples` | 10,550,176 | `451aa0967c47f69147b2d04535faf6ba415f5e05071b5893f80a07aa2cbb6a18` |

Native build/source qualification uses CNA `c6d9d49de` (the pinned `9473f5c89` plus the two
acceptance repairs) and Sharp Runtime `fc033a0e`. Native RUNPATH selects active CNA's SDL3
installation. No Sharp Runtime or service-server source was edited. The independently active
service repository had other work in progress; the exact **executed service/admin binary hashes**
are recorded in `desktop/result.json`, rather than attributing those binaries to an unproven
source revision.

### Two general CNA repairs, no sample workaround

1. **Guide activity:** an actual sign-in overlay left `Game.IsActive=true`, so the unchanged
   sample's original IsActive guard called ShowSignIn on the following frame and threw
   GuideAlreadyVisibleException. CNA's optional internal overlay now reports modal visibility;
   Game's getter combines that with preserved window focus, and Guide supplies its real visible
   state. Drawing-only overlays remain nonmodal. This matches the real XNA Guide activity
   contract described by [the framework's author](https://shawnhargreaves.com/blog/trial-mode-in-xna-game-studio-3-0.html).
   FNA's desktop Guide has no real overlay to supply this behavior.
2. **Network identity:** LocalNetworkGamer retained “Stub Gamer” in its inherited Gamer fields
   despite holding a real signed-in account. CNA now copies that profile's Gamertag and DisplayName.
   The existing wire roster already carried the real tag. No sample label substitution or protocol
   workaround was added.

The fresh Debug CnaTests target builds. **170/170** selected Game, Guide, local/network gamer,
packet codec and session tests pass through CNA's mandatory private GPU runner. New tests cover
modal activity/focus restoration and both identity fields; an existing join-event test was corrected
from its obsolete “Stub Gamer” expectation. The first 169/170 result is retained with that failure. A test-only follow-up, CNA `8d56fa2fa`,
also checks that a drawing-only overlay leaves Game.IsActive true; that updated existing case
passes separately. Concurrent Sharp Runtime collection-guard work on `feature/gamer-services-collections` appeared
during this last framework test rebuild; it is not a SAMPLE-100 edit and is recorded in the closing-head evidence.
The qualified native product still uses the clean pinned Sharp Runtime revision above.
The older 29/29 and 289/289 results above remain historical; no fresh whole-suite result is claimed.

### Actual account sign-in and native networking

The helper provisions a title and two genuine persisted test accounts in a new, private database,
starts its own TLS loopback service, trusts its generated CA explicitly and uses normal Guide
username/password input in each game. TLS verification is not disabled. Passwords are transient,
passed through stdin, and not placed in game manifests or logs. Empty confirmation on the next
unused local slot dismisses the extra pane. The database contains two authenticated sessions.

Two separate real game processes on owned 1920×1080 Xvfb displays then pass:

- original A=create and B=find/join System Link routes, with no injected address;
- client movement, additional driving and turret input, with state received at the host;
- Typical 100 ms/10%, Poor 200 ms/20% and Perfect 0 ms/0% loss;
- 10, 20 and 60 packets/s;
- prediction and smoothing both on and off;
- all four ordinary host SessionProperties independently displayed at the client;
- four pixel-identical stationary **1067×300** gameplay crops after separating the tanks;
- original Escape exits, code 0 for both processes.

The initial overlapping-tank pair is not pixel-identical and is **not counted** among those four
comparisons. Both peers draw their own roster; the later separated stationary pairs are the
useful equality gate. Full frames also contain the original host-only control hints.
All actual windows are 1067×600. Current screenshots show the real account names, not Stub Gamer.
A separate real-peer run uses only original controls to capture a readable native gameplay image
for the gallery; it is a capture run, not another full qualification matrix.

Fresh unchanged XNA/WineD3D renders the authentic menu and exits 0. A/create still reports
**“An error occurred while accessing the network.”** through the original GFWL host. This is
recorded as a reference-host limitation; original multi-machine gameplay is not claimed measured.
The real original and current native screenshots were inspected.

### Native deployment requirement

Current CNA no longer invents signed-in profiles. Configure a running account service and a
provisioned title externally before launching, then sign in with actual accounts:

```bash
export CNA_GAMER_SERVICES_ENDPOINT=https://your-service.example/cna/v1
export CNA_GAME_ID=your-provisioned-title
# For a private CA, also export CNA_GAMER_SERVICES_CA_BUNDLE=/path/to/ca.pem
/rv/tmp/samples/SAMPLE-100-NetworkPredictionSample_4_0/scripts/run-cna-native.sh
```

This is the current CNA service deployment, separate from System Link tank traffic and separate
from Xbox LIVE. Guest/offline sign-in remains unfinished; without service configuration the
framework refuses ShowSignIn. The sample does not bypass that requirement.

### Exact browser boundary and gallery presentation

The gallery's **NetworkPrediction.html** is a status/detail page with an actual current native
gameplay screenshot, original controls, native setup and these five explicit browser gaps:

1. current browser account-service HTTPS sign-in/lifecycle transport is unqualified;
2. Emscripten discovery returns no sessions; the original create/find/join route needs a directory;
3. browsers cannot expose the native inbound host endpoint; relay and connection handoff are missing;
4. real peer state, authoritative settings, prediction/smoothing and all quality/rate paths are
   unqualified with browser peers;
5. no current deployable browser product is qualified. The old retained threaded bundle needs
   cross-origin isolation; with it a local host renders but another browser finds no session.
   That old product does not verify today's account backend and is not published.

The prior source and real two-page negative browser evidence remain in
`evidence/current-head-analysis-20260928/`. No new WEBGL2 game build/run is claimed in this native
scope. The refreshed web build helper is a future reproduction configuration, not a working release.
The gallery adds the 83rd entry, clearly **Native only — browser version unavailable**, without a
Play link or stale bundle. Its global text distinguishes 82 playable browser entries from this
native-only page. Actual Chrome checks cover the detail, last-page card/counts, all 83 unique cards,
images/navigation, mobile overflow and runtime/resource errors; results are under `gallery/`.

### Reproduction and evidence

All paths below are under the stable SAMPLE-100 artifact root named above:

- `scripts/build-original.sh`, `build-cna-native.sh`, `run-cna-native.sh`;
- `scripts/capture-current-desktop.py` for real service/Guide/native peers plus original Wine;
  `--original-only`, `--gallery-only` and `--evidence` select explicit narrower output;
- `scripts/capture-gallery.py` and `chrome-gallery-status.mjs` for the gallery status page;
- `evidence/requal-20260928/{starting-heads,qualification,final-heads}.json`;
- original/native build logs, `framework-tests-final.log`, `desktop/result.json`, native-gallery
  images and `gallery/result.json`;
- `desktop-guide-regression/` records the actual Guide exception;
  `desktop-identity-regression/` records the failed peer equality with the old local label;
  fixture/OCR/composition probes remain labelled as setup/capture probes, not game passes.

Historical original/native/web products, exact content and prior execution evidence remain.
No pruning occurred. The new before-requalification snapshot's `snapshot-note.json` records a
helper-copy mistake: refreshing four hard-linked obsolete wrappers also changed their saved names.
Two bodies were recovered and hash-verified; the other two obsolete capture bodies retain their
original hashes only, with refreshed bodies labelled `.replacement`. Every saved historical
**product/content** hash still matches. The current safe helpers reproduce the fresh runs.
Closing repository heads and publication state are recorded separately in `final-heads.json`.
