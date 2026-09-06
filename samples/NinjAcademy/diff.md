# Deliberate differences from the XNA 4.0 original

Three of them: one owner-approved input addition, and two lines the original cannot need — one
because C# obtains the metadata by reflection at run time, one because the phone shell drew the
Guide overlay that CNA hands back to the game. None changes what the game does.

## Owner-approved: desktop pointer input reaches the touch-only game

`NinjAcademy_4_0` is a Windows Phone game and reads nothing but `TouchPanel` gestures — `Tap` to
throw a shuriken or pick a menu entry, `FreeDrag` to sweep the sword. The owner approved the same
off-by-default opt-in the earlier touch-only samples use, so the game can be driven with a mouse on
desktop and in a browser:

```cpp
// NinjAcademyGame.hpp, in the constructor
CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);
```

What it does: CNA turns the desktop pointer into ordinary touch events, so they arrive through the
unchanged `TouchPanel` path. What it does **not** do: the game grows no mouse code path, no second
set of controls and no keyboard shortcuts of its own. `InputState` is the original's, gesture for
gesture, and the mechanism lives in `cnanext` and stays off unless a sample asks for it. Escape
pauses because the original's own `InputState.IsPauseGame` already maps `Keys.Escape` alongside
`Buttons.Back`.

## C++ mechanic: the reflection metadata C# obtains at run time

`Textures/Animations.xnb` and `Configuration/Configuration.xnb` are reflectively serialized, so XNA
resolves `ReflectiveReader<NinjAcademy.Animation>` and its siblings by walking the game assembly's
types at load time. C++ has no reflection, so the field lists are declared once, in one CNAEXT call
from the game's constructor:

```cpp
// NinjAcademyGame.hpp, in the constructor
NinjAcademyContentReaderRegistrationEXT::RegisterEXT();
```

`src/ContentReaders.cpp` states each type's serialized members in wire order and nothing else; the
reader machinery itself is `cnanext`'s `ReflectiveTypeReaderBuilder<T>`. Nothing is avoided and the
sample gains no behaviour: the same bytes are read from the same official `.xnb`, and the line
disappears the moment the language supplies the mechanism. This follows the precedent of
`samples/HeightmapCollision/diff.md` and `samples/CustomModelAnimation/diff.md`.

`ScreenManager::RegisterScreenType<T>()` is the same shape for the screen-stack serialization the
original's `ScreenManager` performs: XNA reconstructs a screen from the type name it wrote, using
`Type.GetType`.

## C++ mechanic: the game draws the Guide overlay the phone shell drew

On Windows Phone the Guide is a system dialog: the shell draws it over the running game and owns
the screen while it is up. `Guide` has no access to a game's own `GraphicsDevice` or `SpriteBatch`
on any platform CNA targets, so CNA renders its stand-in from an explicit entry point, and the game
calls it once at the end of its own `Draw`:

```cpp
// NinjAcademyGame.hpp, at the end of Draw()
CNAEXT Guide::RenderPendingKeyboardInputEXT(...);
CNAEXT Guide::RenderPendingMessageBoxEXT(...);
```

The other half of that ownership — the game not reading the taps the overlay is covering — is the
framework's and was fixed there rather than guarded here; see [`missing.md`](missing.md).

`GameplayScreen` also asks `Guide::WasKeyboardInputCanceledEXT` where the original tests
`playerName != null`, because `EndShowKeyboardInput` returns a `std::string`, which has no null.
That query is CNA's own documented answer to exactly this, not something this sample invented.

## What is deliberately *not* here

Two things the first pass of this port kept were removed rather than documented, because they
belonged in the framework:

- **A screen deregistering its own components.** `Game.Components` holds strong references in C#,
  so a `GameplayScreen` dropped before its `UnloadContent` runs leaves live components registered —
  wasteful, harmless, and something the original does routinely, since `MainMenuScreen`'s
  saved-game branch rebuilds its loading screen on every frame it is still transitioning off.
  Rather than have the sample track and unregister what it owns, `cnanext`'s
  `GameComponentCollection` gained the ownership-taking `Add(std::shared_ptr<IGameComponent>)`
  overload that restores XNA's own guarantee, and the port simply writes `components.Add(bamboo)`
  where the original writes `Components.Add(bamboo)`.
- **A vector holding the "Game Over" text alive.** Same cause, same fix: the original constructs
  that component inside the `Components.Add(...)` call and keeps no field, and so does the port.
