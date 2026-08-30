# MarbleMaze — intentional C++ language adaptations

The port has no owner-approved gameplay addition and no sample workaround. Three `CNAEXT`-marked
mechanisms exist only because the original C# runtime supplies reflection or managed identity that
closed-world C++ does not.

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
