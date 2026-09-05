# Deliberate differences from the XNA 4.0 original

Three of them. Two are lines the original cannot need because C# supplies the mechanism through
reflection or garbage collection; one is an owner-approved input addition. None changes what the
game does.

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

## C++ mechanic: a screen deregisters the components it owns

`Game.Components` holds strong references in C#, so a `GameplayScreen` dropped before its
`UnloadContent` runs simply leaves live components registered — wasteful, harmless, and something
the original does routinely: `MainMenuScreen`'s saved-game branch rebuilds its loading screen on
every frame it is still transitioning off, and each of those builds a `GameplayScreen`.

CNA's collection holds raw pointers and this port owns its components by `shared_ptr`, so the same
sequence would leave freed pointers registered. `GameplayScreen` therefore remembers what it
registered and removes it again in its destructor:

```cpp
~GameplayScreen() override { RemoveOwnedComponents(); }
```

The original's own `UnloadContent()` — which removes every `RestorableStateComponent` from
`Game.Components` — is translated unchanged and still does the work on the ordinary path; the
destructor only covers the case C# does not have to think about. No component's lifetime,
draw order or update order changes.

The related framework half of this — CNA continuing to call a component that was removed while the
frame iterating it was still running — was fixed in `cnanext` rather than worked around here; see
[`missing.md`](missing.md).
