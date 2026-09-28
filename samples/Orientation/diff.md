# SAMPLE-102 intentional translation seams

The owner requested the input additions below. Other seams are language/host representation
choices, not sample workarounds.

## Owner-requested keyboard orientation — 2026-09-28

The constructor opts into the independent, off-by-default
`GameWindow::setKeyboardOrientationEmulationEnabledEXT(true)` CNA extension. Up requests
Portrait, Left requests LandscapeLeft and Right requests LandscapeRight. CNA applies the
request through the ordinary graphics-device lifecycle and respects SupportedOrientations;
a request blocked by locking takes effect after unlocking. The sample contains no keyboard
handler, direct resize, renderer call, fake orientation state or Escape action.

To make this requested interaction visible, this port now selects the **existing original
scenario #4** by uncommenting its all-orientations configuration and enableOrientationLocking.
The upstream download defaults to scenario #1. This deliberate configuration difference is
recorded separately from the shared emulation opt-in; scenarios #1–#3 remain explained in source.
The original tap branch, drawing, 30 Hz timing, fullscreen request and Gamepad Back exit are
preserved. Left-click follows that original Tap branch through the previously approved input opt-in.

CNA also adds a separate `Accelerometer::setKeyboardEmulationEnabledEXT(bool)` extension.
**102 does not enable it**: orientation and accelerometer emulation are independent features.
See sibling CNA's `docs/keyboard-device-emulation.md` for their API and lifecycle contracts.

## Owner-requested mouse input — 2026-09-27

At this earlier qualification stage, shipped scenario #1 displayed the guide with no active touch action. The
preserved scenario #4 branch uses a Tap gesture to lock or unlock orientation if a reader enables
it in source. At the owner's request the constructor opts into CNA's off-by-default
`TouchPanel::setMouseTouchEmulationEnabledEXT(true)` extension, so a left mouse click can follow
that same Tap path. This input opt-in itself does not enable scenario #4, change orientation rules, add a separate mouse
handler or replace actual touch input. The one source addition is marked `CNAEXT`.

## Object ownership and nullable content

The C#-owned `GraphicsDeviceManager` becomes a game-owned C++ value. `SpriteBatch` uses
`std::unique_ptr`, while the two content values use `std::optional` so their construction remains in
`LoadContent()`. These choices express C++ lifetime only; construction order, content identifiers,
loading order and use are unchanged.

The original private fields, lifecycle overrides and branch order remain private/protected in C++.
The CNAEXT `GetTypeName()` override supplies the fully qualified managed identity
`OrientationSample.OrientationSample`; it does not add a game feature.

## Executable host

The shipping Windows Phone project is launched by the phone application host and preprocessor-
excludes the dormant desktop `Program.Main`. CNA needs a normal native/Wasm entry point, so
`Program.cpp` constructs the same logical `OrientationSample::OrientationSample` class and calls
`Run()`. Stack lifetime provides the disposal boundary that the C# `using` statement supplies. No
input, orientation state or rendering behavior is added.

`AssemblyInfo.cs` metadata with runtime significance is represented by CNA's assembly-title
attribute so the product title remains `OrientationSample`; the remaining CLR-only package metadata
has no game behavior.

## Excluded source

`LayoutSample.cs` is deliberately not translated because the authoritative phone `.csproj` does not
compile it and no shipped entry point references it. Treating it as a second target would expand the
original product rather than port it.

## Package metadata restored during scenario #1 qualification — 2026-09-28

Game.ico, GameThumbnail.png, Background.png and the original Microsoft license are retained
byte-identically at the port root. The Windows diagnostic embeds Game.ico; the runtime still loads
only the original directions and Font XNBs. Background.png is Phone tile metadata, not a new scene.
That earlier qualification added no game logic or new input deviation.
