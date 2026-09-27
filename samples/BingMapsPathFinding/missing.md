# Missing / Differences from XNA 4.0 original

**Status: cancelled by the owner on 2026-09-09 (`⛔`). No C++ port has been started.** This is a
distinct path-finding game layered on the Bing Maps viewer, not a duplicate directory and not an
offline path-finding algorithm. Its tank follows road geometry returned by the live Bing Maps
Routes service. The unchanged source deliberately refuses to compile until the developer supplies
a Bing Maps key, and that service's free account class is retired. Fake road coordinates, cached
screenshots or straight lines between pushpins would remove the sample's defining behavior.

Source: `/rv/tmp/XNAGameStudio/Samples/BingMapsPathFinding_4_0/`.

Retained audit root: `/rv/tmp/samples/SAMPLE-089-BingMapsPathFinding_4_0/`.

## Audited original

The package contains one Windows Phone/Reach application. Its nine runtime source units contain
2,612 lines; AssemblyInfo adds 34 metadata lines. The 878-line supplied documentation describes
the same product observed in the source. The application:

- starts at Microsoft's Redmond campus (`47.639597, -122.12845`) at zoom level 17;
- maintains the same 5x5 in-memory static-imagery plane and up to 25 asynchronous image requests
  as SAMPLE-088, with Web Mercator conversion, touch panning, Aerial/Road switching and location
  search through the Bing Locations XML endpoint;
- lets the player tap the map to append ordered pushpins and uses the Bing Routes endpoint to
  obtain the actual road geometry for every segment;
- supports both Driving and Walking routes, recalculating the full remaining path when the mode or
  pushpin set changes;
- moves and rotates a tank along the returned WGS-84 route points, visits every pushpin in order,
  and lets the player drag the stationary tank to a new start position;
- optionally draws the current and queued route geometry as red indexed `LineList` primitives;
- supports hold-to-delete with a Guide confirmation dialog, clearing all pushpins, and typed
  recentering through Guide keyboard input;
- uses fullscreen 30 Hz Phone behavior and GamePad Back exit.

Four support units are byte-identical to SAMPLE-088 (`BingMapsTiles`, `Button`,
`TileInformation`, `TileSystem`). `BingMapsViewer` deliberately exposes coordinate/offset
information needed by the tank. `Tank`, `PushPin`, `RouteRender` and most of the main game are
new behavior. This is therefore not accurately classified as merely another copy of SAMPLE-088.

As in SAMPLE-088, the `System.IO.IsolatedStorage` import does not implement persistent caching.
The active tile plane is only an in-memory request/image cache. An offline tile or route bundle
would be invented behavior.

`BingMapsSampleGame.cs` intentionally contains:

```csharp
#error For the sample to work, you need to acquire a Bing Maps key. See http://www.bingmapsportal.com/
const string BingAppKey = "<Bing Maps API Key>";
```

The retained `scripts/build-original.sh` runs the official XNA 4.0 content pipeline and verifies
that the exact main source stops with the expected `CS1029`. It separately type-checks all eight
unchanged support units against compatible local XNA 4.0 Windows references. That diagnostic
assembly succeeds and has SHA-256
`cdfd420523dd83b858dc471426505cd8200669b4ee65078b97208b6b4dcbacca`.

The official pipeline builds all five exact WindowsPhone/Reach version-5 XNBs:

| Output | SHA-256 |
|---|---|
| `Font.xnb` | `10d52efa8af488930211953420af4594df5e0d0c9bcb986d201fb21c56845668` |
| `Pushpin.xnb` | `7e83c452731fad7fd192c261598580a87eb66f1cc3e1b07c4801c400b8bcabbc` |
| `blank.xnb` | `75eea224fe584e02e7d9b99689ee4ae864b7e1294d6ba9cfeeb32c8d3d0c5e89` |
| `noImage.xnb` | `19ff261edc0fdd0442fd018321887fbc67daf12b3bd125fa7200357ecd7164df` |
| `tank.xnb` | `6e5067a52f88e89b9f595cc9a3c47000220ae17106cc2fafac22ad88b33484c3` |

No original runtime claim is made: the package contains no credential and there is no authorized
service account. No request was sent with the placeholder key and no private credential was
searched for or invented.

## External-service boundary (reverified 2026-09-27)

The exact source sends plain-HTTP requests to three `dev.virtualearth.net` REST resources:
Imagery/Map, Locations and Routes. Its route request selects Driving or Walking, asks for distance
optimization and route points in XML, then walks each returned `Point` latitude/longitude pair.

Microsoft's current documentation says the Bing Maps Calculate a Route API is deprecated, free
Basic accounts are retired, and existing Enterprise customers may use it only until 2028-06-30.
Microsoft directs migration to Azure Maps Route Directions:

- [Bing Maps Routes API and retirement notice](https://learn.microsoft.com/en-us/bingmaps/rest-services/routes/)
- [Bing Calculate Route to Azure Maps migration](https://learn.microsoft.com/en-us/azure/azure-maps/migrate-calculate-route)
- [Current Azure Maps Route Directions REST contract](https://learn.microsoft.com/en-us/rest/api/maps/route/post-route-directions?view=rest-maps-2026-01-01)

That migration is substantive. The current Azure route contract is an authenticated HTTP `POST`
with a GeoJSON body and returns different route geometry/field shapes; it is not a hostname or key
substitution. SAMPLE-089 also still needs the imagery and location migrations documented for
SAMPLE-088. An HTTPS WEBGL2 page blocks the originals' HTTP requests as mixed content, and browser
requests additionally require an explicit credential exposure/origin/CORS policy.

## Historical CNA and Sharp Runtime audit

The audit used CNA `35268971c` and the clean Sharp Runtime `next` checkout at `bd282d1016`; the
owner's separate in-progress `xml` branch was not inspected or modified.

CNA already has the bounded XNA-side pieces: `DrawableGameComponent`, component scheduling,
`BasicEffect`, `VertexPositionColor`, indexed `LineList` drawing, touch states and Hold/FreeDrag
gestures, Guide keyboard/message boxes, SpriteBatch, `Texture2D::FromStream` and the exact XNB
readers. No distinct graphics workaround or renderer blocker was found during this source audit.

Sharp Runtime already has `System::Uri`, `System::Guid`, `System::Text::StringBuilder`, generic
`LinkedList` and XML/LINQ building blocks. The reusable gaps are:

- no `System::Device::Location::GeoCoordinate`;
- no `System::Net::WebClient`, `OpenReadCompletedEventArgs`/handler, `OpenReadAsync`, `IsBusy` or
  `CancelAsync` implementation;
- no `XDocument::Load(System::IO::Stream&)` overload;
- no `XNode::CreateReader()` integration, and the existing `XmlReader` lacks the exact `EOF`,
  `HasValue` and `ReadContentAsString` route-parser surface;
- no native-plus-Emscripten asynchronous HTTP layer preserving concurrent completion,
  cancellation, user-state identity, streams and errors;
- for a durable Azure migration, no sample-independent authenticated POST/body/header/GeoJSON
  service path and no owner-selected browser credential/CORS policy.

The project references `System.Xml.Serialization`, but no runtime source uses `XmlSerializer`.
SAMPLE-089 is therefore unrelated to `SAMPLES-DEC-008`; do not wait on, modify or broaden the
owner's in-progress XML-serializer branch for this audit.

The coordinate and XML compatibility additions are bounded. The cross-platform async HTTP layer,
live service/account decision, secure browser policy and three-endpoint migration are the material
work. They belong in Sharp Runtime/platform and shared service integration, never as curl/fetch or
hardcoded response logic inside this sample.

## Historical decision options before owner cancellation

No C++ source, CMake target, embedded credential, fake route server, cached screenshot grid,
straight-line fallback or other workaround was added. Before the owner cancelled this row under
`SAMPLES-DEC-004`, the available boundaries were:

1. accept an evidence-backed retired/free-service/non-port result for this distinct route game;
2. supply access to an eligible existing Bing Maps Enterprise account and authorize the reusable
   HTTP/GeoCoordinate/XML work, secure runtime credential injection, HTTPS update and real
   native/browser qualification, acknowledging the service's 2028 retirement;
3. authorize an Azure Maps imagery/search/route migration as a documented service modernization,
   provide its account/authentication and browser CORS policy, and authorize the reusable async
   HTTP POST plus response-format work.

Recorded XML/image/route fixtures can make tile math, route parsing, motion and cancellation tests
deterministic, but cannot alone satisfy the original online map and road-routing product. If option
2 or 3 is selected, keep credentials outside Git, add deterministic tests plus a separately
authorized live canary, and qualify imagery, map drag, view/mode switching, location search,
pushpin add/delete/recalculation, tank motion and visible route geometry on native OPENGLES3 and
real-browser WEBGL2.

---

## Re-audited 2026-09-09: SAMPLE-088's boundary, with a third service on top

The transport and parser size estimate in this historical re-audit was incomplete. The current-head
analysis below corrects it without reopening the owner's cancellation.

This sample is the superset of SAMPLE-088 — 10 sources and 2,646 lines against its 7 and 1,486 —
and it inherits that sample's boundary unchanged, including the same `#error For the sample to work,
you need to acquire a Bing Maps key` at `BingMapsSampleGame.cs:58`.

**It calls three Bing REST services, not one:**

- `dev.virtualearth.net/REST/V1/Imagery/Map/` — the tile imagery SAMPLE-088 also uses
- `dev.virtualearth.net/REST/v1/Locations` — geocoding, also shared
- `dev.virtualearth.net/REST/V1/Routes/` — **new here**, and the reason this is a distinct lesson:
  Driving/Walking route requests whose returned road geometry the tank then traverses

So an Azure Maps migration would have to replace all three and change the route representation, not
adapt one endpoint.

**The runtime side looked smaller than the initial audit implied.** Re-measured against the
then-current Sharp Runtime, `XDocument`, `XElement` and `XNamespace` all **exist** in
`modules/xml-linq`; `HttpClient` already offers `PostAsync` and `SendAsync`, and `net-http-json`
and `text-json` exist. These are POST and JSON primitives, not a usable authenticated native/browser
transport. That pass reduced the list to SAMPLE-088's
`XDocument.Load(Stream)`, `WebClient` and `GeoCoordinate` (used 35 times here against 16 there), but
did not account for the route reader or the HTTP transport limits found below.

**The blocker is unchanged and is the service.** As with SAMPLE-088, the sample's whole visible
output is Microsoft's data: the map beneath, the geocoded destination, and the road the route
follows. Nothing in the upstream sample can stand in for any of the three, the free tier is retired,
and the paid tier is announced to end in 2028.

## CANCELLED by the owner, 2026-09-09

No port will be produced for SAMPLE-089, on SAMPLE-088's boundary plus the `Routes` service. The
runtime gaps stay recorded because they are real and relevant to future online/XML samples; their
full scope is measured below.

## Current-head re-analysis — 2026-09-27

SAMPLE-088 remains cancelled; this pass reviewed SAMPLE-089 without changing the owner's decision.
All **25** physical upstream files match `xna4-original/` byte-for-byte. The one Windows Phone/Reach
project contains nine runtime C# units with 2,612 lines. Its main unit still stops at the deliberate
`#error` on `BingMapsSampleGame.cs:58` and the following placeholder Bing key. Thus no original
game executable or authorized live-service reference can be run here. The unchanged eight-unit
support diagnostic and all five official `XNBm` outputs have their previously recorded SHA-256
hashes. Exact inventory and current repository heads are in
`/rv/tmp/samples/SAMPLE-089-BingMapsPathFinding_4_0/evidence/current-head-analysis-20260927/inventory.json`.
No rebuild was needed for this unchanged-source, owner-cancelled re-analysis.

The distinct route behavior is present in `Tank.cs`: each consecutive tank/pushpin segment makes
an asynchronous Bing Routes request for Driving or Walking, with `optmz=distance`, `output=xml` and
`rpo=points`; its returned `Point` latitude/longitude pairs become the path that turns and moves
the tank. The viewer still requests up to 25 live map images, and search still calls Locations.
No static route fixture or straight line can reproduce those three live services. Microsoft's
[current Bing Routes notice](https://learn.microsoft.com/en-us/bingmaps/rest-services/routes/)
confirms that free Basic accounts are retired and existing Enterprise access ends **2028-06-30**.
An eligible Enterprise customer may still use it now; this workspace has no such credential or
reference capture. Microsoft's [route migration guide](https://learn.microsoft.com/en-us/azure/azure-maps/migrate-calculate-route)
specifies Azure Maps `POST` plus GeoJSON and longitude/latitude coordinates in place of the Bing
`GET`/XML and latitude/longitude contract. Imagery and geocoding need separate migrations too.

On current Sharp Runtime `next 9e58c955`, `WebClient` with `OpenReadAsync`/completion/user-state/
cancellation, `GeoCoordinate` and `XDocument.Load(Stream)` remain absent. There is a distinct
SAMPLE-089 XML gap: `Tank.cs:391-418` calls `XDocument.CreateReader()` and uses `XmlReader.EOF`,
`HasValue` and `ReadContentAsString()`; these are still absent. `HttpClient.PostAsync`, `SendAsync`
and JSON modules do exist, correcting the old blanket claim of missing POST/JSON primitives.
They do **not** provide a working Azure transport: the current `HttpClient` only accepts plain HTTP,
rejects HTTPS, and its Emscripten handler throws `PlatformNotSupportedException`. A faithful
native/browser implementation would require reusable HTTPS, browser Fetch and async response/
cancellation behavior, plus an authorized credential and CORS policy; the XML reader work is
additional to SAMPLE-088. The original's `System.Xml.Serialization` project reference remains
unused in runtime source.

No live service request, rebuild, native/browser run, port, workaround or dependency source change
was made during this re-analysis. The owner's `⛔` cancellation remains in force.
