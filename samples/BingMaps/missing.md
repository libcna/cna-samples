# Missing / Differences from XNA 4.0 original

**Status: cancelled by the owner on 2026-09-09 (`⛔`). No C++ port has been started.** The sample is
an online Windows Phone map client, not a self-contained tile-math demo. Its unchanged source
deliberately refuses to compile until the developer supplies a Bing Maps key, and the free account
class for that service is now retired. A faithful result needs an owner-selected live map service,
credential policy and reusable native/browser HTTPS implementation; checked-in map screenshots or
an always-`noImage` grid would not be the original product.

Source: `/rv/tmp/XNAGameStudio/Samples/BingMaps_4_0/`.

Retained audit root: `/rv/tmp/samples/SAMPLE-088-BingMaps_4_0/`.

## Audited original

The package contains one Windows Phone/Reach application. Its six runtime source units contain
1,452 lines; AssemblyInfo adds 34 metadata lines. The application:

- starts at Microsoft's Redmond campus (`47.639597, -122.12845`) at zoom level 15;
- asynchronously requests a 5x5 in-memory plane of full-screen map images from Bing Maps REST,
  maintaining cancellation/completion state for as many as 25 tile `WebClient` requests;
- calculates WGS-84 latitude/longitude to global Web Mercator pixel coordinates and back, then
  derives the center coordinate of every surrounding tile-sized image request;
- displays placeholders while images are pending or unavailable and decodes each response stream
  with `Texture2D.FromStream`;
- pans within the loaded plane through `FreeDrag`, recenters by entering a place through Guide's
  keyboard, parses the Bing Locations XML response with LINQ to XML, and reloads around that point;
- switches live requests between Aerial and Road imagery through the on-screen touch button;
- uses fullscreen 30 Hz Phone behavior and GamePad Back exit.

Despite importing `System.IO.IsolatedStorage`, this version does not write a persistent disk cache.
Its "cache" is the active 5x5 object/image plane plus per-request byte buffers. An offline tile
bundle would therefore be a new behavior, not recovery of an omitted original cache.

`BingMapsSampleGame.cs` intentionally contains:

```csharp
#error For the sample to work, you need to acquire a Bing Maps key. See http://www.bingmapsportal.com/
const string BingAppKey = "<Bing Maps API Key>";
```

The retained `scripts/build-original.sh` verifies that the exact source stops with the expected
`CS1029` rather than pretending the placeholder is runnable. It separately type-checks the other
five unchanged logical units against the compatible local XNA 4.0 Windows references; that
1,110-line support assembly passes and has SHA-256
`75251023a16264bbbb24169b28c94be77b55adc716bb71cd297bf644d6dd07bf`.

The official XNA pipeline builds all three exact WindowsPhone/Reach version-5 XNBs:

| Output | SHA-256 |
|---|---|
| `Font.xnb` | `10d52efa8af488930211953420af4594df5e0d0c9bcb986d201fb21c56845668` |
| `blank.xnb` | `75eea224fe584e02e7d9b99689ee4ae864b7e1294d6ba9cfeeb32c8d3d0c5e89` |
| `noImage.xnb` | `19ff261edc0fdd0442fd018321887fbc67daf12b3bd125fa7200357ecd7164df` |

The font is the authentic Moire ExtraBold 12 face. No original runtime claim is made: there is no
credential in the package and no local Windows Phone host with an authorized service account.

## External-service boundary (reverified 2026-09-27)

The source calls two exact endpoints on `dev.virtualearth.net`: Imagery/Map for every tile and
Locations for geocoding. Microsoft's documentation still says Bing Maps for Enterprise is
deprecated, has already retired all free Basic accounts, and permits existing Enterprise customers
only until 2028-06-30. Microsoft directs new migration work to Azure Maps:

- [Bing Maps Dev Center retirement notice](https://learn.microsoft.com/en-us/bingmaps/getting-started/bing-maps-dev-center-help/)
- [Bing Maps REST base URL and HTTPS form](https://learn.microsoft.com/en-us/bingmaps/rest-services/common-parameters-and-types/base-url-structure)
- [Azure Maps authentication and browser CORS policy](https://learn.microsoft.com/en-us/azure/azure-maps/azure-maps-authentication)

The original strings use plain HTTP. That was valid for a 2010 Phone application, but an HTTPS
WEBGL2 page blocks such mixed content, while serving the whole application over HTTP would expose
the credential and responses. Cross-origin browser access is a separate service policy. Bing's
documented HTTPS form still needs an eligible key. An Azure migration changes endpoints, response
formats, imagery naming, authentication and account-configured CORS; it can preserve the
user-facing map workflow, but is an explicit service modernization rather than wire-level
XNA-source parity.

No request was sent with the placeholder key and no private credential was searched for or
invented.

## Historical CNA and Sharp Runtime audit

The audit used CNA `35268971c` and the clean Sharp Runtime `next` checkout at `bd282d1016`; the
owner's separate in-progress `xml` branch was not inspected or modified.

CNA already supplies the XNA pieces needed after bytes arrive: real touch gestures, Guide keyboard
input/message boxes, SpriteBatch, runtime `Texture2D::FromStream`, fullscreen/timing behavior and
the exact XNB readers. Sharp Runtime already has `System::Uri` and LINQ-to-XML names, descendants
and parsing. `System.Xml.Serialization` is listed in the old Phone project but no source uses
`XmlSerializer`, so SAMPLE-088 is unrelated to `SAMPLES-DEC-008` and must not wait on or distort
the owner's XML-serializer branch review.

The reusable gaps are instead:

- no `System::Device::Location::GeoCoordinate`;
- no `System::Net::WebClient`, `OpenReadCompletedEventArgs`/handler, `OpenReadAsync`, `IsBusy` or
  `CancelAsync` implementation;
- no `XDocument::Load(System::IO::Stream&)` overload for the geocoder response;
- no established native-plus-Emscripten HTTP/fetch contract that preserves concurrent completion,
  cancellation, user-state identity, response streams, errors and browser CORS behavior.

`GeoCoordinate` and the stream overload are bounded additions. The asynchronous cross-platform
network layer, browser credential/CORS setup and service choice are the expensive parts. They must
live in Sharp Runtime/platform infrastructure, not as curl/fetch code inside this sample. Per the
project's ownership rule, no speculative Sharp Runtime change was made while the live service and
credential policy remain undecided.

## Historical decision options before owner cancellation

No C++ source, CMake target, embedded credential, cached screenshot grid, fake geocoder or other
workaround was added. Before the owner cancelled this row under `SAMPLES-DEC-004`, the available
boundaries were:

1. accept an evidence-backed retired/free-service/non-port result;
2. supply access to an eligible existing Bing Maps Enterprise account and authorize the reusable
   HTTP/GeoCoordinate/stream work, secure runtime credential injection, HTTPS update and real
   native/browser qualification, acknowledging the service's 2028 retirement;
3. authorize an Azure Maps migration as a documented service modernization, provide its account/
   authentication policy and browser CORS origin, and authorize the same reusable runtime work.

An offline fixture mode can help deterministic unit tests for tile math, cancellation and XML
parsing, but it cannot by itself satisfy the original online navigation/geocoding product. If
option 2 or 3 is selected, keep credentials outside Git, add recorded/local deterministic tests
plus a separately authorized live canary, and qualify initial Redmond imagery, 5x5 loading,
dragging, Aerial/Road switching and typed-location recentering on native OPENGLES3 and real-browser
WEBGL2.

---

## Re-audited 2026-09-09 against then-current Sharp Runtime

The size estimate below assumed the existing HTTP stack could support the needed platforms. The
2026-09-27 audit found that assumption false; see the current-head section below.

The runtime gaps have narrowed since this was written, and they were never the real blocker. What
is left is smaller and more precisely nameable than "no `WebClient` surface".

**What the original actually needs, and what exists today:**

| needed | state |
| --- | --- |
| `XDocument` | **exists** — `modules/xml-linq` |
| `XDocument.Load(Stream)` | **missing one overload**: `Load` takes a file path; the sample calls `XDocument.Load(e.Result)` on a stream (`BingMapsSampleGame.cs:237`) |
| `WebClient` — `OpenReadAsync`, `OpenReadCompleted`, `IsBusy`, `CancelAsync` | **missing**; `modules/net-http` exists, but its current transport lacks HTTPS and Emscripten requests (corrected below) |
| `GeoCoordinate` | **missing** — a small `System.Device.Location` value type, used 16 times |

The 2026-09-09 audit estimated this as a few hours of ordinary adapter work, comparable with
SAMPLE-071's `IXmlSerializable` and Base64/BinHex additions. That estimate omitted transport.

**The blocker is the service, and it is not the shape SAMPLE-071's was.** Yacht was unblocked by
measuring that the retired MPNS was a *relay* between two halves of the sample, and that the service
itself was `Server.exe`, shipped in the box and runnable here. **BingMaps has no such half.** Its
entire visible output — the 5×5 plane of aerial and road tiles, and the geocoding that recentres it
— *is* Microsoft's imagery service. There is nothing in the upstream sample that could stand in for
it, and substituting different imagery would not be this sample.

On top of that the source refuses to compile at all until a key is supplied — `#error For the sample
to work, you need to acquire a Bing Maps key` at `BingMapsSampleGame.cs:58` — Microsoft has retired
free Basic accounts, and Enterprise is announced to sunset in 2028. So the credential is not merely
missing today; the service it authorises is scheduled to end.

**What that left at the time.** A C++ translation could compile after the missing API shapes were
added, but qualifying it would still require real transport and eligible live-service access; no
such key or account was available in this workspace. The decision was whether to obtain access,
authorize an Azure Maps migration (changing the demonstrated service), or record a non-port.

## CANCELLED by the owner, 2026-09-09

No port will be produced for SAMPLE-088. The required runtime pieces remain useful to future
samples, but no eligible Bing account or key was provided for this sample, its free tier is retired,
its Enterprise tier is scheduled to end in 2028, and the live imagery *is* the sample's output.

## Current-head re-analysis — 2026-09-27

SAMPLE-087 remains cancelled; this pass reviewed SAMPLE-088 without changing the owner's decision.
All **20** physical upstream files match the retained `xna4-original/` snapshot byte-for-byte. The
six C# runtime units still total 1,452 lines, and `BingMapsSampleGame.cs:58` still has the
intentional `#error` followed by the placeholder key. There is no unchanged-source game executable
to run. The retained five-unit support diagnostic and all three official Phone/Reach XNBs still
have the SHA-256 hashes listed above. Exact file hashes and repository heads are in
`/rv/tmp/samples/SAMPLE-088-BingMaps_4_0/evidence/current-head-analysis-20260927/inventory.json`.

Microsoft's [current Bing Maps REST notice](https://learn.microsoft.com/en-us/bingmaps/rest-services/)
confirms that free Basic accounts are retired and existing Enterprise customers can continue only
until **2028-06-30**. That does not prove no Enterprise customer can run this exact sample today;
this workspace simply has no authorized key or live reference capture. The original uses two HTTP
REST endpoints for the 25-map-image plane and Locations XML geocoding. A browser served over HTTPS
would need HTTPS requests, valid authentication and permitted cross-origin access. An Azure Maps
migration changes the service contract, not just the host name; Microsoft maps the old Imagery and
Locations operations to separate Render and Search APIs in its
[migration overview](https://learn.microsoft.com/en-us/azure/azure-maps/migrate-bing-maps-overview).

The current Sharp Runtime (`next 9e58c955`) still lacks `GeoCoordinate`, `WebClient` and its
`OpenReadAsync`/completion/cancellation surface, and `XDocument.Load(Stream)`. More substantially,
its existing `HttpClient` is **not** yet a cross-platform HTTPS transport: the public header says
it supports plain HTTP only, `parseUrl` rejects HTTPS because TLS is missing, and
`HttpClientHandler::Send` throws `PlatformNotSupportedException` on Emscripten. Therefore the old
"few hours" estimate for a `WebClient` adapter over a working native/browser HTTP stack was
unsubstantiated. Native TLS and browser Fetch/response-stream/cancellation integration remain
separate runtime work; service credentials, HTTPS and CORS still need an authorized product policy.
The missing coordinate value and XML stream overload are narrower issues, but implementing them
alone would not make this online map client runnable.

No live service request, rebuild, native/browser execution, sample workaround or dependency source
change was made during this re-analysis. The owner's `⛔` cancellation remains in force.
