# SAMPLE-106 — `SavingEmbeddedImages_4_0` audit and owner decision

## Status

Fresh audit complete enough to require an owner scope decision under `SAMPLES-DEC-004`. No partial
game, browser-download substitute, virtual-filesystem-only save, sample-local Guide overlay or
invented filename UI was added. The row remains `🛑`; only the owner may select a non-port or
cross-platform modernization boundary.

## Classification and complete source inventory

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

## Original build and content evidence

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

## Live CNA capability audit

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

## Why no partial port was added

A C++ screen that merely draws the exact assets would omit both demonstrated save routes. Writing
inside Emscripten's transient virtual filesystem would claim success without giving the user a
picture. Forcing an `<a download>` changes library identity, collection/readback and persistence
semantics. Calling `RenderPending*EXT` from this one game's `Draw` would change the original source
contract and make the port depend on CNA-only UI. None is a faithful translation.

The native implementation being feasible does not satisfy this campaign's mandatory real-browser
gate. Implementing a browser media-library/download abstraction plus automatic cross-platform
Guide UI is a platform product decision, not a bounded bug fix to hide inside SAMPLE-106.

## Owner decision required

Choose one:

1. accept an evidence-backed Windows Phone media-library non-port for this physical directory;
2. authorize a reusable cross-platform media-save contract and general Guide integration, defining
   native Pictures behavior, browser download/persistent-library semantics, permissions,
   collection/readback identity and the required real-browser gate; or
3. explicitly approve a narrower modernization (for example native Pictures plus browser
   download) and record that its browser result is not XNA `MediaLibrary` parity.

Until that choice, no CNA/Sharp Runtime source change or sample port is justified. The exact XNA
content and all audit evidence remain ready if the owner selects an implementation scope.
