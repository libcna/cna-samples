# SAMPLE-110 — `WP7MusicManagement_4_0` audit and owner decision

## Status

Fresh audit complete enough to require an owner scope decision under `SAMPLES-DEC-004`. No
draw-only screen, always-controlled music simplification, fake launcher, browser-popup substitute
or sample-local Guide overlay was added. The row remains `🛑`; only the owner may select a
Windows Phone non-port or a cross-platform platform-task modernization boundary.

## Classification and complete source inventory

The physical upstream directory is one Windows Phone 7/Reach XNA game with one content project.
The audit covers all 17 files: the solution and both projects, both phone manifests, four packaging
images, the loose package video, the song and SpriteFont sources, HTML documentation, licence and
all 677 checked-in C# lines.

The product is not merely a screen that plays an MP3. Its documented purpose is the reusable
`BackgroundMusicManager` and its response to external phone activities that affect background
music:

- it runs at the Phone default 30 Hz, enables only `GestureType.Tap`, exits through GamePad Back
  and draws four touch buttons at the original positions;
- the manager samples `MediaPlayer.GameHasControl`, remembers the requested `Song`, polls once per
  second, resumes a paused song, restarts stopped playback and stops music when the game no longer
  requests a song;
- if the user already owns the music session, `PromptGameHasControl` opens the original Guide
  Yes/No prompt and stops the user's music only after consent;
- `InvalidOperationException` from Play/Resume (documented by the sample for a Phone connected to
  Zune) clears the pending song and raises `PlaybackFailed`, which opens a distinct Guide message;
- **Play Video** launches the packaged `Video.wmv` through `MediaPlayerLauncher` with no controls.
  Phone music pauses, and activation must restart it because this launcher does not resume it;
- **Get Picture** launches `PhotoChooserTask` with camera enabled. The framework pauses music and
  resumes it at the old position when the chooser returns;
- **View Website** launches `WebBrowserTask` for `http://create.msdn.com`. The framework pauses and
  later restarts music rather than resuming at the old position;
- **Toggle Music** exercises the manager's normal Play/Stop path.

Those three deliberately different deactivate/reactivate cases are the sample's observable lesson.
Omitting them leaves neither the documented product nor a useful test of the manager.

## Original build and content evidence

Artifact root: `/rv/tmp/samples/SAMPLE-110-WP7MusicManagement_4_0/`.

- `original/` is the complete byte-for-byte upstream snapshot.
- The owner-provided Windows 7 SP1 32-bit VM ran headless with all eight virtual network adapters
  set to `none`; no guest browser or internet access was enabled. The VM was shut down normally and
  the temporary `cna_sample110` share was removed after export.
- The unchanged Phone game solution reaches XNA's project validation and stops at
  `Microsoft.Xna.GameStudio.targets(34,5)`: this XNA installation lacks the Windows Phone XNA
  project extension. An authentic Phone runtime/emulator capture is therefore unavailable in this
  reference VM.
- The unchanged content project succeeds through XNA Game Studio 4.0's official
  `FontDescriptionProcessor` and `Mp3Importer`/`SongProcessor` routes for WindowsPhone/Reach. It
  produces:

  | Asset | Bytes | SHA-256 |
  |---|---:|---|
  | `Font.xnb` | 21,678 | `4d7e8609a4a96bcfdeb72453c7afe2cb68e63f31be32c7331121683c2dfcea45` |
  | `Music.xnb` | 123 | `3554610e02ee60aaff062817a659cae26d5671f6d789caaf5133449fbb280b2f` |
  | `Music.wma` | 801,351 | `21ced19d22edb37bb16dc504a601eee8a2ce1473279bdda81e2ad103fac8231c` |

The XNB has the authentic `XNBm` version-5 header, `SongReader`/`Int32Reader`, `Music.wma` sibling
and 32,054 ms duration. It is byte-identical to SAMPLE-060's already native/browser-qualified XNA
Song XNB. Although this fresh WMA encoder run produced different container bytes, it is valid WMA
v2 stereo 44.1 kHz, and decoding it, SAMPLE-060's retained WMA and SAMPLE-060's lossless Ogg-FLAC
deployment companion yields the same 5,652,480-byte signed-16-bit PCM stream with MD5
`8c0a3b34c5b4b1977b773689e4f01ecb`. Content is not the blocker.

Exact commands, both MSBuild logs, output listing, hashes, binary header and PCM comparison remain
under `build-win7.cmd`, `evidence/` and `win7-export/`.

## Live CNA capability audit

The portable pieces are real and tested:

- `ContentManager`/`SongReader` load real external-media Song XNBs and preserve the recorded
  duration; SAMPLE-060 already proves this exact XNB through native OPENGLES3 and real WEBGL2;
- `MediaPlayer` implements Play/Pause/Resume/Stop, state, queue and deferred events;
- `Game.Activated`/`Deactivated`, Phone-space touch input, SpriteFont drawing and the asynchronous
  `Guide.BeginShowMessageBox`/`EndShowMessageBox` shape exist;
- 68/68 focused live Debug tests pass with `SDL_VIDEODRIVER=offscreen`: four `SongContentTypeReader`,
  three `ContentManagerSongXnb`, eighteen `MediaPlayer`/no-sound and 43 `Guide` cases.

The defining platform behavior is absent:

1. `MediaPlayer.GameHasControl` unconditionally returns `true`, and a regression test explicitly
   preserves that FNA/desktop behavior. It cannot represent Phone user-music ownership, the
   consent path or the Zune failure condition demonstrated here.
2. Neither CNA nor Sharp Runtime exposes `Microsoft.Phone.Tasks.MediaPlayerLauncher`,
   `PhotoChooserTask` or `WebBrowserTask`; CNA's audit already classifies photo pickers as outside
   its current device scope. CNA's in-game `VideoPlayer` is not an external OS launcher and cannot
   reproduce the sample's activation and resume/restart distinction.
3. CNA Guide message boxes remain caller-rendered through `CNAEXT` helpers. The original game does
   not draw a Guide overlay itself; adding those helpers only here would be a forbidden sample
   workaround.
4. A truthful replacement crosses several platforms: native/browser video launching, browser
   popup and user-gesture policy, photo/camera picker permissions, window/app deactivation,
   WebAudio suspension/ownership, automatic Guide UI and exact return-to-game playback position.
   These are platform services and lifecycle policy, not a bounded correction to `SimpleButton` or
   `BackgroundMusicManager`.

## Why no partial port was added

A C++ version containing only Toggle Music would remove three of four buttons and every defining
external-event transition. Making them no-ops would falsely present success. Replacing the video
launcher with a texture-rendered `VideoPlayer`, the photo chooser with a bundled image, or the web
task with a platform-specific call changes the demonstrated contracts. Hardcoding
`GameHasControl=true` also makes the consent and failure branches unreachable. None is a faithful
translation, and the campaign's mandatory browser gate prevents calling a native-only approximation
complete.

## Owner decision required

Choose one:

1. accept an evidence-backed Windows Phone music-management/platform-task non-port for this
   physical directory;
2. authorize a reusable CNA platform-task and music-ownership subsystem covering the three
   launchers, automatic Guide integration, native/browser lifecycle, permissions and the exact
   pause/resume/restart semantics, with real platform qualification; or
3. explicitly approve and define a narrower cross-platform modernization, including which Phone
   behaviors may change and what the mandatory WEBGL2 acceptance test becomes.

Until that choice, no CNA/Sharp Runtime source change or sample port is justified. The exact XNA
content and audit evidence remain ready if the owner selects an implementation scope.
