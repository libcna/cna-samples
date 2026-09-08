# SAMPLE-077 — intentional C++ plumbing

This file records the two pieces of port infrastructure that the original C# source cannot need.
Neither changes DynamicMenu behavior.

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

## Platform seam: the mouse becomes a touch on a desktop

`DynamicMenuSample.cpp` calls `TouchPanel::setMouseTouchEmulationEnabledEXT(true)` beside the
`GestureType::Tap` the original enables. Added 2026-09-08.

Every control in this sample is driven by a Tap and nothing else, exactly as the original is: there
is no keyboard or mouse route in the C# either. A phone has a touch screen and a desktop does not,
so without this the sample builds and renders on a desktop and cannot be operated at all — which is
why the native evidence covered Page 1 only until now, while the browser (where CDP can dispatch
real touch events) covered every page.

The alternative was to grow a mouse input path inside the sample beside the original's gestures.
That is the invention the campaign forbids, and it is the one that had to be removed from
SAMPLE-072 and SAMPLE-073 before they could be called faithful. This is the same seam SAMPLE-071
takes, for the same stated reason.

The sample's own logic is untouched: the platform turns a mouse press into a touch, and the
existing `TouchPanel` gesture path consumes it unchanged.
