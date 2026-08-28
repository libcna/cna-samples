# Differences from the XNA 4.0 original

## The sample's own ContentTypeReader is registered by name instead of found by reflection

Not a behavioural deviation, and not an owner-approved addition — a language difference that has to
be spelled somewhere, recorded here because it is the one line of this port that the original does
not have.

`TerrainProcessor` attaches a `HeightMapInfoContent` to the terrain model's `Tag`, and the sample's
own `ContentTypeWriter` records in the `.xnb` which runtime reader answers for it:

```csharp
public override string GetRuntimeReader(TargetPlatform targetPlatform)
{
    return "HeightmapCollision.HeightMapInfoReader, " +
        "HeightmapCollision, Version=1.0.0.0, Culture=neutral";
}
```

XNA resolves that name by **reflecting over the game assembly** at load time and instantiating the
type it finds. C++ has no reflection, so nothing can look a class up by its name. The port therefore
tells the ContentManager which reader answers to that name, in one line in the game's constructor:

```cpp
CNAEXT ContentTypeReaderManager::AddTypeCreator(
    "HeightmapCollision.HeightMapInfoReader",
    [] { return std::make_unique<HeightMapInfoReader>(); });
```

What this deliberately is **not**:

- It is **not a different reader.** `HeightMapInfoReader::Read` is a statement-for-statement
  translation of the original's: terrain scale, width, height, then every height in the same order
  the writer's `foreach` over a `float[width, height]` produced. Only the way it is *found* differs.
- It is **not a name this port chose.** The string is the one the `.xnb` itself records, with the
  assembly qualifiers stripped exactly as CNA normalises every reader name.
- It is **not a change to CNA.** `ContentTypeReaderManager::AddTypeCreator` is the existing public
  hook for a game-supplied reader; this is the first sample in the campaign to need it, and it
  needed nothing added.

## Everything else

No other difference from the original. `missing.md` holds the complete audit, including the
measurement that the collision values themselves — `IsOnHeightmap` and `GetHeight` over an 81-point
grid — are **bit-identical between the two engines, 0 ULP**.
