# SAMPLE-066 — deliberate differences from the original

## Content directory spelling (`CONTENT_NAME content`)

The original's content directory is `Content\` on disk. The game reaches it two ways:
`Content.RootDirectory = "Content"` for every `Content.Load<T>()`, and literal lowercase
`"content/…"` `System.IO` paths in `EntityList.Load`, `LightList.Load`, `AudioEngine`, `WaveBank`
and `SoundBank`. NTFS cannot tell the two apart; a case-sensitive filesystem can.

The deployed directory is therefore named `content`, through `cna_add_sample`'s new optional
`CONTENT_NAME` argument (default `Content`, so no other sample changes). Nothing is bypassed:
every load still goes through the same public API it does upstream, and CNA's `ContentManager`
resolves the `Content` spelling against it, which it already did for every asset path.

## C++ mechanics with no behavioural effect

- `DrawMode` belongs to `AnimSpriteManager.cs`, which needs the enumerations declared in
  `GameManager.cs`; the two cannot both be definitions in one include order. `GameManager.hpp`
  declares `enum class DrawMode : int;` opaquely — legal because a scoped enumeration has a fixed
  underlying type, and identical to the definition in `Graphics/AnimSpriteManager.hpp`.
- `LinkedList<T>` / `LinkedListNode<T>` become `std::list` plus a vector of iterators, preserving
  the upstream two-pass delete (collect nodes while iterating, erase afterwards).
- A `ParticleSystem` that a projectile's trail or a ship's engine steers is held by
  `std::shared_ptr`, because the manager can drop it from its own list while the other object
  still holds it — which is what the C# reference was, the garbage collector keeping it alive.
- `PlayerShip` holds its ship `Model` by value: `Content.Load<Model>` hands back a copy that owns
  its resources, and nothing else in the game keeps a per-player ship model alive.
- A C# `null` `String` becomes an empty `std::string` (`GameManager::SetShips`, `SetLevel`,
  `GetPlayerShip`), because `String` maps to `std::string`, which is not nullable.
- `Projectile::SetExplosion`'s nullable `String sound` becomes `std::optional<std::string>` — the
  blaster genuinely passes `null` there and the missile a name, so the distinction is load-bearing.

## Upstream defects reproduced, not fixed

- `ScreenManager.ScreenHelp` is declared to return `ScreenIntro`, which `ScreenHelp` is not. The
  property is dead code upstream and would throw `InvalidCastException` if it were ever called.
  The port keeps the property and the cast, as a checked `dynamic_cast` that yields null rather
  than undefined behaviour.
- `BlurManager`'s constructor swaps its `sizex`/`sizey` arguments. Every caller passes a square
  buffer, so it never shows. Reproduced.
- `GameplayScreen`-style misspellings in identifiers and comments (`SubCreateBambooSliceComponets`
  has no counterpart here, but `BlurGlowRenterTarget`, `oposide`, `cerate`, `poasticles`,
  `displose`, `deaw`, `Renturns`, `IsTriggerPressedRigth` do) are preserved verbatim.
