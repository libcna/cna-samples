# Missing / Differences from the XNA 4.0 Original

## Status

**Native port complete on 2026-09-01; browser multiplayer scope decision pending
under `SAMPLES-DEC-006`.**

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

## Classification

`SAMPLE-100` is therefore **native-complete and workaround-free**, but remains
`🛑` solely for the browser multiplayer decision recorded in
`SAMPLES-DEC-006`.
