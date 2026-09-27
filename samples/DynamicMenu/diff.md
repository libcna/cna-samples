# SAMPLE-077 — intentional C++ plumbing

This file records two pieces of port infrastructure that the original C# source cannot need.
Neither changes DynamicMenu behavior. The 2026-09-08 desktop mouse-as-touch opt-in was removed
during current-head requalification because no SAMPLE-077 owner request for that added input
behavior was recorded. The port once again accepts the original touch and GamePad Back inputs only.

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

## Historical desktop input addition, removed 2026-09-27

The port enabled `TouchPanel::setMouseTouchEmulationEnabledEXT(true)` on 2026-09-08 so mouse
clicks could drive the original Tap path on a desktop. That added behavior was absent from the
Windows Phone original. SAMPLE-071 had used the same opt-in, but it did not authorize it for
SAMPLE-077. The current port removes this call. Native qualification injects actual SDL finger
events from an external test harness; browser qualification sends actual touch events from Chrome.
Neither test route adds input behavior to the sample.
