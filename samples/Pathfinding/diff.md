# Differences from the XNA 4.0 original

## The map content reader is registered explicitly

The original content project compiles `Map1.xml` through `Map4.xml` into XNBs whose
reader table names `ReflectiveReader<PathfindingData.MapData>` and
`ListReader<Point>`. XNA discovers and constructs the readers through .NET
reflection. C++ has no equivalent assembly reflection, so the port has one
additional line in the game's constructor:

```cpp
CNAEXT PathfindingData::ContentReaders::Register();
```

`ContentReaders.hpp` gives CNA's content manager the two closed reader names
recorded in those XNBs. Its `MapDataReader` reads the original type's five fields
in wire order: `NumberRows`, `NumberColumns`, `Start`, `End`, `Barriers`.
The first four value-type fields are inlined without a reader index; the barrier
list, a reference type, carries one. The port still calls
`Content.Load<MapData>("map1")` through `"map4"`; it does not load or parse XML at
runtime. All 13 checked-in XNBs are byte-identical to this sample's own
official XNA 4.0 pipeline output. Only the way the reader is discovered differs.

## Everything else

`GetTypeName()` is CNA-required runtime type naming and returns the unchanged
logical name `Pathfinding.Pathfinding`. The C# data-library type is represented
in the same namespace in C++, without a separate target for its header.
Other differences are lossless C#-to-C++ mechanics listed in `missing.md`.
There is no owner-approved feature addition or active behavioral deviation.
