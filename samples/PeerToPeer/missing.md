# Missing / Differences from XNA 4.0 original

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
