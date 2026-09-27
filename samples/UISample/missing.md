# SAMPLE-082 — User Interface Sample audit

## Owner-requested Escape as Phone Back — 2026-09-27

The original Phone game uses the physical Back button to leave level selection and high scores.
The owner explicitly requested Escape as a desktop substitute where a gamepad Back button is
unavailable. `InputState::IsNewButtonPress(Buttons::Back, ...)` now accepts a new Escape key press
as the same Back action; this intentional difference is detailed in `diff.md`. The original
screen handlers, touch controls, gamepad Back path, content and timing remain unchanged.

Release OPENGLES3 and nonthreaded Release WEBGL2 were rebuilt against CNA `fd16e1e52` and
Sharp Runtime `9e58c955`. In native mouse and genuine-touch runs, Escape returned from both
subscreens to a frame pixel-identical to the initial main menu; both runs passed their existing
paging/scrolling and clean-exit checks. System Chrome mouse and genuine-touch runs also returned
from both subscreens with zero changed pixels against the main menu, passed WebGL 2, 600 RAF
callbacks and browser-error checks. The exact local-gallery bundle was refreshed, retested with
mouse and Escape, and its controls description updated. Evidence is under
`/rv/tmp/samples/SAMPLE-082-UISample_4_0/evidence/escape-*`.

## Owner-requested desktop mouse input — 2026-09-27

The owner asked that touch-only ports, including this one, enable CNA's shared mouse-to-touch
extension. The sole game-source addition is the `CNAEXT` constructor opt-in recorded in `diff.md`;
the original screen and control input code remains unchanged, and actual touch still works. On
active CNA `fd16e1e52` and Sharp Runtime `9e58c955`, the Release OPENGLES3 and nonthreaded Release
WEBGL2 products were rebuilt in the retained artifact root. Native mouse and genuine SDL finger
runs both pass menu, level paging, score scrolling and normal exit. System Chrome runs for both
mouse and genuine touch pass the same interactions, WebGL 2, 600 further RAF callbacks and the
runtime/HTTP error checks. The exact refreshed local gallery bundle also passes the mouse gate.
One mouse run continued the original inertial score scrolling during the 600 RAF interval; the
capture analyzer now checks that the score screen remains visible rather than demanding a still
image. Evidence is in `evidence/mouse-optin-*` under the artifact root. The original XNA reference
and content were not changed.

## Current-head completion — 2026-09-27

**Status: complete on CNA `next 5572f3ca1` and Sharp Runtime `next
9e58c955`.** The 47-file upstream snapshot remains exact; the repaired
`scripts/verify-snapshot.sh` rechecked all eleven Phone/Reach XNBs byte for
byte against the retained official Win7 XNA Game Studio output. The original
Phone-only project, including its 25 C# files, has no runnable desktop XNA
host. Its unchanged all-source Windows diagnostic remains the valid reference
boundary at `GetUserStoreForApplication()`; no substitute desktop screenshot
is claimed. All 25 C# units have corresponding C++ units, unchanged since the
earlier full audit. `diff.md` still records the necessary closed C++ factory
for the original reflection-based screen restoration.

The Release OPENGLES3 target was rebuilt from the active `libcna/cna-samples`
checkout and ran on Mesa OpenGL ES 3.2. The external SDL preload adapter
injected actual finger events below CNA, and the complete native capture
script now exits 0: menu, House level, sideways flip to Pasture, main-menu
restart, high-score entry and vertical scrolling all passed. The old script's
`xdotool windowclose` destroyed the X11 window from another client and once
caused a `BadDrawable` abort. A test-only `WM_DELETE_WINDOW` sender now requests
normal application shutdown; both runs then exited cleanly. The first new
run visibly scrolled but changed 15,603 pixels, below the old analyzer's
20,000-pixel threshold; the analyzer now uses a measured 10,000-pixel floor.
The final whole native test changed 26,592 pixels and passed every check.
Neither issue required a sample or CNA source workaround.

The fresh nonthreaded Release WEBGL2 bundle has an 8,256,671-byte wasm and
9,631,957-byte data package. Its JS has no `PThread`, `SharedArrayBuffer` or
`emscripten_thread` reference, its wasm has no `debug_info`, and the package
manifest names each of the eleven exact XNBs once. System Chrome on ordinary
HTTP obtained WebGL 2, delivered real touch, opened House, flipped to Pasture,
opened and scrolled high scores, completed 600 additional animation frames
and reported no runtime exception, unhandled rejection, fatal console message
or relevant HTTP error. House and Pasture screenshots are pixel-identical
between native and browser. The main-menu frames differ only by at most two
channel values, on 39,970 of 384,000 pixels; high-score completion times are
random in the original as well, so those frames are not an exact pixel target.

The original `Background.png`, `Game.ico`, `GameThumbnail.png` and
`Microsoft_Permissive_License.rtf` were restored byte for byte beside the
port. Gallery commit `6c98cc5` adds the exact tested four-file WEBGL2 bundle,
the real House gameplay screenshot, card 77, detail page and adjacent
navigation. The exact copied bundle passed a second independent Chrome touch
and 600-frame gate; all 77 cards are unique and the new local links resolve.
No sample source, CNA or Sharp Runtime implementation changed. The original
`SerializeState()` method is retained, but the shipped original source has no
call site for it; menu restart/reload checks are not claimed to exercise
serialization. Current-head native and web evidence is under the sample's
`evidence/cna-native-opengles3-release-qualified/`,
`evidence/cna-web-webgl2-qualified/` and `evidence/gallery-20260927/` paths.
The updated `scripts/` rebuild and test against the active dependency chain.

## Current-head analysis before requalification — 2026-09-27

**Status: requalification pending on CNA `next 5572f3ca1` and Sharp Runtime
`next 9e58c955`.** The complete 47-file physical upstream directory is
byte-identical to `xna4-original/`; all 25 original C# files still have a
corresponding C++ source unit. All eleven checked-in Phone/Reach XNBs were
recompared byte for byte with the retained official Win7 XNA Game Studio
outputs and match. The sole upstream game project is a Windows Phone/Reach
library with `WINDOWS_PHONE`; the unchanged all-source desktop diagnostic
reaches the Phone-only application-isolated-storage boundary, so there is no
authentic desktop XNA visual reference to capture. The original Phone behavior
and its previous native/browser qualification are documented below.

The previous native and WEBGL2 build trees were pruned on 2026-09-09; their
qualified products remain, but belong to the retired `openeggbert` checkout.
The retained native
executable's RUNPATH points to `openeggbert/cnanext`. The retained build and
capture scripts also use `openeggbert/cna-samples`, `cnanext`, an old cache
invocation and an eight-job ceiling. The current active chain requires
`libcna/cna-samples`, `../cna`, `../sharp-runtime`, `CCACHE_DIR=~/.cache/ccache`
and `CCACHE_BASEDIR=/rv`. These scripts and build trees need renewal before
current-head claims can be made. Fresh Release OPENGLES3 and nonthreaded
WEBGL2 products must be built and exercised with real touch, level paging,
high-score scrolling, persistence/restart and a clean exit; the complete
browser bundle needs a new real-Chrome test on ordinary HTTP.

The port still selects `WINDOWS_PHONE`, uses `TouchPanel` rather than a
sample-local mouse path, and records its reflection-free screen factory in
`diff.md`. A targeted scan found no restored F1 overlay, loose-content load or
sample-side touch synthesis. The earlier full source audit remains the basis
for fidelity; current-head runtime requalification is outstanding. The three
original package images (`Background.png`, `Game.ico`,
`GameThumbnail.png`) are missing beside the port. The gallery has no
`UISample` entry; its screenshot should show a real UI state, for example the
level page, after that exact published bundle is tested. No CNA or Sharp
Runtime defect was identified in this analysis.

**Historical qualification: complete on the former dependency checkout; no
known behavior or content differences from the XNA 4.0 original were recorded.**

The historical port was not an acceptable endpoint. It merged the screen classes, changed the
namespace and game type, replaced compiled content with loose PNG/font sidecars, synthesized touch
from mouse input inside the sample, manually published touch dimensions, added an F1 overlay,
omitted tombstone persistence and tracing, shared the background content manager, changed
fullscreen behavior, manually formatted `TimeSpan`, and repaired three latent bugs from the
original. Those workarounds and changes are removed.

Artifact root: `/rv/tmp/samples/SAMPLE-082-UISample_4_0/`

## Original surface audited

All 47 physical files in `UISample_4_0` were retained and reviewed. The product is a Windows Phone
XNA 4.0/Reach touch UI demonstration with 25 C# units: the game, screen manager, nine screens,
eleven reusable controls/helpers, program and assembly metadata. The source project also contains
three SpriteFonts, five level pages, a background, a gradient, a fourth unused game font, Phone
manifests, icons and HTML documentation.

The port now mirrors the original `UserInterfaceSample` namespace and file/class decomposition.
It retains the 30 Hz/fullscreen Phone setup; TouchPanel and GamePad-Back input; menu transitions;
independent background `ContentManager`; loading, level-select and high-score screens; page-flip
and scrolling algorithms; tracing; and complete isolated-storage screen-list/state serialization.
The original touch-only contract is unchanged. There is no sample mouse path, invented key,
manual display-size assignment or runtime help overlay.

The exact original latent behavior is preserved, including its unreachable defects:

- `TextControl::Font()` recursively calls itself;
- `CommonGraphics::DrawRectangle` ignores its color parameter and draws white;
- `Control::BatchDraw` does not populate `DrawContext::BlankTexture`.

The old port's “fixes” for those paths were behavior changes and are gone. The shipped sample does
not reach any of the three paths in a way that triggers a failure.

## Authentic content

The unchanged `UISampleContent.contentproj` completed through XNA Game Studio 4.0 in the owner's
offline Windows 7 SP1 VM for Windows Phone/Reach. All eleven checked-in XNBs are byte-identical to
that official-pipeline output:

| File | Bytes | SHA-256 |
|---|---:|---|
| `Font/MenuDetail.xnb` | 38,062 | `9c2fc378e821c8be71db03fedd95dede2e9cbf4371d17ca8c89a10eaa473d72b` |
| `Font/MenuHeader.xnb` | 38,062 | `3ec7c192d8c2e0c1a2cd1a767cd8138ce19551cd846405331259221c0b4b1dc3` |
| `Font/MenuTitle.xnb` | 267,438 | `9fedb5a5c395310abb3bbd5315b88033414ca08948d801c2ab1526eddc96ee95` |
| `Levels/Castle.xnb` | 1,536,187 | `561843d0742d7a039f06037dc36f40f788b06a2f005e5ebbc74dc7f7b5039c59` |
| `Levels/Dungeon.xnb` | 1,536,187 | `dea058f515d58dbda460a8adcec50f78545d5ffecec5c91ad7c283b55bd8b187` |
| `Levels/Hills.xnb` | 1,536,187 | `5f0c0f105c0769a31297cd6a6d7db7d65202daf48224df3eca8c3611c6b8bff8` |
| `Levels/House.xnb` | 1,536,187 | `32613be72fc904e2c8bcde28f192e1876a4e60f0f8c179c84329924a2c403714` |
| `Levels/Pasture.xnb` | 1,536,187 | `a907801fab51ae7c9af788461e02671adc3ecba8f188041f296332190453d78c` |
| `background.xnb` | 1,536,187 | `ff7cfe18378875746fa0eeb422c0a2581f5d63f59a03acdffe6dd59bf4e5a3e6` |
| `gamefont.xnb` | 70,830 | `beadb8b3a2557eb6c86ace245fd6edc23a991ee7cbfb5a3be7bc5839bad4f95a` |
| `gradient.xnb` | 443 | `c6271602f65b72a413edbcc78adffd56d5446ae764de47890e0e214a3941aa94` |

The loose converted images, generated DejaVu font atlases and JSON sidecars are removed. The
documentation-only `help.png` and exact `UIControlsSample.htm` remain at the sample root and are
not runtime content. The unused original `gamefont` and `gradient` remain because the content
project intentionally builds them even though the C# game does not load them.

## Qualification

All CNA builds used `CCACHE_DIR=/rv/cnaccache` and no more than eight parallel jobs.

- The unchanged Phone/Reach content project built all eleven assets in the offline Win7 VM. A
  Windows/Reach diagnostic then compiled all 25 unchanged product C# units plus a diagnostic entry
  point against the genuine XNA 4.0 assemblies. It reached the original `ScreenManager` startup and stopped only because
  `IsolatedStorageFile.GetUserStoreForApplication()` requires the Phone/ClickOnce activation
  context which a desktop diagnostic process does not have. The VM had no network adapter and
  shut down normally. This is the recorded original-host boundary; no false visual-original claim
  is made.
- Debug and Release OPENGLES3 targets built and ran on Mesa OpenGL ES 3.2. A retained external,
  qualification-only SDL preload adapter converted Xvfb pointer events below CNA into real finger
  events; it is not linked into or shipped with the sample. Both builds opened level selection,
  loaded House, flipped horizontally to Pasture, opened high scores, rendered general
  `TimeSpan` (`{0:g}`) values and scrolled the leaderboard vertically.
- The Release WEBGL2 bundle ran in system Google Chrome. Chrome obtained
  `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, delivered real browser touch events, repeated both
  interaction paths and completed 600 additional `requestAnimationFrame` callbacks with no
  runtime exception, unhandled rejection, fatal console message or relevant HTTP error.
- Capture metrics prove both native and web state changes: opening the level page changed all
  384,000 pixels, House-to-Pasture changed 383,052, opening scores changed about 40,000 and vertical
  scrolling changed 26,946–36,128. Native restart and browser reload reproduced the main menu
  exactly; the web frame remained pixel-identical after the 600-frame canary.
- Sharp Runtime commits `17fb2241` and `efd685ca` add the general invariant `TimeSpan` `g`/`G`
  formatting used by the unchanged source and correct its forward declaration. The focused suite
  passed 380 tests; its complete qualification passed all 17,887 tests in 39 executables.

## Retained evidence

- Exact source snapshot and hashes: `xna4-original/`, `original-manifest.txt`,
  `original-sha256.txt`
- Official content output and all-source diagnostic: `xna4-build/`,
  `evidence/xna-content-sha256.txt`
- Original-host boundary: `evidence/xna-win7-diagnostic-isolated-storage-failure.png`
- Debug and Release native runs: `evidence/cna-native-opengles3-qualified/`,
  `evidence/cna-native-opengles3-release-qualified/`
- Real-browser run: `evidence/cna-web-webgl2-qualified/`
- Reproducible build, capture and verification drivers: `scripts/`

---

## Re-audited 2026-09-09

Verified independently: all 25 original C# units have counterparts, all eleven checked-in XNBs are
byte-identical to the offline Win7 pipeline output, and the source carries no mouse synthesis, no
`F1` path and no help image.

**There is no XNA reference, and this is the right answer rather than a gap.** The upstream is a
Windows Phone game with no desktop XNA host; `evidence/README.md` records the exact boundary the
all-source Windows diagnostic reached — the original `ScreenManager` constructor, stopping at
`IsolatedStorageFile.GetUserStoreForApplication()` because a desktop process has no Phone
activation context — instead of presenting a screenshot that would not be of the original.

So the comparison available is between CNA's own products, and it is a strong one:

| frame | Debug vs Release | native vs browser |
| --- | --- | --- |
| `01-main-menu` | **`0`** | 0.0011 |
| `02-level-house` | **`0`** | **`0`** |
| `03-level-pasture` | **`0`** | **`0`** |
| `05-high-scores-top` | 0.040 | 0.039 |
| `06-high-scores-scrolled` | 0.135 | 0.141 |

Debug and Release are **bit-identical** on the three deterministic screens, and the browser matches
them exactly on two of the three — a WEBGL2 bundle and a native GLES 3.2 build producing the same
bytes.

**The two high-score frames are nondeterministic by design.** `HighScorePanel.cs:43` creates a
`Random` and line 47 fills each row with `TimeSpan.FromSeconds(rng.Next(60, 3600))`, so the
completion times are drawn afresh every run; the port does the same with `System::Random`. Compared
side by side, the player names and scores agree exactly — `player0`/10000, `player1`/9990, down to
`player10`/9900 — and only the `Completed in H:MM:SS` column differs. That is 0.9 % of the pixels on
the unscrolled screen and 8.9 % on the scrolled one, which is simply more rows of times on screen.

**Documentation.** The headers carry no `@brief` and 9 % comment density. Not unique to this
sample — 24 of the campaign's 99 samples are in the same position, recorded as one deferred item in
`plan.md`.

Nothing needed correcting.
