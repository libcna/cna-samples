# SAMPLE-063 — intentional C++ language adaptations

This file records only adaptations required by C++ mechanics. They are not substitutes for missing
XNA behavior.

## Screen type reconstruction

XNA writes each serializable screen's CLR type name and reconstructs it with reflection. Reflection
is intentionally unavailable in SharpRuntime. `ScreenManager::RegisterScreenType<T>()` is a
compile-time factory registry for the four default-constructible serializable screen types. Missing
registrations fail explicitly. Parameterized screens are not registered, matching the fact that
the original `Activator.CreateInstance` route also requires a default constructor.

## Deterministic destruction boundary

The original removes a screen reference while CLR garbage collection determines later object
destruction. C++ `shared_ptr` would otherwise destroy a removed `GameplayScreen` and its many
components synchronously while `Game` still holds the current frame's component snapshot.
`pendingDestruction_` keeps removed screens alive until the start of the next update, after the
previous update/draw iteration has finished. This reproduces the original lifetime boundary and
prevents dangling component pointers; it does not change screen order or transitions.

## Async object lifetime

The original keyboard callback and loading threads naturally retain managed objects. Their C++
closures capture `shared_ptr` ownership until completion. The Guide callback deletes its heap
`IAsyncResult` after `EndShowKeyboardInput`, following the current SharpRuntime callback ownership
contract.

`BeeKeeper::BeginHoneyDeposit` returns a completed `DepositAsyncResult` because C++ cannot represent
the original call's nullable delegate with the reference-only callback type. The gameplay call never
supplies a callback and `EndHoneyDeposit` intentionally ignores the result in both languages, so
observable behavior is unchanged.

## Header colocation

Several component methods are defined after `GameplayScreen` in `GameplayScreen.hpp` so they can
use the complete owning type while preserving the original circular relationships. Public classes,
names and behavior remain separate.

## Song deployment companion

The authentic SongProcessor XNB/WMA pairs remain the content contract. Ogg-FLAC companions are
deployed for CNA's portable native/browser decoder; game code still loads the original Song XNBs.
Both companions retain the WMA streams' 48 kHz stereo PCM without resampling. Decoding each WMA
and companion to signed 16-bit PCM yields the same byte count and SHA-256; the exact values and
file hashes are in the current evidence's `audio-verification.json`.

## Mouse-to-touch opt-in

The original Windows Phone game is touch-only. For normal desktop and browser use, the port enables
the owner-approved off-by-default CNA extension:

```cpp
CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true);
```

While enabled, the left mouse button enters the existing `TouchPanel` state and gesture pipeline.
No menu, gameplay, keyboard or sample-local synthetic-input path was added.

## Background asset loading

Both `GameplayScreen::LoadAssets()` calls run on background `System.Threading.Thread` instances,
including in the threaded WEBGL2 build, as in the original Phone sample. The former Emscripten-only
synchronous calls and completion flags were removed. CNA EasyGL's general renderer-thread context
lease supports this path; it needs no sample-local graphics exception.
