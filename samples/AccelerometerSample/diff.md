# SAMPLE-084 intentional translation seams

There are no known observable behavior or content differences from the original Windows Phone
sample. The following are language/host representation choices, not sample workarounds.

## Desktop and browser emulator controls

The owner explicitly requested that arrow-key steering remain available on both OPENGLES3 and
WEBGL2 in place of a physical accelerometer. This uses the unchanged arrow-key branch of the
original Windows Phone emulator sample. CNA generally reports `DeviceType::Emulator` on desktop
and web, while Android/iOS report `DeviceType::Device`; the sample itself contains no platform
switch or added input mapping. The 2026-09-09 native executable entered the old runtime's
`Device` branch and therefore ignored arrows without sensor hardware. A rebuild against current
CNA selects the original emulator branch and responds to Right and Up on native and web.

## Static state and event synchronization

C# static fields become C++ inline static fields. The callback's `lock (threadLock)` becomes a
`std::mutex` plus `std::scoped_lock`; it protects the same single latest `Vector3` value in both
the event and polling paths. The C# event-handler method is represented by an equivalent lambda
subscribed to CNA's normal `ReadingChanged` event. The sensor object, exception boundary,
initialization guard and branch order are unchanged.

`AccelerometerState.Acceleration` remains a value property: its C++ getter returns `Vector3` by
value. C#'s private setters are represented by private fields initialized by the public
constructor. `ToString()` composes the same vector and Boolean values.

## Game ownership

C# nullable graphics references become `optional<Texture2D>` and the owned `SpriteBatch` becomes
`unique_ptr<SpriteBatch>`. `GraphicsDeviceManager` remains a game-owned value. These choices only
express C++ lifetime; content names, loading order and frame behavior are unchanged.

## Executable host

The phone project is launched by the Windows Phone application host and its dormant desktop-only
`Program.cs` refers to the template's obsolete `Game1` name. CNA needs a normal native/Wasm entry
point, so `Program.cpp` constructs the actual logical `AccelerometerSample::Game` and calls
`Run()`. It adds no game input, state or platform behavior.

The original-reference diagnostic similarly adds a tiny host because the shipping project is a
phone application library. Its phone shim exists only in the retained audit artifact and is never
compiled into the CNA sample.
