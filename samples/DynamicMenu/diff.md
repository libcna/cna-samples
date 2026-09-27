# SAMPLE-077 — intentional C++ plumbing

This file records two pieces of port infrastructure that the original C# source cannot need,
plus one owner-requested desktop input addition. The infrastructure does not change DynamicMenu
behavior. The input addition is described below.

## AOT registration for reflective XNB types

XNA discovers `ReflectiveReader<T>` types, inherited serialized members and polymorphic concrete
types through .NET reflection. CNA deliberately has no runtime reflection, so the C++ port calls
`CNAEXT DynamicMenu::Controls::RegisterDynamicMenuContentReaders()` once in the game constructor.
The closed registration describes the same original control classes and serialized properties in
the same order; it does not parse XML, alter an XNB or build either menu graph by hand.

The registration is the compile-time counterpart of XNA's automatic type discovery. Both Page 2
and Page 3 still travel through ordinary `Content.Load` and the authentic official-pipeline XNBs.
The C# call requests `Container`; the C++ call requests `std::shared_ptr<IControl>` and immediately
casts the polymorphic root to `Container`, matching the pointer shape used by the XNB's
`List<IControl>` graph. The `CNAEXT` marker makes the non-XNA setup line explicit.

## Two original assemblies in one executable

The original solution builds the reusable `DynamicMenu` library and `DynamicMenuSample` game as
separate managed assemblies, each with its own `AssemblyInfo.cs`. The CNA sample is one native
executable, but retains both metadata units in the matching source directories. Only the game
assembly title is process-active so the native and browser title remains `DynamicMenuSample`; the
library title remains documented in its own translation unit. This represents the original
assembly boundary without inventing a second runtime process or changing game code.

## Owner-requested desktop mouse input — 2026-09-27

The port enabled `TouchPanel::setMouseTouchEmulationEnabledEXT(true)` on 2026-09-08 so mouse
clicks could drive the original Tap path on a desktop. That added behavior was absent from the
Windows Phone original. It was removed during the first 2026-09-27 requalification because no
SAMPLE-077 owner request was recorded then. The owner subsequently reported that both OPENGLES3
and WEBGL2 were unusable with a mouse and explicitly requested the mouse-as-touch emulation.
The current port therefore restores this one `CNAEXT` opt-in in the game constructor. CNA keeps
the extension disabled by default; the opt-in affects this sample only.

The extension maps a desktop left mouse press/release to the existing `TouchPanel` Tap gesture.
It does not change any menu algorithm, XML content, drawing, touch handling or GamePad Back path.
Actual touch events continue to work. Current native mouse captures are pixel-identical to the
earlier native genuine-finger captures at five states; current browser mouse and genuine-touch
captures are also identical at five states. See `missing.md` for the qualification evidence.
