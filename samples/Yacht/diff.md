# SAMPLE-071 — where the port differs from the original, and why

Everything here is a deliberate decision with a reason. Anything not listed is a
transcription.

## Upstream faults reproduced, not repaired

The rule is to port the sample, not to improve it, and a client written against a server
that scores a certain way must meet a server that scores that way.

| Where | What the original does |
|---|---|
| `YachtServices/BasicAIPlayer.hpp` | The server's AI rolls between **one and four** dice valued **one to five** — never five dice valued one to six. |
| `YachtServices/BasicAIPlayer.hpp` | Its `Sum` guards the whole test with `value.HasValue`, so a sum with nothing to match adds nothing: **Choice, four of a kind and full house always score zero** however the dice fall. |
| `YachtServices/DataModel.hpp` | `DiceState`, `GameState`, `Message` and `AvailableGames` write their **own wrapping element** from `WriteXml`, which .NET's `IXmlSerializable` contract says is the caller's job. |
| `YachtServices/YachtService.cpp` | The Message payload carries **two nested `Message` elements**, because the outer one is `XmlSerializer`'s root wrapper. Both ends depend on it; the capture in `evidence/soap-oracle/` is the proof. |
| `YachtServices/DataModel.hpp` | `Choise` — the enum member's own spelling. |

## No reflection: explicit tables where .NET looks a type up

Each of these is the same shape the content readers already take in this repository.

| Where | What replaces the lookup |
|---|---|
| `System.ServiceModel`'s `ServiceHost` | `Type.GetType`/`Activator.CreateInstance` become a table of operations the service registers; an unknown operation is faulted rather than ignored. |
| `ScreenManager::SerializeState` | The screen list is written by name and restored through a registered factory; a name with no factory is skipped, as .NET skips a type that will not resolve. |
| `GameScreen::GetTypeName` | A screen states its own type name, because C++ cannot ask an object what it is. |
| `Server/Program.cpp` | The host cannot read an operation's parameters: a wrapped SOAP call carries no type information, so which elements to expect is the contract's knowledge and lives with the contract. |

## Threading

| Where | Deviation |
|---|---|
| `YachtServices/YachtServiceClient.hpp`, `Misc/NetworkManager.hpp` | Windows Phone's generated proxy captures the caller's `SynchronizationContext` and raises completions back on it. CNA has no pump, so completions and push notifications are **queued** and `YachtGame::Update` drains both once per frame on the game's own thread. The effect is the original's: a handler never runs while a frame is half drawn. Chosen by the project owner over adding a pump to `Game`. |
| `Objects/Dice.hpp` | The die's face and its rolling flag are written by the roll timer's thread and read by the game's, so both are `std::atomic`. The original simply races; C++ would make that undefined rather than merely unpredictable, and the observable behaviour is identical. |

## Types that could not be transcribed

| Where | Deviation |
|---|---|
| `Misc/YachtEventArgs.hpp` | `ExceptionEventArgs` carries a `std::exception_ptr`, which cannot be read without rethrowing, so the message is offered as a property rather than making every call site write the same try/catch. |
| `YachtServices/YachtServiceClient.hpp` | The generated proxy declares ten `…CompletedEventArgs` classes differing only in the result type; they are aliases of one template here. The names callers use are the generated ones. |
| `YachtServices/DataModel.hpp` | `List<T>` is `std::vector`, and a `byte[]` that .NET can leave null is an **empty vector** — every emptiness test says so where it is made. |
| `YachtGame.hpp` | The original writes the saved game through `XmlWriter.Create(fileStream)`; this runtime's writer builds its document in memory, so the text is written to the stream instead. The file is identical. |

## Platform seams: one line each, where the phone's operating system acted

| Where | Why |
|---|---|
| `YachtGame::Initialize` | `TouchPanel::setMouseTouchEmulationEnabledEXT` — the game is all touch and a desktop has no touch screen. |
| `YachtGame::Draw` | `Guide::RenderPendingMessageBoxEXT` / `RenderPendingKeyboardInputEXT` — the phone's shell drew these; CNA has no shell above the game. |
| `YachtGame`'s constructor | `PhoneApplicationService::AttachEXT` — on the phone the operating system owned the service. Attached last, because attaching reports the fresh start and a handler added afterwards would miss it. |

## Framework behaviour this port does NOT compensate for

| Where | What differs, and why it stays |
| --- | --- |
| `GraphicsDevice.Viewport` under EasyGL | With the window shaped differently from the backbuffer, CNA reports a viewport widened to the window's aspect (measured in the browser: `1067x800` for a `480x800` backbuffer), where XNA and FNA report the backbuffer. This game anchors its leaderboard and buttons to `Viewport.Width`, so they move. The behaviour is a deliberate CNA extension with a public opt-out, `GraphicsDeviceManager::setPreferredPresentationModeProperty`; calling it here would be a sample-side workaround for a framework default, and changing the default is the project owner's call. Recorded in `missing.md`, not papered over. |

## Structure

| Where | Deviation |
|---|---|
| Several `*Bodies.hpp` | The original's classes reference each other freely; C++ needs the cycle broken, so declarations stay with their class and bodies close in a bodies header once both sides are complete. |
| `Objects/Dice.hpp`, `Misc/AudioManager.hpp` | A `static` member of a class's own type is declared in the class and defined after it, because the deleter would otherwise be instantiated on an incomplete type. |
| `YachtServices/DataModel.hpp` | The server's working members sit behind `YACHT_SERVER`, which is this port's spelling of the original's `#if !WINDOWS_PHONE`. |

## Not ported, and why

| What | Why |
|---|---|
| `MenuScreen`'s `WINDOWS` and `XBOX` input branches | The file serves three products behind `#if`; Yacht is the phone one and compiles the third. Porting the others would give this game keyboard and mouse menu navigation it does not have. |
| The online half in the browser | Emscripten cannot open a raw socket. See `missing.md`. |
| MPNS itself | Retired. The channel is the endpoint here rather than a relay to one; `HttpNotificationChannel`'s header records what that costs. |
