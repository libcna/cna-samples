# SAMPLE-082 — User Interface Sample audit

**Status: complete — no known behavior or content differences from the XNA 4.0 original.**

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
