# MarbleMaze — intentional differences from the XNA 4.0 original

The port has one owner-approved input accessibility addition. Three other
`CNAEXT`-marked mechanisms exist only because the original C# runtime supplies
reflection or managed identity that closed-world C++ does not. The former
Emscripten-only synchronous loading branch was removed on 2026-09-26 after the
original background-thread path passed current Chrome and Firefox WEBGL2 gates;
see `missing.md`. It is no longer an intentional difference.

## Mouse input is supported in addition to touch

**Requested by the project owner on 2026-09-05.** The original Windows Phone game operates its
screen stack through `TouchPanel` contacts and gestures. A desktop or browser without a touch
screen therefore renders the menus but cannot navigate them with a pointer.

The port enables one CNA extension in `MarbleMazeGame`:

```cpp
CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);
```

CNA maps the left mouse button to the same touch contact and gesture pipeline the unchanged screen
manager already consumes. No `Mouse::GetState()`, alternate menu handler, keyboard shortcut or
second input path is added. The extension is off by default framework-wide and is enabled only by
this marked sample call, so other applications are unaffected and real touch continues unchanged.

## Screen construction registry

The original `ScreenManager.DeserializeState()` stores each screen's assembly-qualified CLR type
and recreates it through `Type.GetType()` plus `Activator.CreateInstance()`. The C++ game registers
the six default-constructible screen types in `MarbleMazeGame` and stores/looks up their RTTI names
in an AOT factory map. The same screen classes are serialized and reconstructed; no screen or state
branch is substituted.

`CalibrationScreen` intentionally has no registration: like the original it requires a live
`GameplayScreen` constructor argument. If such a screen is encountered during restoration, the
factory failure follows the original method's catch/delete-state path.

## `Model.Tag` dictionary carrier

The custom XNA processor writes a CLR `Dictionary<string,List<Vector3>>` into `Model.Tag`. CNA's AOT
content reader preserves every dictionary entry and value in `CNA::Content::ObjectDictionaryEXT`.
`Maze::LoadContent()` performs one `CNAEXT`-marked cast to that carrier, then reads the original
`Floor`, `floorSides` and `walls` lists. Rendering, collisions, bones and checkpoints all continue
to use the authentic `Model`; no alternate mesh or collision representation is introduced.

## Managed runtime type names

Concrete `System::Object`-derived sample types provide CNA's required `CNAEXT GetTypeName()`
override (`MarbleMazeGame.MarbleMazeGame` and `GameStateManagement.MenuEntry`). This supplies the
logical CLR identity that C# obtains automatically and does not change game behavior.
