# SAMPLE-139 — `PushRecipe_WP7_SL` audit and owner decision

## Status

Fresh audit complete enough to require a joint retired-service and non-XNA product-boundary
decision under `SAMPLES-DEC-004` and `SAMPLES-DEC-005`. This is a complete Windows Phone
Silverlight client, WPF server, WCF service pair and reusable MPNS message library. It is not an
XNA `Game`. No fake channel URI, local notification echo, server-only target, reduced progress
animation or silent Web Push rewrite was added.

## Complete product inventory

The upstream delivery contains **128 files / 3,396,196 bytes** and one four-project solution:

- the Phone client has 19 C# files / 2,574 lines and eight XAML files / 462 lines;
- the shared `WindowsPhone.Recipes.Push.Messasges` library has 14 C# files / 1,408 lines;
- the WPF server has 32 C# files / 2,497 lines and nine XAML files / 1,075 lines;
- `Libraries.csproj` is an empty six-file vendor-DLL container, not a fourth runnable product;
- the application ships Phone artwork, three tile backgrounds, ten server-side digit images,
  WPF skins/templates, a class diagram, WP7 Toolkit and WPF Interactivity binaries;
- `WindowsPhonePushRecipe_V2.docx` is a 22-authored-page, 4,945-word recipe by Yochay
  Kiriaty. Its headless PDF render has 24 pages, all of which were rendered and inspected.

None of the product `.cs`, `.xaml`, project, solution or configuration files references
`Microsoft.Xna`. A single XNA name in the delivery occurs only in the vendored WP7 Toolkit XML
API documentation. Inventing a `Game` wrapper would change the product rather than port it.

## Audited Phone client

`PushContext` owns the singleton `HttpNotificationChannel` lifecycle. It finds or creates the
named channel, opens it, dispatches URI/error/raw events onto the Phone UI thread, persists the
push/tile/toast/raw switches in `IsolatedStorageSettings`, and binds or unbinds shell tile and
toast delivery. The application sends the service-issued channel URI and username to the WCF
server and exposes login, registration, server status, inbox and settings views.

Foreground raw messages update the application. The special `AskToPin` payload asks the user to
pin the application; toast and tile delivery can instead be owned by the Windows Phone shell. A
`ShellTileSchedule` performs hourly remote-image updates against the server's image endpoint.
These foreground/background, shell ownership, channel registration and schedule semantics are
the demonstrated behavior, not incidental UI.

The login view embeds SAMPLE-124's progress control. After removing comments and normalizing
only the namespace and two renamed helper methods, all **212/212 logical C# lines** are
sequence-identical. The `PerformanceProgressBar` and `CustomIndeterminateProgressBar` style trees
are also semantically identical after normalizing their keys. This is shared support code, but it
does not make the complete push recipe a duplicate of the isolated control sample.

## Audited message library and WPF server

The thread-safe message library implements real MPNS HTTP POST requests rather than a simulation.
It caches payloads, enforces the 1,024-byte message limit, supplies `X-MessageID`,
`X-NotificationClass` and `X-WindowsPhone-Target`, and parses notification, device-connection and
subscription response headers. Separate Raw, Tile and Toast types preserve their payload and
priority rules and expose synchronous and ThreadPool-backed asynchronous sends.

The MEF-composed WPF application opens two real `ServiceHost` instances at localhost port 8000:
the SOAP PushService registers subscribers and serves server-info/tile-update operations, while
the REST ImageService returns tile image streams. Its five distinct pattern view models are:

1. **Ask to Pin** — probes a tile update, then sends the `AskToPin` raw message when shell pinning
   cannot be confirmed;
2. **Tile Schedule** — passively serves the digit image requested by a client's hourly schedule;
3. **Custom Tile** — renders a 173×173 WPF visual into a PNG through `RenderTargetBitmap`;
4. **Counter** — uses a raw liveness probe, increments the shell counter only when the app is not
   foreground-running, and resets it on registration;
5. **One Time** — sends any selected combination of Tile, Toast and Raw messages.

The UI logs each response's message, notification, connection and subscription status. The
official recipe explicitly calls the server a demonstration: it intentionally omits production
authentication, persistence/database and deployment hardening. Those omissions are original
scope, not permission to omit the delivered client, service or shell behavior.

## Authentic build and runtime evidence

Artifact root: `/rv/tmp/samples/SAMPLE-139-PushRecipe_WP7_SL/`.

- `original/` is the complete byte-for-byte upstream snapshot; inventory, SHA-256 and empty
  snapshot-diff evidence cover all 128 files.
- The unchanged Release/Mixed Platforms solution was rebuilt using .NET 4 MSBuild under Wine
  without external requests, with `/m:1` and CPU affinity 0–7. The message library builds to a
  20,992-byte PE DLL and
  the WPF application builds to a 146,944-byte PE executable.
- The Phone client then stops before compilation at its exact import of
  `Microsoft.Silverlight.WindowsPhone.Overrides.targets`; the local installation lacks the
  required Windows Phone Silverlight v4 SDK. The unrelated empty `Libraries.csproj` separately
  reports `CS2008: No inputs specified` after its three real consumers have been processed.
- The unchanged WPF server runs under a 2560×1440 Xvfb display, never on the owner's real desktop.
  Its window appears only after MEF composition and both synchronous `ServiceHost.Open()` calls.
  All five tabs were activated and captured at the authored 650×700 size while the process was
  alive. No Send action or external MPNS/network request was made.
- The DOCX metadata, full extracted text, PDF metadata and inspected render retain the architecture,
  wire contract, screenshots and complete five-pattern explanation.

This is useful execution evidence for the unchanged desktop half, but it is not a live MPNS or
Phone-shell result. The Phone SDK/emulator and service needed to issue a valid channel URI are
absent, and the retired external delivery path was deliberately not contacted.

## Relationship to SAMPLE-105

SAMPLE-105 is the smaller focused XNA Phone receiver plus WinForms test sender. A complete hash
comparison finds only two exact shared files: generic `AppManifest.xml` and generated
`Resources.resx` scaffolding. SAMPLE-139 has different product code/assets and adds a reusable
message library, persistent Silverlight application, generated WCF client, two hosted services,
five coordinated server/client recipes and a rich WPF operator UI. It therefore needs its own
scope decision even though both depend on MPNS and Windows Phone shell behavior.

## Live CNA and Sharp Runtime boundary

At CNA HEAD `5347b52eae13` and Sharp Runtime HEAD `9cc96cd57cde`, a focused source scan finds no
`HttpNotificationChannel`, Phone shell tile/toast/schedule, Phone application/page lifecycle,
WPF controls/animation/rendering, WCF `ServiceHost`, MEF composition or `IsolatedStorageSettings`
surface. Those are retired Phone/.NET desktop platform systems, not bounded defects in an XNA
API implementation.

A faithful result would have to retain both deployed products and define a reusable contract for
channel/subscription identity, foreground versus background ownership, tile/toast/raw delivery,
scheduled image fetches, response-state correlation, persistent settings, service hosting and a
complete operator UI. The mandatory browser target additionally needs permission, service-worker,
HTTPS-origin and browser-owned notification behavior that are not wire-compatible with MPNS.
A local echo, synthetic channel URI, inert UI or server without a receiving shell would fabricate
the behavior and violate the zero-workaround rule.

No CNA or Sharp Runtime source change was made. Native OPENGLES3 and WEBGL2 artifacts do not apply
to an absent, non-XNA port before the owner chooses a modernization/non-port boundary.

## Evidence and reproducibility

- `evidence/solution-msbuild-release.log` retains both successful desktop builds and both precisely
  classified later solution failures;
- `evidence/original-build/` and `original-build-sha256.txt` retain the tested PE outputs;
- `evidence/original-server-*.png`, `original-server-window.txt` and the X11 diagnostics retain all
  five offline/headless WPF states and the eight-core affinity;
- `evidence/WindowsPhonePushRecipe_V2.pdf`, `recipe-document.txt`, `recipe-pdf-info.txt` and
  `docx-metadata.txt` retain the complete documentation audit;
- `evidence/sample124-reuse-comparison.txt` and `sample105-overlap.txt` retain both relationship
  measurements;
- `evidence/live-runtime-surface-search.txt` records the live repository HEADs and zero matching
  platform surfaces;
- `scripts/qualify.sh` reproduces the complete offline audit, cleans generated `bin/obj` trees and
  constrains both MSBuild and the Wine process to CPU 0–7.

## Owner decision required

Choose one:

1. accept an evidence-backed historical Windows Phone/MPNS/WPF/WCF non-port boundary;
2. provide and authorize a faithful retired-platform environment/service route and port the full
   Phone client, message library and WPF/WCF server without replacing their contracts; or
3. authorize a complete modern two-product notification system and explicitly define native and
   browser channel, permission, shell/UI, service, persistence, scheduling and deployment
   semantics, including which MPNS-visible differences are accepted.

Retaining the reused progress control as SAMPLE-124 support data is compatible with any choice,
but it does not resolve this product. Do not port only the server, only the progress control, only
the five pattern names or a local notification loop and call SAMPLE-139 complete.
