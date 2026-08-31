# Necessary C#-to-C++ mechanics

SAMPLE-069 has no owner-requested behavioral deviation. The following lines exist only because
C++ does not provide C# garbage collection, universal reference identity or runtime reflection.

## Object ownership and identity

- `CardsGame` owns dynamically-added game components with `shared_ptr`, while CNA's XNA-shaped
  `GameComponentCollection` retains the same non-owning component pointers exposed by XNA.
  Releases are deferred until the next update boundary so a component is never destroyed while a
  collection snapshot is executing. This replaces C# GC reachability without changing component
  add/remove order or callbacks.
- `CardPacket` and `Hand` move `unique_ptr<TraditionalCard>` objects. The address of a card therefore
  remains stable as it moves through the deck and hands, matching C# reference identity and the
  original reference-equality lookups.
- Textures created by repeated end-game transitions and the singleton `AudioManager` have explicit
  C++ owners and deterministic teardown. They live through the same observable use interval as
  their garbage-collected C# counterparts.

## Reflection substitute

The original screen-stack serializer writes each screen's assembly-qualified type name and
constructs it through .NET reflection. C++ has no equivalent runtime facility, so the constructor
contains four `CNAEXT ScreenManager::RegisterScreenType<T>()` calls. Serialization writes the
compiler type identity and deserialization invokes the matching registered factory. The original
screen list and per-screen payload order are unchanged; the registration is the only closed-AOT
substitute.

Concrete `System::Object` and `GameComponent` descendants also expose CNAEXT `GetTypeName()`
overrides required by CNA's C++ runtime type convention. These methods do not participate in game
logic.

## C++ name lookup

C# permits a property to have the same identifier as a visible type. C++ does not permit the same
surface reliably in these class scopes, so `Player.Hand` is represented as `PlayerHand` and
`CardsGame.Game` as `GameInstance`. Every original read/write maps directly to the renamed member;
there is no behavioral branch attached to either spelling.
