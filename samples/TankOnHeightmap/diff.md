# SAMPLE-074 C# to C++ translation notes

The port preserves the original sample's behavior and source decomposition. The following are
only mechanical C#-to-C++ mappings.

| Original C# construct | C++ CNA representation |
|---|---|
| `Vector3 Position` and `float FacingDirection` read-only properties | `getPositionProperty()` and `getFacingDirectionProperty()` |
| Nullable reference fields for the two `Model` instances | `std::optional<Model>` until `LoadContent()` assigns them |
| `ModelBone` reference fields | Non-owning `ModelBone*` values owned by the loaded model |
| `float[,]` and `Vector3[,]` | Nested vectors indexed in the same `[x][z]` order |
| `out float` and `out Vector3` | Mutable C++ references |
| `foreach` over model meshes and effects | Range loops over CNA collections |
| C# `as HeightMapInfo` and typed `BasicEffect` enumeration | Checked `dynamic_cast`, retaining the original failure boundary |
| `using (game) { game.Run(); }` | Automatic C++ lifetime around the stack game object |

The original XNA runtime discovers `HeightMapInfoReader` through assembly reflection. CNA has no
reflection by design, so the constructor explicitly registers a creator for the exact
`TanksOnAHeightmap.HeightMapInfoReader` string stored in `terrain.xnb`. Because the deserialized
value is carried by `Model.Tag`, the CNA reader specialization returns
`std::shared_ptr<System::Object>` while constructing the same logical `HeightMapInfo` type and
reading the exact writer payload in the exact order.

The original pipeline itself was not reimplemented in sample runtime code. Its unchanged XNA 4.0
processor and writer produced the checked-in XNB files. There are no gameplay, rendering, input,
content or timing deviations.
