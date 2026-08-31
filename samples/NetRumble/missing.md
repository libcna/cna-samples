# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked on 2026-08-31. No port has been
started.** The old claim that four HLSL effects required handwritten GLSL was false, but the
fresh audit found two independent blockers that cannot be hidden in the sample: authentic Song
content is unavailable from the current reference environment, and CNA's browser networking
cannot execute NetRumble's defining create/find/join flow.

Source: `/rv/tmp/XNAGameStudio/Samples/NetRumble_4_0/NetRumble/`.

Retained audit root: `/rv/tmp/samples/SAMPLE-062-NetRumble_4_0/`.

## Audited original

The physical package contains 120 files. `NetRumbleWindows.csproj` is the selected Windows XNA
4.0 Debug/x86/HiDef game and lists 49 C# compile units; the C# tree is 12,812 lines. The project
contains the complete screen manager, signed-in/live menus, asynchronous session creation and
search, lobby, gameplay, packet serialization, collision, weapons, particles, bloom, audio and
entry point. The Xbox 360 project is a platform variant, not a replacement for the selected
Windows reference.

Ten source files containing 5,488 lines directly participate in gamer/session identity or packet
behavior: `NetRumbleGame`, `ScreenManager`, `MainMenuScreen`, `SearchResultsScreen`,
`LobbyScreen`, `GameplayScreen`, `World`, `Ship`, `ShipInput` and `PlayerData`. The menus expose
System Link create/find for a locally signed-in gamer and Player Match create/find/quick-match for
a LIVE gamer. Gameplay then relies on real `NetworkSession.Update`, join/leave/host events,
`LocalNetworkGamer`, targeted and broadcast `PacketReader`/`PacketWriter` traffic and ordered or
reliable-in-order delivery. A local fake lobby or single-player branch would not be a port of this
sample.

The content project has 56 compiled assets and six verbatim XML particle definitions:

- four effects (`Clouds` plus Bloom extract, two-pass Gaussian blur and combine);
- three fonts, including the font-texture processor output;
- 28 textures;
- 15 WAV sound effects;
- six XML particle graphs marked `None` + `CopyToOutputDirectory`;
- the WMA song `One Step Beyond` through `WmaImporter`/`SongProcessor`.

`scripts/build-original.sh` in the retained audit root reconstructs this project through XNA's
official `BuildContent` task and the in-prefix .NET compiler. It reads the content project rather
than maintaining a second handwritten asset list.

## The historical shader blocker is disproved

All four unchanged `.fx` files compile successfully through XNA 4.0's official
`EffectImporter`/`EffectProcessor` for Windows/HiDef. The three NetRumble Bloom files differ from
the already qualified `SAMPLE-031` versions only by renaming `PixelShaderFunction` to
`PixelShaderF`; their shader code and techniques are otherwise identical. `Clouds.fx` is a
pixel-only `ps_2_0` effect using the SpriteBatch texture sampler, wrap addressing and one `float2`
parameter. Its unchanged official XNB is also produced successfully.

CNA already loads official compiled Effect XNBs on OPENGLES3 and WEBGL2. No GLSL rewrite,
`.shader.json`, hand-bound effect or disabled Bloom branch is permitted or needed. Because the
sample is stopped at the decision boundary below, these four XNBs have not yet been claimed as a
NetRumble runtime qualification.

## Blocker 1: authentic `SongProcessor` output

The exact official content build succeeds through every effect, font, texture and the first WAV,
then XNA's `WmaImporter` rejects the shipped 7,407,286-byte `One Step Beyond.wma` under Wine:

```text
Failed to open file One Step Beyond.wma. Ensure the file is a valid audio file and is not DRM protected.
BuildContent (Windows/HiDef) result: False
```

The input is a real Microsoft ASF/WMA file. This is the same unavailable Windows Media codec path
measured by SAMPLE-060, not malformed sample data. A separately labelled diagnostic run omits
only that importer invocation, builds all other 55 compiled assets, copies the six XML sources and
links all 49 unchanged game sources successfully. It is evidence about the rest of the project,
not accepted runtime content.

Running that diagnostic executable from its correct working directory proves why a loose WMA is
not a substitute: the unchanged `Content.Load<Song>("One Step Beyond")` requests
`Content/One Step Beyond.xnb` and throws `ContentLoadException` because the authentic Song XNB is
absent. Do not add a loose-file fallback, handwritten Song XNB or sample-local load path. Completion
needs the Windows/HiDef `One Step Beyond.xnb` and its processor-produced external stream from a
real Windows XNA 4.0 build.

The owner-requested Win7 retry on 2026-08-31 verified that VirtualBox, the guest and Guest
Additions now boot and that a narrow export share is configured. It then stopped at a newly
measured access blocker: the saved `vboxuser` automatic-login credential is invalid and empty-
password Guest Control is rejected. No credential was guessed, extracted or reset; the VM is
safely saved pending owner login. Shared evidence:
`/rv/tmp/samples/SAMPLES-DEC-007-Win7-SongProcessor/`.

## Blocker 2: the required browser multiplayer route does not exist

Live CNA at `b54c4d25a` has a real native System Link implementation. `NetworkSession`, gamer
events and packet transport are backed by ENet, and native `ENetDiscoveryService` advertises and
finds sessions with UDP broadcast/loopback. The old document's blanket claim that networking was
missing is therefore also stale.

The Web platform boundary is explicit and materially different:

- `ENetBackend::RealNetworkingEnabled` returns true only for `SystemLink`; Player Match and Ranked
  do not provide real matchmaking transport.
- Under `__EMSCRIPTEN__`, `ENetDiscoveryService::{RegisterHost,UnregisterHost,Poll}` are no-ops and
  `FindSessions` always returns an empty collection because browsers cannot use raw UDP discovery.
- CNA documents browser hosting as impossible. Its Emscripten ENet client can make an outbound
  connection only when code already has a concrete address/port and an appropriate WebSocket
  relay/proxy.
- NetRumble uses only public XNA `BeginCreate`, `BeginFind`, `BeginJoin` and invited-session routes.
  It has no direct-address configuration that could reach CNA's internal outbound seam.

Consequently a WEBGL2 build could render the title/menu but could neither advertise a session nor
discover and join one. Skipping those branches, inventing a single-player session, adding a manual
IP menu or declaring a menu-only browser smoke would violate the required full networking and
real-browser gates.

Faithful browser completion requires a project-level decision and a substantial new capability:
design and operate a browser-reachable session directory/broker plus ENet-compatible WebSocket
relay/address handoff, expose it without corrupting XNA ownership/session semantics, and qualify at
least two peers through lobby and gameplay. The alternative is an explicit owner-approved web
scope exception or evidence-backed non-port/native-only boundary. The campaign rules do not allow
this session to choose either option autonomously.

## Current result and resume conditions

No C++ source, CMake target, content substitute or network workaround was added to the placeholder.
SAMPLE-062 remains `🛑` until both of these are resolved:

1. obtain authentic `One Step Beyond.xnb` plus its external media output from Windows XNA 4.0;
2. choose and provide the browser multiplayer boundary described under `SAMPLES-DEC-006`.

After those prerequisites exist, port all 49 source units, retain the four official effect XNBs
and exact remaining content, qualify multi-process native System Link, then qualify real multi-peer
WEBGL2 lobby/gameplay rather than merely loading the menu.
