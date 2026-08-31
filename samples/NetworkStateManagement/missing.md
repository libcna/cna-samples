# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked. No C++ port has been started.** The old
classification was wrong: the deliberately small `GameplayScreen` is not the product being taught.
This sample's product is the complete multiplayer user-interface and session-lifecycle layer. A
faithful port cannot pass the campaign's browser gate until CNA has a browser-reachable
create/find/join route, and its Xbox LIVE `PlayerMatch`/invite behavior also needs an explicit
retired-service decision.

Source: `/rv/tmp/XNAGameStudio/Samples/NGSMSample_4_0/`.

Retained audit root: `/rv/tmp/samples/SAMPLE-075-NGSMSample_4_0/`.

## Audited original

The physical package contains Windows and Xbox 360 solutions around the same game. The selected
Windows XNA 4.0 Debug/x86/Reach project lists 26 C# compile units containing 5,177 lines. Twelve
units and 2,572 lines directly participate in profile, session, lobby, invite or synchronized
screen behavior. The sample implements:

- Single Player, LIVE (`PlayerMatch`) and System Link menu routes;
- profile validation, Guide sign-in/marketplace flow and online privilege checks;
- asynchronous `BeginCreate`/`EndCreate`, `BeginFind`/`EndFind`, `BeginJoin`/`EndJoin` and
  invited-session operations, with busy and error screens;
- a 16-gamer/four-local-gamer lobby with host identity, voice/ready icons, readiness propagation,
  forced start, leave/end-session behavior and join/leave notifications;
- a `NetworkSessionComponent` that owns, updates, exposes and disposes the session and translates
  disconnect/end reasons back into UI state;
- synchronized lobby-to-gameplay and gameplay-to-lobby transitions;
- a 30 Hz background worker that keeps the session updated and explicitly presents the loading
  animation while the main thread performs slow content loading;
- the complete inherited screen manager, menus, dialogs, message display, input and pause flow.

The original documentation says exactly that the sample demonstrates the UI for a multiplayer
networked game: create, find and join menus, lobby and robust error handling. Its later statement
that there is "no actual game code" refers only to the intentionally instructional placeholder
after the lobby. The placeholder still has the original single-player movement, random enemy
jitter, pause policy and network-state transition checks; removing the surrounding network product
would not be a port.

The content project contains ten exact assets: two SpriteFonts, the background, blank and gradient
textures, a cat texture and four lobby voice/ready icons. The retained `scripts/build-original.sh`
uses XNA 4.0's official content pipeline and compiles all 26 unchanged C# units. It produced all ten
Windows/Reach XNBs plus a PE32/x86 `NetworkStateManagement.exe`; the full log and SHA-256 list are
under `evidence/build-original.log`.

The original's `GamerServicesComponent` also requires Microsoft's `XnaLiveProxy.exe`. The retained
offline capture route installs that component from the local XNA Game Studio media, adds only its
local `Bin` directory to `WINEPATH`, and never enables network access. `scripts/capture-original.sh`
then runs the unchanged executable with WineD3D on isolated Xvfb and records:

1. main menu;
2. animated slow-loading screen;
3. the single-player placeholder;
4. pause menu;
5. quit-game confirmation;
6. return to the main menu;
7. exit-sample confirmation and a clean process exit.

Those screenshots and the clean-exit log are retained in
`evidence/original-windows-reach/`. They disprove the historical reason for ignoring this sample.

## Live CNA audit

The dependency audit used clean CNA commit `0a6158e4ff764907065cd7259e3d29e331a52088`.
CNA already exposes the XNA-shaped session, gamer, Guide, event, readiness, session-state and
asynchronous APIs used by the source. The previously assigned `NetworkSession.MaxGamers` issue is
already fixed. No missing C++ signature justifies a sample workaround.

The implementations deliberately establish a narrower runtime contract:

- `ENetBackend::RealNetworkingEnabled` returns true only for `SystemLink`. Native System Link has
  real ENet peers plus UDP discovery and can support a later two-process qualification.
- `Local`, `LocalWithLeaderboards`, `PlayerMatch` and `Ranked` are explicitly synthetic. CNA's
  policy regression tests prove that they bind no real port, discover nothing and transmit no
  packets. Creating `PlayerMatch` can therefore produce a one-process local lobby, but it is not
  Xbox LIVE matchmaking or remote multiplayer.
- `GamerServicesDispatcher::Initialize` creates four stub profiles marked signed in to LIVE,
  `GamerPrivileges` allows online sessions, and `Guide::ShowSignIn` is a no-op. This makes the
  sample's profile check advance, but does not reproduce an actual identity or Guide flow.
- `NetworkSession::InviteAccepted` exists as an event surface, but live CNA has no service that
  delivers an external LIVE invitation into it.

These are truthful platform/service boundaries, not requests to fake the sample in C++.

## Blocker 1: the required WEBGL2 multiplayer route does not exist

CNA's browser limitation is explicit. Under `__EMSCRIPTEN__`,
`ENetDiscoveryService::{RegisterHost,UnregisterHost,Poll}` do nothing and `FindSessions` always
returns empty because a browser cannot use raw UDP discovery. A browser tab also cannot listen as
an ENet host. CNA can create an outbound Emscripten client only after code already has a concrete
address and port.

Network State Management deliberately exposes only the public XNA create/find/join/invite routes.
It has no manual-address input and no private CNA seam. Consequently a WEBGL2 build could draw and
exercise Single Player, but could not advertise a System Link session, discover one, join it, or
qualify two peers through lobby readiness and synchronized transitions. A menu-only browser smoke,
fake local lobby, hard-coded address or sample-specific WebSocket path would violate the campaign
rules.

Faithful browser completion needs the same project-level capability recorded by
`SAMPLES-DEC-006` for NetRumble: a browser-reachable session directory/broker and
ENet-compatible WebSocket relay/address handoff, integrated behind the XNA session contract and
qualified with at least two peers. The alternative is an owner-approved browser scope exception or
an evidence-backed native-only/non-port boundary.

## Blocker 2: Xbox LIVE PlayerMatch and invitations are retired

The LIVE half of the original is not another spelling of LAN System Link. It checks for a LIVE
profile and privileges, creates/finds `NetworkSessionType::PlayerMatch` sessions, receives Guide
invitations, and joins the invited session. Microsoft's original service is no longer an available
reference endpoint, while CNA intentionally provides neither an Xbox LIVE-compatible identity and
matchmaking service nor a replacement service contract.

Silently mapping `PlayerMatch` to System Link would misrepresent the original network scope and
Guide/invite semantics. Keeping CNA's current synthetic local lobby would preserve API shape but
not the sample's demonstrated behavior. Under `SAMPLES-DEC-004`, the owner must choose a reusable
replacement service/identity/invite design, explicitly approve a documented System-Link-only
scope, or accept an evidence-backed non-port boundary.

## Current result and resume conditions

No C++ source, CMake target, converted content, fake profile/session or other workaround was added.
No CNA or sharp-runtime change was needed for this audit. SAMPLE-075 remains `🛑` until the owner
decides both the retired LIVE boundary and the WEBGL2 multiplayer boundary.

If a full network route is authorized, resume by porting all 26 source units and all ten exact XNBs,
then qualify the same UI/session transitions with at least two native System Link processes and at
least two real-browser peers. If a narrower scope is approved, record it as an explicit owner
decision before changing the source or completion criteria.
