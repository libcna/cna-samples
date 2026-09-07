# SAMPLE-070 — deviations from the original

Every entry is a place where C++ cannot express what the C# does, or where the framework's shape
differs. Nothing here is a simplification of behaviour; where behaviour differs it says so.

## Reflection

C# resolves types by name at runtime three times over. C++ has no reflection, so each becomes an
explicit table:

1. **Content type readers.** Each `.xnb` names its reader by the assembly-qualified type name XNA
   resolves against `RolePlayingGameDataWindows`. `src/Data/RolePlayingGameDataContentReaders.cpp`
   says once which reader answers to each name; the readers themselves are the original's, one per
   original `*Reader` nested class. The registration call in the game constructor is marked
   `CNAEXT`.

2. **`XmlSerializer` members.** A save type opts in with `SHARP_XML_SERIALIZABLE`, listing the
   members in the original's order. The names, order and types are the original's, and the document
   written is the one the XNA 4.0 runtime writes — measured with a probe compiled against the real
   `RolePlayingGameDataWindows.dll` under the XNA 4.0 Wine prefix, not inferred. `.NET` generic
   instantiation names (`ContentEntryOfGear`, `WorldEntryOfChest`, …) are spelled out in
   `src/Data/XmlEntryNames.hpp`; `Point`, `Vector2` and `Direction` opt in by ADL in
   `src/Data/XmlSaveHooks.hpp`.

3. **Enum names.** `Keys.ToString()` and `Armor.ArmorSlot.ToString()` become the name tables
   `InputManager::KeyName` and `RolePlayingGameData::ArmorSlotToString`, the same approach
   `Spacewar`'s `Settings.cpp` uses. `Quest::QuestStage` goes through `SHARP_XML_ENUM`, so the
   save file carries the enumerator name as `Enum.Parse`/`ToString` do.

## Polymorphic content loads

Every gear reader targets `shared_ptr<Gear>`, the common base, because `std::any_cast` needs an
exact type match where C# only needs an assignable one. The narrowing C# performs at the
`Load<Equipment>` call site is written out at each call site instead (see `PartySaveRestore.hpp`
and `FightingCharacterReader`). This is CNA's established XNB-40 precedent.

## Cross-references

`Session`, `CombatEngine`, `Hud`, `TileEngine` and `Party` refer to each other in cycles that C#
resolves at link time and C++ cannot resolve in headers. The declarations stay with their class;
the bodies that close a cycle are gathered in `RolePlayingGame.hpp`, the same way every other
forward-reference cycle in this repository's ports is resolved. No logic moves — the statements
are the original's, in the original's order.

## Framework shape

- `StorageDevice::BeginShowSelector` completes synchronously in CNA, so the delegate the original
  hands to `Session::GetStorageDevice` runs before that call returns. The flow is otherwise the
  original's, callback and all.
- `ContentManager::Load<T>` returns `T` by value, so screens hold `Texture2D` by value where C#
  holds a reference. Data classes that must share one texture hold `shared_ptr<Texture2D>`, which
  is what the readers produce.
- C# `ReadOnlyCollection<T>` returns become `const std::vector<T>&` — a read-only view over the
  existing list, which is what the original hands back; neither copies.
- `GameScreen` and `MenuEntry` derive from `System::Object` so a screen can be the `sender` of its
  own events, which is what `Accepted(this, EventArgs.Empty)` requires.
- `ScreenManager::TraceScreens` writes to `stderr`. The original writes through
  `System.Diagnostics.Trace` under `#if WINDOWS`; that module is not in this sample's link
  closure, and a debug listener stream is what the line is for.

## Platform conditionals

The original's `#if XBOX` / `#if !XBOX` blocks keep only the Windows arm — this port has no Xbox
configuration, so the other arm does not exist. The `#if DEBUG` combat cheat key (right shoulder,
or `W`, ends combat in victory) is kept under `#ifndef NDEBUG`.

## Known behavioural difference

`Party::MonsterKills` is a `std::unordered_map`, where the original is a `Dictionary<string,int>`.
Lookups and counts are identical; only the order in which `PartySaveData` writes
`monsterKillNames`/`monsterKillCounts` can differ from the order .NET happens to enumerate. The
save reads back the same kills either way.
