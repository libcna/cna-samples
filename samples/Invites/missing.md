# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked. No C++ port has been started.** The API names
needed by this sample exist in CNA, but their defining service does not. This is specifically a
LIVE `PlayerMatch`/friends/Guide invitation lesson, not a generic LAN tank game. CNA deliberately
implements real transport only for `SystemLink`; its `PlayerMatch`, friend list and invited-session
paths are synthetic surfaces with no identity, matchmaking, invitation delivery or remote packet
transport. Replacing an accepted invitation with a key press or silently changing the session to
SystemLink would remove the sample's subject, so neither workaround was added.

Retained audit root:
`/rv/tmp/samples/SAMPLE-096-InvitesSample_4_0/`.

Exact upstream snapshot:
`/rv/tmp/samples/SAMPLE-096-InvitesSample_4_0/xna4-original/`.

## Audited original

The package provides Windows and Xbox 360/Reach projects over the same 734 lines of C#:

- the 549-line `InvitesGame` owns GamerServices sign-in, PlayerMatch create/find/join, accepted
  invitation handling, session events, state packets, drawing and all input;
- the 152-line `Tank` implements per-gamer placement, hull/turret steering, velocity/friction,
  edge clamping and two-texture drawing;
- 33 assembly-information lines complete the game endpoint.

The constructor adds `GamerServicesComponent` and subscribes to the static
`NetworkSession.InviteAccepted` event. With no profile signed in, the menu calls
`Guide.ShowSignIn(4, false)`. A/keyboard A creates a 16-player/four-local-player
`NetworkSessionType.PlayerMatch`; B searches PlayerMatch and joins the first result.

The invitation event is the defining behavior. It is delivered after either of two system-Guide
workflows:

- a host selects a friend and sends **Invite to Game**, then the friend accepts the notification
  (the documentation's pull mode);
- a friend selects the host and chooses **Join Session In Progress** (push mode).

The same handler disposes any current session and calls `NetworkSession.JoinInvited(4)` without a
second prompt because the Guide already obtained user confirmation. The documentation also covers
cross-title invites: the console can launch the installed title or route to its marketplace page;
Windows asks the user to start the named title and retains the pending invite briefly.

After any create/join route, `GamerJoined` attaches one `Tank` to each gamer and `SessionEnded`
returns to the menu. Each local gamer sends position, hull rotation and turret rotation with
`SendDataOptions.InOrder`; every local endpoint consumes remote packets. The game draws all tanks,
gamertags, host identity and talking state. Arrow keys/left stick drive, WASD/right stick aim and
Escape/Back exits. These packet-gameplay mechanics matter, but they are the Peer-to-Peer base on
which the invitation lesson is built—not a substitute for it.

Microsoft's documentation requires two Windows/Xbox machines, two XNA Creators Club
subscriptions and two befriended LIVE profiles. Invites are explicitly unavailable for System
Link. Those requirements define the tested product boundary rather than incidental setup advice.

## Authentic XNA build and runtime evidence

The retained `scripts/build-original.sh` compiles all three unchanged source units and runs the
three exact content declarations through the official offline XNA 4.0 Windows/Reach pipeline. The
build passes and produces:

| File | SHA-256 |
|---|---|
| `Invites.exe` | `6f24d9f22883da4a4ea0ade894a080a2e812f9b2170621b501b150410f6899a8` |
| `Font.xnb` | `62e04ac27f0a10f46424bdae3c53d9371e164e20480aa77f0ee3e88796ac2d59` |
| `Tank.xnb` | `3f171b4448ed8e33767173137073593333b673e65628598b564e0a6ccc35b2c0` |
| `Turret.xnb` | `5c7cae093a829276b215c0d16cb26cb80e829e030c518510efb78bb912db37d9` |

The retained `scripts/capture-original.sh` runs that exact executable with the locally installed
XNA Live Proxy on isolated Xvfb and with no Internet access. The real Games for Windows LIVE Guide
first displays its local `Player1` sign-in surface. Selecting it reaches the exact A=create /
B=join menu. Pressing A then produces the authentic error that an eligible signed-in gamer and
possibly a LIVE profile is required: the local offline profile cannot enter PlayerMatch. All three
captures and the runtime transcript are retained under `evidence/original-windows-reach/`.

This proves the unchanged executable, Guide boundary, menu and service precondition. It cannot and
does not claim a successful 2026 invitation, because the required Creators Club/LIVE service and
friend accounts are retired. The original source and Microsoft documentation remain the
authoritative evidence for the remote invite workflow.

## Live CNA audit

The audit used clean CNA commit `e5ae0820e234`. CNA exposes the expected XNA-shaped classes,
overloads and events, including `NetworkSession::InviteAccepted`, `JoinInvited`, PlayerMatch
create/find, gamer/session events, packet readers/writers and Guide sign-in. Therefore this is not
a reason to add duplicate signatures or a sample-local compatibility shim.

The owning implementations and documentation establish an intentionally narrower contract:

- `ENetBackend::RealNetworkingEnabled` is true only for `SystemLink`; PlayerMatch, Ranked and the
  other synthetic types never bind a port, connect or send remote packets;
- discovery returns empty immediately for non-SystemLink sessions;
- `SignedInGamer::GetFriends()` has no population source and returns an empty collection;
- Guide sign-in/friends behavior is a stub rather than an operating-system or online-service UI;
- the static invitation subscription is valid, but CNA's own C-API documentation states that
  nothing in the canonical implementation raises `InviteAccepted`;
- `JoinInvited` can construct the local XNA-shaped session object but has no invitation token,
  selected friend, remote host address or transport to join.

The Debug `CnaNetTests` target built with at most eight jobs. With local socket access enabled, the
complete current suite passed **295/295 tests from 33 suites**. This includes real SystemLink ENet
and two-process coverage as a control, plus explicit policy tests that non-SystemLink types are
synthetic, PlayerMatch join does not activate transport and invited-session API validation/local
construction works. The retained log is `evidence/cna-net-tests.log`. A first sandboxed run failed
only where the test process was forbidden to bind local sockets; it is not product evidence and is
not the retained qualification result.

The browser boundary is stricter still: a real WEBGL2 product cannot host or discover raw ENet/UDP
sessions. Even a SystemLink rewrite would need the broker/relay work already recorded in
`SAMPLES-DEC-006`; a faithful invite implementation additionally needs identity, friends,
matchmaking, invitation delivery and address handoff. A one-tab local session demonstrates neither
the original native nor browser behavior.

## Current result and resume conditions

No C++ source, CMake target, fake LIVE profile, locally raised invite event, SystemLink remap, CNA
change or Sharp Runtime change was added. SAMPLE-096 remains `🛑` under `SAMPLES-DEC-004` and
`SAMPLES-DEC-006` until the owner chooses one of these scopes:

1. accept this evidence-backed retired-LIVE/invite non-port boundary;
2. authorize a reusable replacement identity, friends/presence, matchmaking, invitation and
   relay/address-handoff service for native and browser CNA, with explicit cross-title/deep-link
   policy;
3. explicitly authorize a documented SystemLink-only modernization and its browser broker/relay,
   accepting that the result is no longer the invitation sample described by Microsoft.

If option 2 is authorized, implement and qualify the service behind the shared XNA-shaped network
and GamerServices APIs before translating the 734 lines. Completion then requires at least two
independent identities and clients: create/find/join, pull and push invitation acceptance,
leave-current-session semantics, successful invited remote join, synchronized tank movement,
host/talking labels, session end and real-browser multi-frame/error gates. Do not call a manually
raised test event or synthetic local session a successful invite.
