<!-- SPDX-License-Identifier: MIT -->

# Handoff: `SAMPLES-DEC-008` is unblocked — XML serialization exists in Sharp Runtime

**Left here 2026-09-01 by the `cna-cs` session, at the owner's request.**

Left as a standalone file on purpose: `NEXT.md` and `plan.md` were both modified in your working
tree when this was written, and overwriting your in-flight edits was not worth the risk. Fold
this into them yourself, or delete it once read.

## The ask

**After you finish the sample you are on, and before starting further samples**, take this as an
intermediate step:

1. Look at the new XML serialization in `sharp-runtime` and fix whatever needs fixing once it
   meets a real port.
2. Revisit the samples that were blocked purely by its absence — `SAMPLE-014` (Spacewar),
   `SAMPLE-066` (ShipGame), `SAMPLE-070` (RolePlayingGame) — and take them as far as the
   serializer now allows.

## What landed

`sharp-runtime`, branch **`next`**, at **`bd282d10`** (pushed). New module
`modules/xml-serialization` — component `Xml.Serialization`, header-only `INTERFACE`, public
dependencies `Core.Base` and `Xml`.

Reflection stays a permanent deviation, so a type opts in explicitly:

```cpp
struct Entity {
    std::string name;
    Matrix transform;
    SHARP_XML_SERIALIZABLE(Entity, "Entity",
                            SHARP_XML_M(Entity, name),
                            SHARP_XML_M(Entity, transform))
};

std::string xml = XmlSerializer<EntityList>{}.Serialize(list);
EntityList back = XmlSerializer<EntityList>{}.Deserialize(xml);
```

Same customization-point shape `JsonSerializer` already uses through `nlohmann`'s ADL hooks.
There is no vendored mapper underneath — `tinyxml2` is a parser — so the traversal, naming,
ordering, collections and text conversion are the module's own.

**Read `sharp-runtime/docs/XmlSerializationScope.md` first.** It has the capability inventory
with per-item evidence, four out-of-scope areas with the greps proving they are unreachable, and
seven recorded deviations.

## What it covers, and why each is there

Composite types in registered order · value-type flattening (`Matrix` → 16 `<M11>`…`<M44>`) ·
`List<T>` fields · root-level `List<T>` → `<ArrayOfT>` · `List<primitive>` with XSD item names
(`<string>`, `<int>`, `<boolean>`…) · enums as member **names** · inherited members · generic
instantiations (`WorldEntry<Chest>` registers as `"WorldEntryOfChest"`, so a list of it is
`<ArrayOfWorldEntryOfChest>`) · missing element leaves the member at its default, unknown
element ignored (both .NET's own behaviour) · whitespace-insensitive reading · markup escaping.

**The one worth knowing about before you start:** sixteen of the twenty `XmlSerializer` call
sites in `RolePlayingGame/Session/Session.cs` serialize into an **already open `XmlWriter`**,
nesting several objects into one `<rolePlayingGameSaveData>` document rather than each writing
its own. `SerializeInto(doc, parent, value)`, `DeserializeFrom(element)` and `RootElementName()`
cover that shape. `modules/xml-serialization/tests/.../RolePlayingGameSaveTests.cpp` builds that
exact document and reads it back the way `Session.Load` does — start from that file.

## Deliberately out of scope, by evidence

`[XmlInclude]`/`xsi:type` polymorphism (zero hits across ShipGame and RolePlayingGame — the
`Character → FightingCharacter → Monster` chain belongs to the **Content Pipeline** writers, not
to `XmlSerializer`) · `[XmlArray]`/`[XmlArrayItem]` overrides · `[XmlAttribute]`-mapped members ·
circular-reference detection. If a sample needs one of these, it is new work with its own
ticket, not a silent gap.

## Deviations you may hit

1. `XmlConvert::ToString(float)` emits a lowercase `e` where XSD, .NET and every authentic
   fixture use `E`. Corrected inside this module; the underlying `Core`/`Xml` behaviour is
   untouched and pinned by `DoubleTests.cpp:643`. Probably deserves its own ticket.
2. A **whitespace-only** string is dropped by the parser (not the serializer) — tinyxml2
   discards an all-whitespace text node. Nothing in the three samples stores one; whitespace
   *around* real content is fine.
3. Significant-digit count differs from .NET Framework 4.0 (`3.4028235E+38` vs
   `3.40282347E+38`). Both parse to the identical float.
4. A bare `&` is accepted as literal text where .NET rejects the document. Safe direction for a
   loader; the writer always escapes.
5. The consumer surface does not build under `-Wpedantic -Werror`, because `XmlSerializer.hpp`
   reaches `XmlConvert.hpp` → `Decimal.hpp` → `__int128`. **Pre-existing**: a fixture including
   only `System/Xml/XmlConvert.hpp` under component `Xml` fails identically.

## How far it is actually verified — and how far it is not

Verified against **Microsoft's own output**, not against this repository's reading of the format.
The XNA Game Studio tree ships files the games load at runtime, and those are the corpus:
ShipGame's `level1_spawns.xml`, `level1_lights.xml`, `level{1,2}_powerups.xml`,
`ship1.xml`/`ship2.xml`, and Spacewar's 184-line `settings.xml` — including its `ShipLighting[]`
array of five-`Vector4` structs, its `Keys` enumerators by name, and its `<X>.4</X>` floats with
no leading zero. Tests read them from `XNA_SAMPLES_ROOT` (default `/rv/tmp/XNAGameStudio/Samples`)
and skip if absent.

State: **17,934 tests across 39 executables, 0 failed**; clean under ASAN+UBSAN with leak
detection; 11/11 gate scripts OK.

**What is not verified:** no sample is ported against it. The types in the tests are
transcriptions written from the C# sources, not the real ported classes, so a real port may hit a
shape that was not modelled. Nothing was ever generated by real .NET and diffed byte-for-byte —
the comparison is against shipped fixtures, which is good but not the same thing. Treat the
engine as a solid starting point, not as proof the ports will be clean.

## Where to look

- `sharp-runtime/docs/XmlSerializationScope.md` — scope, evidence, deviations
- `sharp-runtime/modules/xml-serialization/` — the module and its five test files
- `sharp-runtime/test/consumer/xml_serialization.cpp` — standalone consumer usage
- `sharp-runtime` branch `next` @ `bd282d10`
