# SAMPLE-106 — `SavingEmbeddedImages_4_0` audit and owner decision

## Status

Re-analyzed against current heads on 2026-10-03 (next section). The row remains `🛑` owner decision
pending. No sample source, CMake target, partial game, browser-download substitute,
virtual-filesystem-only save, sample-local Guide overlay, invented filename UI, CNA change or Sharp
Runtime change was added. Only the owner may select a non-port or an implementation/modernization
boundary. The 2026-09-01 audit below is kept as history; its Guide findings are superseded where the
re-analysis says so.

## Current-head re-analysis — 2026-10-03

### Heads and retained evidence

| Repository | Branch | HEAD | State |
|---|---|---|---|
| `cna-samples` | `develop` | `e7723c0f893b` | clean, equal to `origin/develop` |
| `../cna` | `next` | `fc64a4be3339` | clean, equal to `origin/next` (276 commits after the handoff's `b1ea16aeb`, including the Gamer Services merge) |
| `../sharp-runtime` | `next` | `db86514c5bb8` | clean, equal to its upstream (no longer on `feature/gamer-services-collections`) |
| `../samples.libcna.com` | `main` | `8e48825c0033` | unchanged |

Artifact root: `/rv/tmp/samples/SAMPLE-106-SavingEmbeddedImages_4_0/`, new evidence under
`evidence/current-head-analysis-20261003/`. The retained `xna4-original/` is still byte-identical
to the physical upstream directory (all 16 files; hashes in `xna4-original.sha256`), and the four
official Phone/Reach XNBs in `win7-export/Content/` still match the hashes listed below.

The unchanged original still cannot be run: no Windows Phone SDK, reference assemblies or
application host/emulator exists on this host, and the Win7 VM's XNA installation was already
measured to lack the Windows Phone project extension. The VM was not booted for this re-analysis;
nothing suggests the installation changed. The authority for behavior remains the source, the
Phone documentation and the official content. Content generation success is not a runtime
comparison.

### What changed in CNA since the 2026-09-01 audit

CNA now has a real system Guide: a CNA-drawn overlay with its own keyboard, mouse and gamepad
input, on-screen keyboard and message boxes. `Game` draws it after the game's `Draw` every frame,
and `EndShowMessageBox`/`EndShowKeyboardInput` now **wait** for the answer as XNA documents, by
running modal frames (input plus overlay over a cleared screen, the game's own Update/Draw frozen):
CNA `modules/gamer-services/src/Xna/Guide.cpp:400` (`PrepareForEnd`, task GS-005i) and
`modules/runtime/src/Game.cpp:336` (`ModalFrames::runModalFrame`). The 2026-09-01 statement that
`EndShowMessageBox` always throws while pending and that only caller-owned `RenderPending*EXT`
rendering exists is therefore superseded **for games whose Guide overlay is installed**.

### Remaining gaps for this unchanged source

**G1 — the Phone Guide contract (native and browser).** The overlay is installed only by
`GamerServicesDispatcher::Initialize` (`modules/gamer-services/src/Xna/GamerServicesDispatcher.cpp:47`),
which a game reaches through `GamerServicesComponent`. XNA documents `GamerServicesComponent` and
`GamerServicesDispatcher` for Xbox 360 and Windows only; Windows Phone has neither, its Guide is
owned by the phone shell, and this sample (like every Phone sample) adds no component. In CNA, a
pending dialog is drawn **and answered** only inside the overlay's draw
(`modules/gamer-services/src/Internal/Guide/GuideScreens.cpp:809`) or inside an explicit
`CNAEXT Guide::RenderPending*EXT` call (`Guide.cpp:718`, `Guide.cpp:803`). By code inspection, the
unchanged translation would therefore:

1. open an invisible keyboard prompt on Tap, while CNA correctly withholds touch from the game; only
   a blind hardware Enter could answer it;
2. after the save, call `BeginShowMessageBox` then immediately `EndShowMessageBox`; the modal frames
   draw only the installed overlay, which is absent, so nothing presents or answers the box and the
   native wait cannot end until the window is closed, after which `End` throws
   `InvalidOperationException`.

Precedent: the completed Phone ports NinjAcademy (SAMPLE-065) and Yacht (SAMPLE-071) end their
`Draw` with a documented `CNAEXT Guide::RenderPending*EXT` line (`samples/NinjAcademy/diff.md`,
"the game draws the Guide overlay the phone shell drew"). That line would not rescue this sample:
modal frames never call the game's `Draw`, so the immediately blocking `EndShowMessageBox` still
has no drawer. Separately, MarbleMaze (SAMPLE-061) and HoneycombRush (SAMPLE-063) call
`BeginShowKeyboardInput` with neither a component nor that line; by the same inspection their
high-score name prompt is not drawn at this CNA head. Those rows were not reopened; the observation
is reported for the owner.

**G2 — blocking `End*` in the browser.** `runModalFrame` returns `false` under `__EMSCRIPTEN__`
(`Game.cpp:338`), so even with an overlay `EndShowMessageBox` throws "cannot wait for the answer"
at once. This is no longer a missing platform capability: every sample WEBGL2 executable already
links `-sASYNCIFY=1` (`cmake/SampleHelpers.cmake`), and `Game::RunLoop` already suspends its stack
on `requestAnimationFrame` between frames (`Game.cpp:1362`). A modal frame in such a blocking-`Run`
executable could suspend the same way. It must not do so in the Asyncify-free C API library, so it
is a scoped runtime change and needs its own qualification.

**M1 — browser media-library destination (unchanged).** SDL3's Emscripten backend still supports
only the home folder (`third_party/SDL/src/filesystem/emscripten/SDL_sysfilesystem.c:85`), so
`MediaLibraryPaths::GetPictureRoot` is empty and `MediaLibrary::SavePicture` throws
`System::IO::IOException` (`modules/media/src/Xna/MediaLibrary.cpp:586`). A transient MEMFS file,
a persistent IDBFS "Pictures" root, a download or a file picker are each a different user-visible
contract and remain a product decision.

**M2 — the failure exception.** The original catches only `InvalidOperationException` (its
documented failure: the phone is tethered with Zune running) and then shows "Unable to save
image.". CNA's failure is an `IOException`, which the unchanged code does not catch, so in the
browser the exception escapes the keyboard callback instead of reaching the original's own failure
dialog. The XNA exception type is supported by the sample's code and comment but could not be
checked against a Windows Phone `Microsoft.Xna.Framework` assembly, because none exists locally.
If confirmed and fixed generally, a browser build without a library would show the original's
failure branch; it still would not demonstrate a successful save.

**Native media (no gap found).** `SavePicture` writes a real JPEG under the platform Pictures folder
in `Saved Pictures` and updates the `Pictures`/`SavedPictures` collections. `Texture2D::FromStream`,
`SaveAsJpeg`, `TitleContainer`, Phone XNB platform `m`, 30 Hz timing, Tap gestures, the
owner-required `TouchPanel::setMouseTouchEmulationEnabledEXT(true)` opt-in and Escape as the Phone
Back button are all established by completed Phone ports.

### Fresh native gate at current heads

`scripts/build-current-head-analysis-20261003.sh` builds CNA's focused test executables in
`cna-native-opengles3-analysis/` (Release, OPENGLES3, shared ccache; 10:55 wall, exit 0, 123
inherited compiler warnings, no source changed). `scripts/run-current-head-analysis-20261003.sh`
runs them from CNA's cwd through `tools/platform/run_gpu_tests_private.sh`:

| Executable | Filter | Result |
|---|---|---|
| `CnaContentTests` | `XnbHeaderTest.*` | 11/11 |
| `CnaGraphicsTests` | `Texture2DFromStreamFormatTest.*:SaveAsJpegTest.*` | 21/21 |
| `CnaMediaTests` | `MediaLibrarySavePictureTest.*`, `…NoPreexistingRootTest.*`, `SavedPictureStoreTest.*`, `MediaLibraryPathsTest.*` | 16/16 |
| `CnaGamerServicesTests` | `GuideTest.*`, `GuideVisibilityTest.*`, `GuideInputTest.*`, `GuideAlreadyVisibleExceptionTest.*`, `SystemGuideTest.*` | 75/75 |

**123/123 passed, none skipped**; the CNA checkout was clean before and after. These include
`GuideTest.EndWaitsForTheAnswerWhileTheGamesModalFramesRun`,
`GuideTest.EndWithoutARunningGameRefusesAndCanBeCalledAgainOnceAnswered` and
`GuideVisibilityTest.IsVisibleRequiresInitializedGamerServices`. The last asserts the
Xbox/Windows rule that `Guide.IsVisible` throws before gamer services are initialized. Windows
Phone has no such initialization, so it is a further, minor item for (a) below; this sample never
reads `IsVisible`. The tests prove that the native pieces work in isolation and that the modal wait
works with the overlay present. They do not prove G1/G2 either way: those findings come from code
inspection of the owning sources cited above, because no sample translation or diagnostic game was
written. Logs: `evidence/current-head-analysis-20261003/{content,graphics,media,guide,build}.log`
and `test-results.txt`.

The analysis tree holds 653 MiB of intermediates. It can be removed at any time; the build script
regenerates it.

### Options for the owner (updated)

1. **⛔ Cancel** this Windows Phone media-library lesson as an evidence-backed non-port.
2. **Authorize general work, then a complete port:** (a) CNA presents Guide dialogs without
   `GamerServicesComponent`, as the Phone shell does, and the existing `RenderPending*EXT` lines in
   SAMPLE-065/071 are reconciled so that nothing draws or reads input twice; (b) Asyncify modal
   frames for blocking-`Run` browser executables; (c) a defined browser media-save contract
   (for example a persistent IDBFS Pictures root with collection readback, optionally plus a
   download so the user actually receives the JPEG), recorded in `diff.md` where it is not XNA
   parity; (d) the failure exception aligned with XNA once it is confirmed. Then a faithful port with
   native and real-Chrome gates and a gallery entry.
3. **Narrower:** do (a) and port natively (OPENGLES3) only, with the browser either deferred or,
   after (b) and (d), shipping only the original's own failure branch, marked `🟡` partial.

(a) and (b) are runtime fixes that also apply beyond this row (the SAMPLE-061/063 observation),
not sample workarounds. Each one touches the Gamer Services/runtime area the owner manages, so
none was started without a decision.

## Classification and complete source inventory (2026-09-01 audit, still accurate)

The physical upstream directory is one Windows Phone 7/Reach XNA game with one content project.
The audit covers all 16 files: solution/project metadata, both manifests, four packaging images,
the game-project JPEG, three content-project images/fonts, both SpriteFont declarations, the HTML
documentation and all 334 checked-in C# lines (293 runtime plus 41 assembly metadata lines).

The product is not merely a two-image rendering screen. Its documented purpose is to transfer two
different embedded-image representations into the phone's user-visible media library and exercise
the phone Guide UI around that operation:

- it runs fullscreen at 480x800 and 30 Hz, enables only `GestureType.Tap`, and exits through the
  phone Back button;
- it loads `GameProjectImage.jpg` through `TitleContainer.OpenStream` and
  `Texture2D.FromStream`, while loading `ContentProjectImage` through
  `ContentManager.Load<Texture2D>`;
- tapping either 200x333 image starts `Guide.BeginShowKeyboardInput` with a distinct default name;
- the game-project route reopens the original JPEG stream and passes it directly to
  `MediaLibrary.SavePicture`;
- the content-project route encodes the loaded texture through `Texture2D.SaveAsJpeg`, rewinds its
  `MemoryStream`, and passes that stream to `MediaLibrary.SavePicture`;
- success or `InvalidOperationException` produces the corresponding `Guide` message box, after
  which input is re-enabled.

The manifest explicitly requests `ID_CAP_MEDIALIB`. Microsoft XNA documentation classifies
`MediaLibrary.SavePicture` as Windows Phone-only, requires JPEG input, and promises that the image
is saved to the media library and returned as a `Picture` object. A hidden sandbox file or a local
echo of the bytes is therefore not the demonstrated result.

## Original build and content evidence (2026-09-01)

Artifact root: `/rv/tmp/samples/SAMPLE-106-SavingEmbeddedImages_4_0/`.

- `xna4-original/` is the complete byte-for-byte upstream snapshot.
- The owner-provided Windows 7 SP1 32-bit VM was run headless with all eight virtual network
  adapters set to `none`; no guest browser or internet access was enabled.
- The unchanged Phone/Reach content project builds successfully through XNA Game Studio's official
  pipeline. It produces four uncompressed XNB version-5 files with Windows Phone platform byte
  `m`: `ContentProjectImage` and `GameBackground` are 480x800 `Texture2D` assets, and `mainFont`
  and `detailFont` are the exact Segoe UI Bold SpriteFonts.
- Output SHA-256 values are:
  - `mainFont.xnb`: `db678b2838d4e1d3bf4be50d04efdbf0ff2d13097d50af3b79e3caa886b01c16`
  - `detailFont.xnb`: `489f27f114a9645b3f7b64584d4efd379667383a66a4db6ffa94e278484bd441`
  - `ContentProjectImage.xnb`: `8b6616ff235b22460fbf9a34be9e70d2cb09865e8acc157585c5c85d6f0912c4`
  - `GameBackground.xnb`: `e6f3ca9dbc0fe5291db2777b98fdffd9efc932da79c012d643bfabaafa2395d2`
- Live CNA's unchanged XNB importer successfully reads and converts all four authoritative files:
  two `Texture2D` and two `SpriteFont` assets. Content is not the blocker.
- The complete unchanged solution stops before C# compilation at
  `Microsoft.Xna.GameStudio.targets(34,5)`: this VM's XNA installation lacks the Windows Phone XNA
  project extension. That local SDK absence prevents an authentic game run, but is separate from
  both the successful official content build and the measured live CNA platform boundary below.

Exact commands, MSBuild logs, hashes and the CNA content-reader transcript are retained under
`scripts/`, `evidence/` and `win7-export/Content/`.

## Live CNA capability audit (2026-09-01, historical; Guide findings superseded above)

The portable/native pieces are already real rather than stubs:

- `Texture2D::FromStream` decodes JPEG and `Texture2D::SaveAsJpeg` encodes a real JPEG while
  retaining CPU pixel data from the content path;
- `MediaLibrary::SavePicture` writes the stream into the selected platform's Pictures root,
  creates a real `Saved Pictures` album and updates the `Pictures`/`SavedPictures` collections;
- Phone XNB platform byte `m` is accepted and both exact textures/fonts pass CNA's importer;
- `TitleContainer`, touch gestures, Back input, fullscreen and 30 Hz timing already exist.

The focused live OPENGLES3 test gate passes 77/77 tests covering XNB headers, JPEG stream
decode/encode, `SavedPictureStore`, `MediaLibrary.SavePicture` buffer/stream paths, and the complete
current `Guide` suite. This proves a native file-backed implementation exists; it does not turn the
browser sandbox into a phone media library.

Two concrete fidelity gaps still block a normal source translation:

1. **WEBGL2 has no media-library destination.** `MediaLibraryPaths::GetPictureRoot` asks the active
   platform for `UserFolder::Pictures`. SDL3's live Emscripten backend explicitly supports only
   `SDL_FOLDER_HOME` and returns null for Pictures. The root is therefore empty,
   `SavedPictureStore::SavePicture` returns failure, and `MediaLibrary::SavePicture` throws
   `System::IO::IOException`. A browser download, OPFS/IDBFS file, File System Access picker or
   native Pictures directory are materially different contracts and need an explicit product and
   persistence/permission decision.

2. **The current Guide interaction cannot run unchanged.** The original source relies on the
   platform-owned on-screen keyboard/message box. In particular it deliberately calls
   `EndShowMessageBox` immediately after `BeginShowMessageBox`; XNA's local documentation specifies
   that `EndShowMessageBox` blocks until the display operation finishes. Live CNA instead throws
   `InvalidOperationException` until caller code explicitly renders and completes the
   `CNAEXT` `RenderPendingMessageBoxEXT` route. The same manual-render extension owns the visible
   keyboard overlay. No `Game` or `GamerServicesComponent` hook renders either overlay
   automatically. Adding these calls and overlay assets only to this sample would be exactly the
   sample-local runtime workaround forbidden by `rules.md`; fixing it generally requires a
   platform-owned modal/async Guide design that remains viable in the externally driven browser
   event loop.

## Why no partial port was added (2026-09-01)

A C++ screen that merely draws the exact assets would omit both demonstrated save routes. Writing
inside Emscripten's transient virtual filesystem would claim success without giving the user a
picture. Forcing an `<a download>` changes library identity, collection/readback and persistence
semantics. Calling `RenderPending*EXT` from this one game's `Draw` would change the original source
contract and make the port depend on CNA-only UI. None is a faithful translation.

The native implementation being feasible does not satisfy this campaign's mandatory real-browser
gate. Implementing a browser media-library/download abstraction plus automatic cross-platform
Guide UI is a platform product decision, not a bounded bug fix to hide inside SAMPLE-106.

## Owner decision required (2026-09-01 wording; see the updated options above)

Choose one:

1. accept an evidence-backed Windows Phone media-library non-port for this physical directory;
2. authorize a reusable cross-platform media-save contract and general Guide integration, defining
   native Pictures behavior, browser download/persistent-library semantics, permissions,
   collection/readback identity and the required real-browser gate; or
3. explicitly approve a narrower modernization (for example native Pictures plus browser
   download) and record that its browser result is not XNA `MediaLibrary` parity.

Until that choice, no CNA/Sharp Runtime source change or sample port is justified. The exact XNA
content and all audit evidence remain ready if the owner selects an implementation scope.
