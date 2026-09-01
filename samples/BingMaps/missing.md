# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked. No C++ port has been started.** The sample is
an online Windows Phone map client, not a self-contained tile-math demo. Its unchanged source
deliberately refuses to compile until the developer supplies a Bing Maps key, and the free account
class for that service is now retired. A faithful result needs an owner-selected live map service,
credential policy and reusable native/browser HTTP implementation; checked-in map screenshots or
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

## Current external-service boundary

The source calls two exact endpoints on `dev.virtualearth.net`: Imagery/Map for every tile and
Locations for geocoding. Microsoft's current documentation says Bing Maps for Enterprise is
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

## Live CNA and Sharp Runtime audit

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

## Current result and resume conditions

No C++ source, CMake target, embedded credential, cached screenshot grid, fake geocoder or other
workaround was added. SAMPLE-088 remains `🛑` under `SAMPLES-DEC-004` until the owner chooses one
of these boundaries:

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
