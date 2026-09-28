# Missing / Differences from XNA 4.0 original

## Owner cancellation — 2026-09-28

**Current status: `⛔` cancelled.** The owner explicitly instructed that SAMPLE-103 remain
cancelled after reviewing the current account-service and browser multiplayer boundaries below.
No further port or renewed qualification is scheduled. The existing source, original/native/web
products, scripts and all analysis evidence are retained. The owner separately authorized pruning
later in the same session; the receipt below records that cleanup.
The general CNA offline/guest authentication and browser/public session gaps remain framework work.
This decision is specific to SAMPLE-103 and does not cancel those subsystems or other samples.

## Owner-authorized cancelled-sample prune — 2026-09-28

The owner's subsequent explicit instruction authorized pruning this single cancelled root.
`--allow-cancelled --keep-debug-symbols --apply` removed **229 intermediate paths**;
allocated storage was approximately **392 → 33 MiB**, including the new closure receipts.
All **115 retained pre-existing files** verify unchanged, apart from the expected manifest
replacement. Both original executable directories, historical/current native products, the complete
historical WEBGL2 bundle, source, scripts and analysis evidence survive. Native diagnostic symbols
were intentionally retained. A repeat dry run proposes zero paths. Cancellation remains `⛔`;
this cleanup does not qualify or publish the retained diagnostic products.
Receipt: artifact `evidence/closure-20260928/{receipt,before}.json`, dry/apply/repeat logs,
and the archived pre-prune manifest. The shared prune tool now permits one explicitly named
cancelled root and includes dated `xna4-build*` intermediates while keeping their `bin/` products.

## Current-head re-analysis — 2026-09-28

**Analysis checkpoint status: `🛑`; superseded by the explicit cancellation above.** This is a
source/reference analysis, not renewed multiplayer completion. The existing C++ port remains;
no gameplay, input, content, account substitute or framework implementation was changed here.
The September 1 qualification below is historical and does not qualify today's dependency chain.

### Product and controls

The physical download contains **one runnable game**, with Windows and Xbox 360 projects selecting
the same `PeerToPeerGame.cs`, `Tank.cs` and `Properties/AssemblyInfo.cs`. Both target XNA 4.0 Reach;
the Windows x86 configuration is the desktop reference. All **16 upstream files / 193,084 bytes**
match the complete retained snapshot, including solutions, documentation, license and images.

This is a 1067×600 LAN tank-movement demonstration. It creates a **SystemLink** session for up to
16 gamers, with up to four local gamers per machine. Every machine simulates only its own tanks
and broadcasts their position/body rotation/turret rotation using `SendDataOptions.InOrder`.
The packet is 16 bytes per locally controlled tank per update, with the default 60 Hz game loop.
There is no shooting, score, win condition, custom effect or audio asset. Host status supplies a
label; it does not make the host authoritative for other tanks.

| Action | Original keyboard | Original gamepad |
|---|---|---|
| Create session | A | A |
| Find and join first session | B | B |
| Move tank | Arrow keys | Left stick |
| Aim turret | WASD | Right stick |
| Exit | Escape; Alt+F4 is the host close action | Back |

This sample already has keyboard controls. Mouse-to-touch, accelerometer and orientation
emulation are unnecessary. Xbox project references do not make it an Avatar sample: the selected
game source uses no Avatar API.

### Fresh original/content evidence

The unchanged selected sources again compile against official XNA 4 assemblies under the
established Wine prefix. A separate output preserves the previously qualified original:
`xna4-build-analysis-20260928/bin/PeerToPeer.exe`. The stock official Windows/Reach pipeline
reproduces **all three checked-in XNBs byte for byte**, with the hashes in the table below.
There is no custom processor, content conversion, loose texture or font-sidecar requirement.

On a private Xvfb display, WineD3D (`WINEDLLOVERRIDES=d3d9=b`) again reaches the real local-profile
sign-in and authentic A/B menu. Pressing A produces `An error occurred while accessing the network.`
at both two and ten seconds. Escape exits **0**. This proves source/content startup and the observed
Wine failure; it does not measure original multi-machine LAN gameplay or Xbox hardware behavior.
The direct original route works, so the Windows 7 VM was not needed for this analysis.

### Current CNA boundaries

The fresh Release/static-CNA OPENGLES3 diagnostic builds against canonical sibling repositories,
with both shared ccache exports and all cores. CNA advanced independently during the analysis;
the build was refreshed incrementally at **`next 9373ca58b`**. Sharp Runtime remains the owner's
unchanged **`feature/gamer-services-collections 6c4a857d`** branch. The build log retains compiler
diagnostics from dependencies/optimized framework code; a warning-free acceptance gate is not claimed.

1. **Native bare startup now fails before normal gameplay.** The unchanged menu calls
   `Guide.ShowSignIn(4, false)` when no gamer is signed in. Current `Guide.cpp` requires a configured
   account service; `onlineOnly=false` does not currently provide a real offline/guest profile path.
   The private-display run of the fresh binary exits with SIGABRT and the exact message
   `No CNA account service is configured.` That call is outside the original Create/Join catch blocks.
   This is the same general account-service boundary recently exposed by SAMPLE-100, not a renderer
   failure. A configured service/title and genuine accounts can supply the existing sign-in route;
   doing so for two current SAMPLE-103 peers was not qualified in this analysis. A fabricated gamer,
   skipped Guide call or sample-side catch-and-continue path is not authorized.
2. **Current WEBGL2 configuration fails earlier than the historical web bundle.** Gamer Services
   requires target CURL 7.85 or newer; the Emscripten configure reports missing CURL_LIBRARY and
   CURL_INCLUDE_DIR. A browser account/authentication transport remains unfinished. Passing host
   libcurl into a Wasm build or removing Gamer Services from this game would not solve that contract.
3. **The defining browser multiplayer path remains absent.** The public SystemLink discovery route
   in `ENetDiscoveryService.cpp` returns an empty list under Emscripten. Browser-hosted inbound ENet
   and the session address/peer handoff needed by original B=find/join are unavailable. Complete
   browser gameplay needs a shared directory/hosting/relay path preserving the XNA-shaped APIs,
   broadcast ownership, ordering, roster/events and session teardown, followed by actual multi-peer
   browser qualification. A menu-only page, fake peer or manual-address UI would not qualify it.
4. **New Gamer Services progress is not browser acceptance.** The current living CNA plan records
   a private authenticated directory, server relay/framing and bounded client assembly. Public online
   session integration, actual client relay I/O and browser transport/qualification remain open.
   These checkpoints do not close the original public SystemLink browser route.

The older two-process test establishes historical transport/ownership behavior. Its test counts,
frames and bare-start success must not be presented as measured on the current account backend.
No current browser runtime, authenticated two-peer gameplay, four local controllers or voice path
was measured. No new dependency unit tests are claimed: this task changed analysis documentation
and reproduction helpers, not production framework code.

### Translation review and renewed work

Every selected C# and C++ source, project/content declaration and original documentation was freshly
reviewed. Tank constants, integer spawn arithmetic, angle wrapping, friction/clamping, packet order,
local/remote ownership, notifications, drawing and original controls are preserved. C++ session
destruction is correctly deferred until its executing Update returns; this is a lifetime adaptation.
There is no renderer/network bypass, fake peer, direct-address control, F1 loader or loose-content
replacement in the game.

Before renewed completion, restore original `Game.ico`, `PeerToPeer.png` package metadata and the
Microsoft license at the port root; they are present in the exact snapshot. Replace the unnecessary
local `ReplaceAll` helper with the existing general `System::String::Replace`. Review enum formatting:
the local switch maps the four normal end reasons correctly, but its unknown-value `Unknown` fallback
differs from managed numeric enum formatting. These are bounded translation/metadata tasks, not
evidence that a new content or graphics subsystem is missing. The historical build helpers/cache
also use legacy roots, lack CCACHE_BASEDIR and cap jobs at eight; use the new canonical helpers.

**Decision boundary:** authorize renewed native qualification with a configured genuine service,
including two independently controlled peers and full state/exit paths, or defer this row pending
shared account/browser work. Native-only completion needs an explicit SAMPLE-103 scope decision;
SAMPLE-062/091's exceptions and SAMPLE-100's deferral do not extend to it. Full WEBGL2 completion
requires the general account and multiplayer paths above. This analysis neither cancels the sample
nor implements that subsystem or changes the gallery.

### Evidence and reproduction

Stable root: `/rv/tmp/samples/SAMPLE-103-PeerToPeerSample_4_0/`.
Current evidence: `evidence/current-head-analysis-20260928/`, including `inventory.json`,
`source-audit.md`, original build/products/captures/exit, native build/head refresh/unconfigured
run, failed WEBGL2 configuration and closing heads. All earlier products/evidence survive;
no SAMPLE-103 prune is authorized. The old cache points to `sharp-runtimenext`; the separate
canonical diagnostic tree preserves that frozen product and remains reusable for follow-up work.

```sh
root=/rv/tmp/samples/SAMPLE-103-PeerToPeerSample_4_0
bash "$root/scripts/build-original-analysis-20260928.sh"
bash "$root/scripts/capture-original-analysis-20260928.sh"
bash "$root/scripts/build-cna-native-analysis-20260928.sh"
python3 "$root/scripts/probe-unconfigured-native-20260928.py"
bash "$root/scripts/configure-cna-web-analysis-20260928.sh" # expected target CURL failure
```

The helpers own their displays/processes. The unconfigured probe isolates account configuration,
uses the public game entry point and verifies the actual failure; it does not inject a backend/profile.

## Historical qualification — 2026-09-01

**Status: native port complete; browser multiplayer scope decision pending
(SAMPLE-103, 2026-09-01).** The previous port was not an acceptable reference: it
mixed most of the game into headers, omitted the original blocking-operation messages,
used loose PNG/font sidecars, and added the repository's historical F1 help overlay.
The sample has now been re-audited against the unchanged XNA 4.0 source and restored as
a complete C++ port. Native CNA passes a real two-process peer-to-peer test. The WEBGL2
artifact builds, but the original browser multiplayer path cannot be exercised with
CNA's current Emscripten networking contract, so the row remains decision-blocked under
`SAMPLES-DEC-006` rather than being labelled complete.

The complete audit, build scripts, unchanged-source snapshot, captures, logs, and
manifests are retained outside Git at:

```text
/rv/tmp/samples/SAMPLE-103-PeerToPeerSample_4_0/
```

## Authoritative original and content

The authoritative source is:

```text
/rv/tmp/XNAGameStudio/Samples/PeerToPeerSample_4_0/
```

The unchanged Windows project compiles against XNA 4.0 and the unchanged content
project builds through the official Windows/Reach pipeline. The resulting files are
checked into this port without conversion or substitutes:

| Asset | SHA-256 |
|---|---|
| `Font.xnb` | `62e04ac27f0a10f46424bdae3c53d9371e164e20480aa77f0ee3e88796ac2d59` |
| `Tank.xnb` | `3f171b4448ed8e33767173137073593333b673e65628598b564e0a6ccc35b2c0` |
| `Turret.xnb` | `5c7cae093a829276b215c0d16cb26cb80e829e030c518510efb78bb912db37d9` |

The loose `Tank.png`, `Turret.png`, JSON font description, and generated font atlas
have been removed. The historical `help.png` is retained at the sample root for source
provenance only; it is neither loaded nor packaged as runtime content.

The original executable runs at 1067x600 with the title
`Networking:  Peer-to-Peer`, completes offline Guide sign-in, and reaches the authentic
`A = create session` / `B = join session` menu. On this isolated Wine reference host,
Create truthfully reports `An error occurred while accessing the network.` both two
and ten seconds after activation. That is an offline Games for Windows/XNA host
limitation, not evidence against the source or the CNA transport. Captures are in
`evidence/original-windows-reach/`.

## Restored behavior

The port retains the complete behavior of `PeerToPeerGame.cs` and `Tank.cs`:

- the original `PeerToPeer.PeerToPeerGame` identity, 1067x600 backbuffer, 16-gamer
  and four-local-gamer limits;
- Gamer Services sign-in plus A/B create/find/join and Escape/Back exit;
- the immediate `Creating session...` and `Joining session...` draws before the
  potentially blocking calls;
- System Link session creation, first-result join, available-session disposal, gamer
  join and session-ended handlers;
- one independently simulated tank per local gamer, broadcast to every peer with
  `SendDataOptions::InOrder`, and direct remote position/body/turret state ingestion;
- the original arrow-key/gamepad body input, WASD/right-stick turret input, turn rate,
  acceleration, friction, and screen clamping;
- exact menu/error formatting, tank/turret rendering, gamertag placement, `(host)`
  suffix, and talking-state yellow label.

There is deliberately no host-authoritative simulation branch: host status is cosmetic
in this sample. Each peer advances its own tank and publishes the complete state to all
other peers, exactly as the C# original does.

The only intentional language-lifetime adaptation is session teardown. C# can set its
managed `networkSession` field to null from the `SessionEnded` callback invoked inside
`NetworkSession.Update`. The C++ port disposes there but defers destruction of its
`unique_ptr` until that currently executing method has returned, avoiding destruction
of a live stack receiver while preserving the observable XNA flow. The similarly
scoped available-session collection is disposed by a local RAII guard, matching the
original C# `using` statement.

No F1 overlay, invented controls, automatic menu selection, direct-address shortcut,
fake lobby, alternate packet kind, loose-content fallback, or CNA-internal API is
present. No CNA or Sharp Runtime change was needed for this port.

## Native qualification

The target passes all of the following with OPENGLES3 and Mesa software GL:

- existing Debug build of `PeerToPeer_cna_samples`;
- clean Release configure/build in the artifact directory;
- Debug and clean-Release real two-process runs using separate Xvfb displays, so input
  and windows cannot leak between the two processes while UDP discovery and ENet still
  use the real host network stack;
- CNA Net: 303/303 tests in 33 suites, including all three real
  `TwoProcessLoopback` tests;
- CNA Runtime: 159 passed, two documented platform skips;
- CNA Gamer Services: 368/368;
- focused authentic-XNB/SpriteFont/Texture2D content tests: 135/135;
- focused GraphicsDevice/SpriteBatch/SpriteFont/Texture2D tests: 208 passed, seven
  renderer/platform capability skips.

The two-process test performs the defining topology rather than merely opening two
menus:

1. process A creates a session and process B discovers and joins it;
2. B holds Right until its independently owned tank reaches the right edge;
3. both complete 1067x600 views then match at absolute-error pixel count 0;
4. A separately holds Left until its own tank reaches the left edge;
5. both complete views again match at absolute-error pixel count 0.

The client movement changes 6,145 pixels from the joined host view. The subsequent
host movement changes 13,227 pixels. The synchronized view hashes are:

```text
after client movement: 90beae180747d5e96086916c80ec519ac415eacee235d180ae8d2f6a4983f9e1
after host movement:   ae631d485116ca780cadf36bacb9f68d447442169c7bd6a4e6804686efb4e397
```

This proves bidirectional independent ownership, broadcast delivery, remote state
application, and rendering agreement. The reproducible driver is
`scripts/capture-cna-native-two-process.sh` in the artifact directory.

## Web boundary

A clean Release `CNA_GRAPHICS_RENDERER=WEBGL2` build produces the complete HTML, JS,
Wasm, and preloaded-content bundle. The final link contract contains
`MIN_WEBGL_VERSION=2` and `MAX_WEBGL_VERSION=2` and packages only the three official
XNBs. No browser runtime result is claimed for this audit because the integrated
browser runtime was unavailable in this session.

More importantly, a successful single-tab menu/render smoke could not qualify this
sample's product. CNA's Emscripten System Link discovery route is intentionally empty,
a browser cannot host an inbound ENet peer, and the unchanged sample exposes no direct
address with which to use an outbound-only WebSocket route. Its mandatory B=find/join
path and bidirectional multi-peer tank synchronization therefore require a reusable
browser session directory/address handoff plus relay/hosting design. Adding manual-IP
UI, a local fake peer, or a menu-only success claim would be a sample workaround.

**Tracked in:** `plan.md` decision `SAMPLES-DEC-006`. The owner must either authorize
the reusable browser broker/relay capability or explicitly accept a native-only/non-port
scope boundary. Until then SAMPLE-103 is correctly marked `🛑` despite its complete
native port.
