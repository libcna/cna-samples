# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked. No C++ port has been started.** This Windows
Phone 7 sample is a live GPS car finder. Its defining input is not a fixed coordinate or a portable
piece of math: a high-accuracy `System.Device.Location.GeoCoordinateWatcher` supplies permission/
status transitions and changing position, accuracy, speed and course values. Live CNA and Sharp
Runtime have no `System.Device.Location` API or location backend, and CNA explicitly records it as
a future, unimplemented subsystem. Static, random or sample-scripted coordinates would replace the
lesson rather than port it, so none were added.

Retained audit root:
`/rv/tmp/samples/SAMPLE-095-GeolocationSample_4_0/`.

Exact upstream snapshot:
`/rv/tmp/samples/SAMPLE-095-GeolocationSample_4_0/xna4-original/`.

## Audited original

The 19-file package contains 932 lines of C# and one Windows Phone/Reach game endpoint:

- `Game1.cs` is 517 lines and owns the real location service, app lifecycle, distance/bearing math,
  touch UI, compass rendering and persistence;
- `UIElement`, `TextBox` and `Button` contribute all 360 lines of the sample-owned touch UI;
- `Program.cs` and assembly metadata contribute the remaining 55 lines;
- the content project contains the exact Arial 14 SpriteFont and the arrow, circle and spot images.

The constructor requests `GeoPositionAccuracy.High`, subscribes to `StatusChanged` and
`PositionChanged`, sets `MovementThreshold = 0.5` and starts the watcher. `Disabled` distinguishes
permission denial from unsupported hardware and displays the corresponding Guide message before
exit. `Ready` publishes the current position; later changes are accepted only while Ready.

The sample preserves substantially more behavior than displaying latitude and longitude:

- current and saved timestamps, latitude, longitude, altitude, speed, course, horizontal/vertical
  accuracy and update count are rendered at 480×800 fullscreen and 30 Hz;
- Save stores the watcher's current position, then `GeoCoordinate.GetDistanceTo` and the sample's
  spherical initial-bearing calculation update metres/feet, m/s, km/h and mph;
- the green arrow shows current course and the red spot the saved-location bearing, with both
  North-up and direction-up modes;
- touch press/release identity, button fill/outline states and exact UI layout are sample-owned;
- deactivation writes timestamp ticks, UTC-offset ticks and seven coordinate doubles to isolated
  storage; activation reconstructs the same `DateTimeOffset`, `GeoCoordinate` and `GeoPosition`;
- GamePad Back exits.

The documentation describes the intended physical workflow: save a parked car's position, walk
with the phone, and follow changing distance, direction and speed back to it. The sample has no
fallback track and no keyboard/emulator GPS route in its own source. Windows Phone's external
emulator tooling or real hardware supplied location data.

## Authentic build evidence

The retained `scripts/build-original.sh` uses the official offline XNA 4.0 pipeline on the exact
unchanged content declarations for both WindowsPhone/Reach and Windows/Reach. Both passes succeed,
producing eight version-5 XNBs with truthful platform markers. The exact unchanged six C# units
also compile as a diagnostic library against XNA 4 and the compatible .NET 4
`System.Device.Location` surface; no location shim or source edit is used.

| Asset | Windows SHA-256 | Windows Phone SHA-256 |
|---|---|---|
| `Font.xnb` | `cd14bf5de3cc2506e6beb775b436dd7900d8eb98e0bc108be52765b8620fa339` | `e0e1dd786b5efab8f66348dc6d4ee8df51bd804ffebfd47b6d203cc28e7c83f7` |
| `arrow.xnb` | `75e08ad89e81222725fa164473b6043fd783b719717832f73a8c5cdf12744c5f` | `03afe03ac91e7e461ffa48f2baac4f4498f9404cc46441d579075717c4a40762` |
| `circle.xnb` | `515699759400b836ded9cf4988254dd395ffc2ecf0881dbba8697bdcba8b4f3e` | `7b8220094838b2317dc978da5a0a906395cb8eeaadeb1f31a562be4ab5564aaa` |
| `spot.xnb` | `adca59435168be0181289cb354bc08048e909965a22e154e1504597ae3e1e923` | `cc91cf5a107078f103d31246c05f2986a214b1341a5abb458d914df4867a7eb8` |

The diagnostic library SHA-256 is
`a808912d8ae1463aaa64a8ed8a6ad83e77007ae9e88da52cfdd9d8a1be9e9c2a`. The complete build
transcript, live dependency audit and hash manifest are retained under `evidence/`. There is no
desktop XNA game project and no Windows Phone runtime/emulator in this audit environment, so no
false original-runtime or physical-GPS claim is made.

## Live CNA and Sharp Runtime audit

The audit used CNA `e5ae0820e234` and Sharp Runtime `next` at `bd282d101640`. No
`GeoCoordinate`, `GeoPosition<T>`, `GeoCoordinateWatcher`, location enums/event args or
`System::Device::Location` namespace exists in live implementation code. CNA's
`docs/location-future-plan.md` explicitly says that nothing in the document is implemented and
that location remains unscheduled.

That document also records the correct layering: GPS is a real `System.Device.Location` API, not
part of XNA's `Microsoft.Devices.Sensors`. Reusing the Accelerometer/Compass classes or attaching a
GPS member to them would create the wrong public API. A reusable implementation requires at least:

- Sharp Runtime's exact `System::Device::Location` value types, generics, permission/status enums,
  properties and events;
- a CNA-owned platform seam with thread/lifecycle and movement-threshold semantics;
- real provider/permission integrations such as Android LocationManager, Apple Core Location and
  W3C browser Geolocation, plus an honest native-desktop policy/provider;
- deterministic test-only injection behind that seam, while the shipped sample still consumes a
  real provider and never hardcodes a route;
- real permission-denied, unsupported, no-data and ready regressions, followed by native and
  browser multi-position qualification of distance, bearing, persistence and both compass modes.

SDL3 itself has no geolocation API, so this is not a bounded missing function. Implementing only
the structs would make the source compile but leave the product permanently Initializing/Disabled
and demonstrate none of its advertised behavior.

This sample does **not** depend on the pending `System.Xml.Serialization` work. Its project carries
the historical WP7 XML reference, but its documentation and source deliberately avoid
`XmlSerializer` because of `DateTimeOffset` and use the explicit binary format described above.

## Current result and resume conditions

No C++ source, CMake target, fake GPS trace, location shim, CNA change or Sharp Runtime change was
added. SAMPLE-095 remains `🛑` under `SAMPLES-DEC-004` until the owner chooses one of these scopes:

1. accept this evidence-backed Windows-Phone/location-hardware-only non-port boundary;
2. authorize the cross-repository `System.Device.Location` API and real CNA platform backends,
   including permissions, lifecycle, a test seam and native/WEBGL2 qualification;
3. explicitly approve a narrower platform boundary—for example real browser/mobile location with
   an honestly unavailable desktop route—and define how that exception changes the campaign's
   native parity gate.

If option 2 is authorized, implement the runtime/platform layer as its own committed task first.
Then port all 932 lines, retain the four selected official XNBs, and qualify multiple real or
test-seam positions, Save, persistence across deactivate/activate, both compass orientations,
permission denial, unsupported/no-data states, touch controls and clean exit. A trace is acceptable
as deterministic test infrastructure only after the production provider contract exists; it is not
acceptable as the product implementation.
