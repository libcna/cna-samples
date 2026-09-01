# SAMPLE-105 — `PushNotificationsSample_4_0` audit

## Status

Fresh audit complete enough to require an owner scope decision under `SAMPLES-DEC-004`. No port,
fake channel URI, local notification loop, sender-only target or modern Web Push substitute was
added. This row remains `🛑`; only the owner may select a non-port or modernization boundary.

## Classification and complete source inventory

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

## Original build and run evidence

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

## Live CNA and Sharp Runtime capability audit

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

## Owner decision required

SAMPLE-015 previously established an owner-approved non-port for a larger WCF/MPNS game, but the
campaign rules require a separate owner decision for each physical upstream directory. Choose one:

1. accept an evidence-backed non-port for this focused retired-service sample;
2. authorize a faithful, reusable cross-platform notification service plus native/browser shell
   integration that preserves channel, foreground/background, toast and tile semantics; or
3. explicitly authorize a modernized product using different native and Web Push contracts and
   define how its two original products and mandatory browser gate should be represented.

Until that choice, native OPENGLES3 and WEBGL2 artifacts are intentionally absent: inventing an
inert client or fake service merely to satisfy those gates would violate the zero-workaround rule.
