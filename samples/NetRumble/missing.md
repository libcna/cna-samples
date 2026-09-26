# Missing / Differences from XNA 4.0 original

**Current status: native OPENGLES3/System-Link port requalified on 2026-09-26 (`✅`).**
On 2026-09-05 the owner explicitly selected a
faithful native System-Link port and decided that no web port will be produced for SAMPLE-062. This
is a SAMPLE-062-only exception to the ordinary browser gate, not permission to reduce or fake the
game. The old HLSL, Song, XML and packet-reuse blockers are resolved; all 49 original source units,
exact content, lobby and packet gameplay are represented and qualified in the native port.

Source: `/rv/tmp/XNAGameStudio/Samples/NetRumble_4_0/NetRumble/`.

Retained audit root: `/rv/tmp/samples/SAMPLE-062-NetRumble_4_0/`.

## Current-head requalification — 2026-09-26

The build chain is CNA `next` `cefe6c83b` and SharpRuntime `next`
`d86adb65`, with the `DirectoryInfo` fix described below. The retained
120-file `xna4-original/` snapshot matches the physical upstream directory
exactly. The unchanged 49-source Windows/x86/HiDef project rebuilt with
`scripts/build-original.sh`: Wine's XNA pipeline compiled every non-WMA item,
and the helper verified and inserted the exact offline Win7 `SongProcessor`
XNB/WMA pair. The original product's other 63 content files are byte-identical
to the native source content. `build-original.log` records `BuildContent
(Windows/HiDef) result: True` for that non-WMA pipeline run.

The Release OPENGLES3 product was rebuilt against these heads with ccache and
its 64 runtime content files match `samples/NetRumble/Content/` byte for byte.
The two-process `scripts/qualify-current-native.sh` run on separate X displays
completed real host create, client discovery and join, two-player lobby, both
ready transitions, synchronized two-ship gameplay and client thrust visible
on the host. The menu Exit/confirmation path terminated cleanly. A separate
host-only SDL disk-audio pass recorded 39.033 seconds of 44.1 kHz stereo PCM,
with continuous gameplay output after the silent menu interval; a five-second
lossless excerpt is retained as `host-gameplay-audio.flac`. Captures and logs
are in `evidence/current-20260926/`; the earlier 2026-09-05 qualification
remains historical evidence. The C++ translation still represents all 49
original source units, including networking, particle XML, Bloom and audio.

The owner-approved changes are recorded in `diff.md`: the duplicate local XML
adapter was removed in favor of CNA's shared `Vector2`/`Vector4` mapping;
the original `audio/wav` and `Textures/Clouds` identifiers were restored; and
the lossy Vorbis companion was replaced by Ogg FLAC. SharpRuntime's general
`DirectoryInfo` lookup resolves unambiguous ASCII case differences only after
an exact path lookup fails, so ordinary exact-path operations retain their
fast path. Its 11 focused `DirectoryInfo` tests pass. The full component gate
passes 18,089 tests across 41 executables when seven unrelated known-baseline
cases are filtered: two external `SoapChannelLiveTest` skips and five existing
`Xml.Linq` whitespace expectation failures. No CNA code change was needed.

The unchanged original EXE was also attempted under WineD3D and DXVK. After
making `XnaLiveProxy.exe` available beside a temporary product copy, neither
route created a game window; X11 showed only Wine's 1×1 GDI+/IME helpers.
An isolated diagnostic copy traced the stall to the base XNA `Game`
constructor, before NetRumble's constructor body. The retained original source
and EXE were not modified for this diagnostic. The offline Win7 VirtualBox VM
could not be started on this host because its virtualization driver/device is
unavailable. Thus this requalification makes no fresh XNA screenshot or direct
Wine visual/audio parity claim. The complete native port is qualified against
the audited original source and authentic pipeline content; the original
execution limit is an environment limitation, not a sample workaround.

The SAMPLE-062-only no-web decision still matches CNA `cefe6c83b`:
`ENetBackend::RealNetworkingEnabled` accepts only `SystemLink`, while
`ENetDiscoveryService` under `__EMSCRIPTEN__` still has no-op host registration
and returns no discovered sessions. NetRumble's public XNA menus have no
direct-address path to a browser relay. Do not replace this with a menu-only
WEBGL2 smoke or a sample-local network bypass.

No WEBGL2 bundle or gallery card is expected for this owner-approved native-only
sample. The gallery promises browser-playable samples, so a native-only card
would misrepresent this result.

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

CNA loads the four official compiled Effect XNBs on OPENGLES3. No GLSL rewrite, `.shader.json`,
hand-bound effect or disabled Bloom branch is present. Runtime qualification exercised the Clouds
effect in the background and all three Bloom effects in gameplay.

## Resolved prerequisite: authentic `SongProcessor` output

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

After the owner supplied guest access, the completely offline Win7 XNA pipeline rebuilt the
unchanged Windows/HiDef content project and returned zero. `One Step Beyond.xnb` is 133 bytes
(SHA-256 `c95955413a49ade9b48d14fd257b4d66abd25faba33c32a36ef2e8823eacc14e`); its external
`One Step Beyond.wma` is 8,827,707 bytes (SHA-256
`94333300dea59aa89c54d305082533e6379e717bc8b2de5b6cd6167cb04ba30c`). The XNB is `XNBw`,
names `SongReader`/`Int32Reader` and the matching stream, and its 366,085 ms duration equals the
valid stereo WMA v2 stream exactly. Shared evidence:
`/rv/tmp/samples/SAMPLES-DEC-007-Win7-SongProcessor/export/`.

## Owner-approved SAMPLE-062-only web exception

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

Faithful browser completion would require a project-level decision and a substantial new capability:
design and operate a browser-reachable session directory/broker plus ENet-compatible WebSocket
relay/address handoff, expose it without corrupting XNA ownership/session semantics, and qualify at
least two peers through lobby and gameplay. On 2026-09-05 the owner chose the native-only boundary
for this sample. Therefore no WEBGL2 build, menu-only browser smoke, fake lobby, manual-IP UI or
browser artifacts will be produced. Native System Link remains mandatory and must be qualified
with two independent processes through create/find/join, lobby and gameplay. This ruling does not
apply to any other sample.

## Completed implementation and qualification

The port represents all 49 original C# compile units in 48 `.cpp` and 49 `.hpp` files. The original
nested entry point becomes `Program.cpp`, while `ParticleEffectType.cs` is declared with the other
sample enums in `NetRumbleTypes.hpp`; neither transformation removes behavior. The four effects,
28 textures, three fonts, 15 SoundEffects, six verbatim particle XML files and authentic Song
XNB/WMA output are retained. The additional Ogg FLAC `.oga` is a decode companion for the native audio backend,
not a replacement game-code path; `Content.Load<Song>("One Step Beyond")` remains unchanged.

The six particle graphs deserialize through Sharp Runtime's reusable `XmlSerializer(Stream&)` and
`DirectoryInfo.GetFiles` implementations. The former local `XmlSerializationAdapters.hpp` has
been removed in favor of CNA's shared XML mapping. The original mixed-case content identifiers
are preserved and resolved in the owning runtime layers. No manual sample XML parser exists.
CNA's reused `PacketWriter`
send path now transmits only bytes through the writer's current position (`cnanext 1704c3273`), and
all 18 `LocalNetworkGamerTest.*` tests pass, including the new long-then-short packet regressions.

A fresh OPENGLES3 build passes. Native runtime qualification loaded every menu/gameplay asset and
all six particle definitions. A two-process System-Link run completed host creation, discovery by
the second process, join, a two-player lobby, both ready transitions, and synchronized gameplay
rendering with both ships. After client thrust, both peer captures show the changed ship position.
This is real ENet discovery and packet traffic; no fake session, single-player substitute,
manual-address UI or reduced networking branch was added. The standalone FLAC probe decoded and
seeked the `.oga`; full decoding of it and the shipped WMA yields exactly the same 16-bit stereo
PCM SHA-256 `4d8d5be04a89c990f1bbe30a43ec082d493534245a540a7114c46268f40619aa`.

## Retained artifact inventory

After the owner explicitly authorized pruning on 2026-09-26, the artifact
root went from 237.4 MB to 82.2 MB. The prune removed 15 reproducible
intermediate paths, stripped the native executable and deduplicated identical
content blocks. Of 372 pre-prune hashed files, all 323 retained files still
match; the other 49 were only intended CMake intermediates. A repeat dry run
proposes zero paths. The stripped product passed another real two-process
System-Link run through discovery, join, ready states, gameplay and client
thrust without an error. The current retained outputs are:

- `xna4-original/`: exact 120-file upstream snapshot;
- `xna4-build/bin/`: unchanged-source XNA executable, framework DLLs and official content;
- `cna-native-opengles3/samples/NetRumble/`: current native executable and the exact
  64-file runtime content;
- `scripts/build-original.sh` and `scripts/qualify-current-native.sh`: reproducible
  original rebuild and two-peer native qualification;
- `evidence/current-20260926/`: current build log, native menu/lobby/gameplay/exit
  captures, gameplay-audio excerpt and Wine failure diagnostics;
- `evidence/post-prune-20260926/`: retained-file validation, post-prune
  host/client captures and logs.

SHA-256 verification confirms that the native and XNA products use the authentic
`One Step Beyond.xnb` (`c95955413a49ade9b48d14fd257b4d66abd25faba33c32a36ef2e8823eacc14e`)
and WMA (`94333300dea59aa89c54d305082533e6379e717bc8b2de5b6cd6167cb04ba30c`) outputs.
The new Ogg FLAC companion is 37,279,699 bytes, SHA-256
`83541d9407937b36d10ac3f4b544cbcf39c715848cc8075e0aa453b57f222e51`.
`MANIFEST.md` in the artifact root records the current prune and the corrected
restoration commands against the active `libcna` checkouts.
