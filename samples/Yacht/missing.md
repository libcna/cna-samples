# SAMPLE-071 Yacht — fresh audit and blocked fidelity boundary

## Status

`🛑` — the checked-in C++ program is **not a qualified port** of the complete XNA 4.0
sample. The original ships two runnable products, a Windows Phone XNA client and a WCF
server, and its advertised Wi-Fi multiplayer mode is a substantial part of the source.
The current port drops that complete client/server path. Restoring it requires an owner
decision recorded as `SAMPLES-DEC-009`; an offline-only game must not be relabelled as the
complete Yacht port without that explicit scope decision.

No old workaround documented below is accepted by this audit. The port was deliberately
left unchanged while the decision is pending.

## Exact original and artifact root

- Upstream: `/rv/tmp/XNAGameStudio/Samples/Yacht_4_0`
- Retained artifact root: `/rv/tmp/samples/SAMPLE-071-Yacht_4_0`
- Exact 109-file snapshot: `xna4-original/`
- Snapshot manifest: `evidence/original-sha256.txt` (109/109 entries verified)
- Original client solution: `xna4-original/Yacht Client.sln`
- Original server solution: `xna4-original/Yacht Server.sln`
- Reproducer: `scripts/build-original.sh`
- Full successful build log: `evidence/build-original.log`

The snapshot contains 39 C# files and 10,983 lines. The original documentation explicitly
requires building and running both Yacht Server and Yacht Client and describes multiplayer
over HTTP. The selected client is the Windows Phone 7 Reach project. The server consists of
the `YachtServices` WCF library and the `Server` console host.

## Original build evidence

The official XNA 4.0 `BuildContent` pipeline built all 45 declared Phone/Reach assets:

- 5 SpriteFont XNBs using the original Impact, Segoe UI Mono and Lindsey faces;
- 26 Texture2D XNBs, including the online selection assets;
- 14 SoundEffect XNBs.

They are retained under `xna4-build/Content-phone/`; all entries in
`evidence/xna-content-sha256.txt` verify. No loose PNG/WAV or replacement font is used as
original evidence.

The unchanged service and host sources compile to:

- `xna4-build/bin/YachtServices.dll`;
- `xna4-build/bin/Server.exe`.

The host was started and its real `http://localhost:8888/GameServer/?wsdl` response was
captured as `evidence/YachtService.wsdl` (9,980 bytes, SHA-256
`3041217a988083808e290941ed6c071f5efd061d8ec9652b260d9b54e430a7bf`).

The Wine prefix has XNA 4.0 but not the Windows Phone 7 SDK. To distinguish source errors
from unavailable phone assemblies, `scripts/WindowsPhoneDiagnosticShim.cs` supplies only
compile-time declarations for the missing WP sensor, lifecycle, push and Silverlight proxy
types. With no edit to the 39 original sources, every client unit type-checks into
`xna4-build/bin/Yacht.dll`. This diagnostic assembly is not runtime parity evidence and the
shim is not part of the C++ port.

## Measured complete-product blocker

The online feature is not a menu stub. Its missing implementation includes:

- the 674-line client `NetworkManager` and 475-line online-game screen;
- `NetworkPlayer`, the 1,117-line generated WCF client proxy and shared 747-line data model;
- the 807-line authoritative `YachtService`, server-side AI, subscriptions and console host;
- ten service operations: register/unregister, join/leave, game step/state, game discovery,
  new game, timeout reset and score-card retrieval;
- server validation, turn/state ownership, serialization, timers and Windows Phone raw/toast
  push notifications;
- `PhoneApplicationService` activation/deactivation state used by offline and online flows.

Live CNA and Sharp Runtime have no `System.ServiceModel` client/host/channel contract,
`HttpNotificationChannel`/MPNS implementation or `PhoneApplicationService` lifecycle layer.
This is a cross-platform service/protocol and retired-platform design, not a bounded missing
method. Adding only sample-local HTTP calls, a fake lobby, or an offline replacement server
would be a forbidden workaround.

`SAMPLES-DEC-009` asks the owner to choose one of these truthful boundaries:

1. authorize a dedicated reusable WCF/SOAP, lifecycle and push-compatible subsystem and port
   both original products;
2. authorize a documented cross-platform protocol modernization that preserves the complete
   client/server game behavior but intentionally changes the retired transport;
3. accept an evidence-backed offline-only/non-port boundary for this directory.

Until that choice is made there is no native OPENGLES3 or browser WEBGL2 parity claim; the
corresponding retained artifact directories are intentionally empty.

## Existing port audit

The checked-in port has 19 C++ source/header files and 3,010 lines, versus 39 original C#
files and 10,983 lines. It omits the complete online client, server, shared data model,
network player, tombstoning/save-load path and original name-entry flow. It also contains
these unqualified substitutions:

- loose PNG/WAV assets and generated bitmap-font sidecars instead of the 45 exact XNBs;
- replacement DejaVu fonts despite every original font being available and buildable;
- per-frame timer accumulators in place of the original `System.Threading.Timer` behavior;
- fixed `"Player1"` instead of `Guide.BeginShowKeyboardInput`;
- added mouse paths and an invented message-box screen;
- changed fullscreen/screen navigation and omitted persisted state.

Several historical explanations in the former `missing.md` are now false: CNA implements
`Guide.BeginShowKeyboardInput`, accelerometer and vibration support; Sharp Runtime implements
`System.Threading.Timer` and `System.IO.IsolatedStorage`. A future repair must use those real
APIs and the exact XNBs, remove the sample-local substitutions, and audit every original
source line. It must not preserve those workarounds merely because they predate this campaign.

## Qualification summary

- Exact snapshot manifest: **PASS, 109/109**
- Official XNA Phone/Reach content: **PASS, 45/45 XNBs**
- Unchanged WCF service/library compile: **PASS**
- Unchanged console server compile and live WSDL: **PASS**
- All unchanged client source units, diagnostic WP SDK shim only: **PASS**
- Faithful C++ translation: **BLOCKED by `SAMPLES-DEC-009`**
- Native OPENGLES3 parity: **not run; no faithful complete product exists**
- Real-browser WEBGL2 parity: **not run; no faithful complete product exists**
