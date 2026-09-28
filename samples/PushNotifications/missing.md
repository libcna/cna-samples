# SAMPLE-105 — `PushNotificationsSample_4_0` audit

## Current-head analysis — 2026-09-28

**Status: 🛑, owner scope decision required; analysis only.** There is no C++ sample target or
browser product. The historical claim that CNA has no notification channel is now obsolete:
the current framework contains a working native **loopback HTTP channel**, but it does not
implement this sample's complete Phone/service/shell contract. No implementation was changed.

### Physical product and controls

All **30 files / 187,521 bytes**, including both solutions, three projects, manifests, licence,
documentation and all **12 C# units**, match the complete retained upstream snapshot. This is a
pair of applications: a Phone 7 XNA **Reach** receiver and a separate .NET 4/x86 **WinForms sender**.
The receiver is portrait 480×800, fullscreen, 30 Hz, with a scrolling white-text console on
CornflowerBlue. Its only polled input is Gamepad Back to exit; it has no touch/accelerometer
interaction needing desktop emulation. There is no audio, model, custom effect or custom processor.
Its only compiled asset is `font` (Segoe UI Mono, 14 pt). Tile images are original package assets.

The receiver opens/reuses `ExampleXNAPushChannel`, displays its service URI, decodes foreground raw
messages with **BinaryReader.ReadString** and foreground toast dictionaries, binds shell toast/tile
and closes/reopens on channel error. Tile changes are shell behavior, never a drawn game panel.
The sender accepts the URI, toast strings, tile title/image/count and raw text through its original
form. Raw uses **BinaryWriter.Write(string)**, not the XML described by the HTML overview; source
takes precedence. Toast/tile are the original XML and realtime class 2/1; raw class is 3, with a
1,024-byte ceiling. Preserve the source's tile-count increment before sending and its repeated
DeviceConnectionStatus in the Subscription Status display; neither was silently repaired.

### Fresh original and framework evidence

Artifact root: `/rv/tmp/samples/SAMPLE-105-PushNotificationsSample_4_0/`.
Current receipts: `evidence/current-head-analysis-20260928/`.

- Unchanged sender project builds **Debug/x86** with real .NET 4 MSBuild in the established
  `/home/robertvokac/.wine-cna-xna40` prefix. Product:
  `xna4-build-analysis-20260928/sender/bin/PushNotificationSender.exe`, 21,504 bytes, SHA-256
  `5b77cfc9f840a66941e3b0a7298b68a6fe987a72c8f62b43133693b7af59e1c4`.
  MSBuild reports missing reference-assembly targeting packs and resolves GAC assemblies; this
  successful local build is not claimed warning-free or a fresh Phone SDK installation.
- The same unchanged sender runs under **Mono** on an owned Xvfb. The complete form, raw/tile
  empty-URI messages, tile count 1→2 and normal **WM-close exit 0** are observed. Mono autoscaling
  yields 488×689, so this is not a pixel-identical Windows reference. The toast capture proves
  button focus only, not delivery. No notification was sent to a fabricated or external URI.
- Current Wine sender startup reaches an invisible GDI+ window but not the form within the probe
  timeout. Logs/window inventory and the earlier capture-helper attempts remain separate evidence.
  This does not invalidate the earlier genuine Win7 sender run. No VM was needed or changed.
- Current unchanged Phone/client and ordinary content-project MSBuild attempts stop at missing
  installed XNA target imports. A separate established **official Microsoft BuildContent** runner
  successfully builds the exact content declaration as **WindowsPhone/Reach**, XNB v5 platform
  `m`: `xna4-build-analysis-20260928/bin/Content/font.xnb`, 21,678 bytes, SHA-256
  `4b01b7c7c08ccfb71a29a234e65be3887a3c821dd6ea1c99e40ba41b7beb5444`.
  It differs from the retained Win7 pipeline output; both are preserved, with no byte-parity claim.
  No unchanged Phone application or live MPNS delivery was run in this analysis.
- Current canonical CNA `next 92d23c84d` and owner Sharp branch
  `feature/gamer-services-collections 007280bd` build the existing focused **CnaPhoneTests** in a
  separate Release **OPENGLES3** diagnostic tree. The native notification/sender tests pass
  **9/9** through CNA's private GPU runner. They exercise the existing loopback backend, not the
  original two-product system, background delivery or a browser. Compiler warnings remain in the
  unmodified dependency/test build; no warning-free production claim is made.

Reproduction: `scripts/build-original-analysis-20260928.py`,
`build-content-analysis-20260928.sh`, `build-phone-analysis-20260928.sh`, and
`CNA_SENDER_RUNTIME=mono python3 scripts/capture-sender-analysis-20260928.py`.
CNA builds use canonical dependencies, shared physical ccache, `CCACHE_BASEDIR=/rv`, both
launchers and all 16 cores. Source/product/framework hashes are in `analysis-provenance.json`.

### Exact current gaps and owning layers

| Original requirement | Current behavior / required work |
|---|---|
| Service-issued channel and remote delivery | `modules/phone/src/HttpNotificationChannel.cpp` binds **INADDR_LOOPBACK**, returns `http://127.0.0.1:<port>/<name>/` and starts a local listener thread. It neither registers with MPNS nor reaches another machine, a sleeping device or an exited process. A reusable notification-service/platform design is needed in CNA; a synthetic sample URI is not a solution. |
| Raw stream contract | `HttpNotificationEventArgs.hpp` exposes a byte vector and text helper, not the original `Notification.Body` Stream shape used by BinaryReader. Restore the public contract generally rather than hand-decode a replacement in the sample. |
| Toast and error events | `ShellToastNotificationReceived`, `NotificationEventArgs`, `ErrorOccurred` and `NotificationChannelErrorEventArgs` are absent. The existing toast sender's XML reaches **HttpNotificationReceived as raw bytes**; tests explicitly prove this different behavior. It cannot qualify the original toast dictionary or error/reopen branch. |
| Tile and background shell behavior | `BindToShellTile` is absent; `BindToShellToast` only records a boolean. No shell popup, pinned title/count/image update or process-independent notification delivery exists. These need general platform backends and a defined cross-platform fidelity boundary. |
| Normal event lifecycle | Open raises URI synchronously; received bytes require an explicit `DispatchPendingNotificationsEXT`. The source has no such call and CNA Game does not automatically dispatch channels. A faithful shared event/lifecycle route is required; no sample polling workaround was added. |
| Sender UI and HTTP surface | Sharp Runtime has no WinForms/System.Drawing or the used `WebRequest`/`HttpWebRequest`/`HttpWebResponse` surface. Native HttpClient exists but is a different API; its Emscripten handler explicitly throws PlatformNotSupportedException. CNA's alternate notification helpers do not provide this form or complete historic response/status contract. |
| Browser delivery | The channel is a native incoming socket listener. The installed Emscripten SOCKFS `listen` rejects non-Node environments; pthreads do not supply a browser listener or background push. General service, browser delivery/permission/lifecycle and notification UI work remain. |

The normal networking/Gamer Services server track does not by itself supply Phone notification
channels, shell tiles, process-independent delivery or a WinForms sender. No new server subsystem,
local echo demonstration, sender-only port, invented game screen or browser bundle was added.

### Decision boundary

The archived [Microsoft Phone API](https://learn.microsoft.com/en-us/previous-versions/windows/apps/ff402781%28v%3Dvs.105%29)
documents the required events and tile binding. Microsoft's archived
[termination notice](https://learn.microsoft.com/en-us/answers/questions/2764791/mobile-push-notification-services-are-ending-for-w)
announces notification/live-tile retirement for WP7.5/8.0 on 2018-02-20; it is not evidence for
every later Windows notification service. A new CNA service would be a deliberate replacement
boundary, not today's original Microsoft channel.

[Emscripten networking documentation](https://emscripten.org/docs/porting/networking.html) describes
browser socket/proxy constraints. Modern [Web Push](https://www.w3.org/TR/push-api/) has its own
permission, subscription, service-worker and protocol model; it is not an unmodified MPNS backend.

Under `SAMPLES-DEC-004` and `SAMPLES-DEC-005`, the owner can cancel this retired-platform pair,
authorize a reusable notification/service/shell system with a faithful two-product acceptance
contract, or explicitly modernize both receiver and sender UI and specify supported tile/browser
differences. Merely extending method declarations or showing the console would not complete 105.
The present analysis neither cancels it nor authorizes any of these implementation scopes.

## Historical audit — 2026-09-01

The following build/VM records are retained. Its blanket API-absence conclusion is superseded by
the current-head analysis above.

### Status

Fresh audit complete enough to require an owner scope decision under `SAMPLES-DEC-004`. No port,
fake channel URI, local notification loop, sender-only target or modern Web Push substitute was
added. This row remains `🛑`; only the owner may select a non-port or modernization boundary.

### Classification and complete source inventory

The physical upstream directory contains two separately built and deployed products:

- `PushNotificationClient.sln`: a Windows Phone 7/Reach XNA receiver application;
- `PushNotificationSender.sln`: a Windows Forms desktop utility that posts test notifications to
  the device-specific URI supplied by the phone client.

The audit covers all 30 upstream files, both solutions and all three projects, manifests, icons,
tile images, the SpriteFont declaration, documentation, generated WinForms/resource sources and
all 1,240 checked-in C# lines. The exact tree is retained unchanged under the artifact root.

This is not a notification-themed rendering sample. Its XNA output is a portrait 480x800,
fullscreen, 30 Hz CornflowerBlue screen with a thread-safe scrolling `SpriteFont` console. The
behavior being demonstrated is channel creation and service/shell delivery:

- `HttpNotificationChannel.Find` reuses a registered `ExampleXNAPushChannel`; otherwise the
  client creates it for `ExampleXNAPushService`, subscribes to `ChannelUriUpdated` and opens it;
- the URI allocated by Microsoft's service is displayed and copied to the sender;
- foreground raw messages arrive as bytes and are decoded through the original `BinaryReader`
  string contract;
- foreground toast messages arrive as a string dictionary, while background toast is rendered by
  the Windows Phone shell;
- tile messages are never handled in-game: the phone shell changes the pinned shortcut's title,
  count and background image;
- errors close and reopen the channel, and a URI update binds the channel to both shell toast and
  shell tile delivery.

The companion WinForms utility builds the exact toast/tile XML or BinaryWriter raw payload, enforces
the 1,024-byte limit, POSTs to the phone-provided channel URI and uses the historic
`X-WindowsPhone-Target`, `X-NotificationClass`, `X-MessageID`, `X-NotificationStatus`,
`X-DeviceConnectionStatus` and `X-SubscriptionStatus` headers.

### Original build and run evidence

Artifact root: `/rv/tmp/samples/SAMPLE-105-PushNotificationsSample_4_0/`.

- `xna4-original/` is the complete byte-for-byte upstream snapshot.
- The owner-provided Windows 7 SP1 32-bit VM was run headless with all eight virtual network
  adapters set to `none`; no browser or guest internet access was enabled.
- The unchanged `PushNotificationSender.sln` builds successfully as Debug/x86 with the guest's
  .NET 4 MSBuild. Its exported executable has SHA-256
  `b6a3b4f87d59fa2a0a20fe4c45ba96e31e728abfb42472e6a1cdf62164277503`.
- That exact sender executable runs in the guest and displays the complete 418x560 form containing
  channel URI, toast, tile, raw and server-response groups. The headless capture is
  `evidence/original-sender.png` (SHA-256
  `007473bf7c804a5fe654cb04321bed100c80c563104a3d336948078854246a21`).
- The unchanged WindowsPhone/Reach content project independently builds `font.xnb` through XNA
  Game Studio's official pipeline. The exported Phone XNB has SHA-256
  `4d7e8609a4a96bcfdeb72453c7afe2cb68e63f31be32c7331121683c2dfcea45`.
- The unchanged client solution stops before source compilation at
  `Microsoft.Xna.GameStudio.targets(34,5)`: this VM's XNA installation does not include Windows
  Phone project support. That local SDK absence is recorded separately from the service boundary;
  it does not affect the successful Phone content build or sender build.

Exact commands and results are retained in `scripts/build-original.cmd` and
`evidence/build-results.txt`; exported binaries are under `win7-export/`.

No live delivery was attempted. The guest intentionally remained offline, and a valid channel URI
can only be issued to a supported phone application by Microsoft's service. An arbitrary or local
URI cannot exercise the product: it would bypass both MPNS delivery and Windows Phone shell
behavior.

### Live CNA and Sharp Runtime capability audit

Searches of live `cnanext` and `sharp-runtimenext` found no `Microsoft.Phone.Notification`,
`HttpNotificationChannel`, notification-channel event args or phone shell implementation. This is
not a missing XNA API: the types belong to the retired Windows Phone platform and require an
external service plus OS shell integration.

Porting only the CornflowerBlue console would omit the sample. Porting only the sender form would
leave a tool whose sole operation targets a channel URI CNA cannot create. A local echo server or
synthetic URI would fabricate the channel lifecycle, device/subscription response headers,
foreground/background split, toast UI and pinned-tile behavior.

Modern browser push is not a compatible backend hidden behind the old names. It requires an HTTPS
origin, user permission, a service worker, Push API subscription/VAPID semantics and browser-owned
notification UI; it does not implement MPNS channel URIs or the Windows Phone tile protocol.
Likewise, native desktop notification systems do not provide one cross-platform equivalent of the
WP7 shell tile and channel-registration contract.

### Owner decision required

SAMPLE-015 previously established an owner-approved non-port for a larger WCF/MPNS game, but the
campaign rules require a separate owner decision for each physical upstream directory. Choose one:

1. accept an evidence-backed non-port for this focused retired-service sample;
2. authorize a faithful, reusable cross-platform notification service plus native/browser shell
   integration that preserves channel, foreground/background, toast and tile semantics; or
3. explicitly authorize a modernized product using different native and Web Push contracts and
   define how its two original products and mandatory browser gate should be represented.

Until that choice, native OPENGLES3 and WEBGL2 artifacts are intentionally absent: inventing an
inert client or fake service merely to satisfy those gates would violate the zero-workaround rule.
