# Missing / Differences from XNA 4.0 original

**Current status: requalified on 2026-09-26.** The port uses the original seven
stock XNBs and preserves the complete positional-audio behavior without a
sample workaround. The only current source correction is the direct C++ pointer
call for CNA's `EffectPassCollection`; the original icon and PNG were restored.
No active behavioral difference or framework change was found.

Artifact root: `/rv/tmp/samples/SAMPLE-059-Audio3DSample_4_0/`.

## Current-head source, content and build

The physical `/rv/tmp/XNAGameStudio/Samples/Audio3DSample_4_0` directory and
retained `xna4-original/` snapshot are byte-identical. Windows/Reach and
Xbox/Reach projects compile the same seven runtime classes plus AssemblyInfo;
all seven content declarations use stock XNA importers/processors. The C++ port
keeps the original audio manager, cat/dog update schedule, camera/listener
controls, billboards, ground, alpha test and draw order. Its `Content.Load<T>`
paths consume exact XNBs; no raw loader, loose sound/texture, Doppler patch,
invented overlay or active `help.png` use remains. A targeted scan found only
the required `CNAEXT` logical type names. The port's `Game.ico` and
`3DAudioSample.PNG` now match upstream byte for byte; its HTML topic and
license also match.

`scripts/build-original.sh` rebuilt unchanged Windows/Reach source, the
Windows/Reach pipeline, and Xbox/Reach content. All seven **fresh** Windows
XNBs match checked-in `samples/Audio3D/Content`, XNA EXE Content and current
native Content byte for byte. `scripts/build-current.sh` configured Release
OPENGLES3 and nonthreaded WEBGL2 with the active sibling CNA `next`
`cefe6c83b` and SharpRuntime `next` `41b918c9`, pinned FNA3D and shared
ccache. The native executable's RUNPATH includes the active CNA SDL location.
The port edit in `QuadDrawer.cpp` changes `[0].Apply()` to `[0]->Apply()`;
it is the direct C++ call syntax for the current `EffectPass*` API.

## Current-head execution and sound

The unchanged Windows XNA EXE ran through WineD3D on isolated Xvfb with a
temporary private copy of the established Wine prefix. The real 800×480 window
showed the moving cat, stationary dog and checker ground; left-camera input
and Escape passed. Its captured audio is real PCM16 stereo at 44.1 kHz.
`evidence/requal-20260926/xna-original-final/` retains the window screenshots,
run log and 10.40-second recording. The private prefix avoids the shared
prefix's Wine display-device state; the executable itself was not patched.

Current native OPENGLES3 rendered the same scene, turned the camera, exited
with Escape and logged no fatal error. Its first isolated PulseAudio recording
was silent because Pulse stream restore routed the SDL process to the normal
speaker sink despite `PULSE_SINK`; a live sink-input inspection identified that
route. The capture script now moves **only this process's** stream to its null
recording sink, without changing the system default. The passing capture is
`evidence/requal-20260926/cna-native-opengles3-routed/`.

Current nonthreaded WEBGL2 ran over plain local HTTP in the system Google
Chrome. A trusted click unlocked audio under normal browser autoplay rules.
WebGL 2, the 800×480 canvas, stock content, cat animation, left-camera input,
Escape stopping frames, all four bundle responses and renderer/audio logs
passed. There were no relevant HTTP failures, unhandled rejections, runtime
exceptions or fatal console errors. JS has no pthread/shared-memory runtime
markers and WASM has no debug section. The canonical capture is
`evidence/requal-20260926/cna-web-webgl2/`.

| capture | first signal | active frames after start | longest ≤−60 dB run |
|---|---:|---:|---:|
| unchanged XNA | 0.048 s | 81.46% | 0.56 s |
| current OPENGLES3 | 0.688 s | 86.30% | 0.54 s |
| current WEBGL2 | 2.933 s | 84.49% | 0.55 s |
| gallery WEBGL2 copy | 2.336 s | 85.73% | 0.59 s |

All recordings are PCM16 stereo at 44.1 kHz and show changing left/right
balance. Startup offsets differ by harness; the recurring silent run confirms
the dog sound reaches its authored rest interval instead of playing a
10×-slowed first pass. The historical owner listening confirmation of the
corrected sound remains below. The general CNA Doppler fix `e1d3aa5d5` and
SharpRuntime `Double` alias `eebebd86` are ancestors of the active heads; no
new CNA or SharpRuntime source change was necessary.

At the paired two-second screenshots, XNA/native pixels agree within eight
RGB levels at 94.06%; XNA/WEBGL2 at 96.84%. The continuously moving cat is
at a different phase/position in the timed captures. The static left sky
matches 100% in both pairs; the dog region matches 99.51% native and 96.75%
WEBGL2 within the same tolerance. The checker ground, textures, dog size,
horizon, clear color and 800×480 output visually agree. `scripts/compare.py`
and the retained screenshots provide the reproducible comparison.

## Current gallery and artifact result

The 58th gallery card and `Audio3D.html` use a real current game screenshot.
All four gallery bundle files are byte-identical to the tested nonthreaded
Release output. The exact gallery copy passed the same Chrome and audio gates
again; its 13.75-second recording includes a 0.59-second silent run. The
card, detail, reciprocal navigation and 14 local HTTP routes passed; see
`evidence/requal-20260926/gallery-webgl2/` and `gallery-routes.txt`.

`MANIFEST.md`, `scripts/build-current.sh`, `scripts/capture-original.sh`,
`scripts/capture-cna-native.sh` and `scripts/capture-web.sh` reproduce the
active checks. The owner-authorized 2026-09-26 prune removed 27 intermediate
paths, reducing the artifact root from 199.8 MB to 52.7 MB (147.1 MB freed,
including stripping and deduplication). Of 247 pre-prune retained-tree files,
228 kept their hashes, 18 generated CMake/object files were removed, and only
the stripped native executable changed. It passed rendering, left-camera
input, Escape and real stereo recording again afterward; the dog-rest silent
run remained 0.54 s. A repeat dry run proposes zero paths. The manifest and
`build-current.sh` retain active rebuild commands; the source and exact web
bundle remain reproducible. Historical pre-fix captures below remain evidence
of the former general Doppler defect.

## Historical completion evidence — 2026-09-05

## Doppler correction and regression evidence

The owner's listening check found that both animal recordings sounded like a motorcycle. The
assets were not responsible: the checked-in XNBs remain byte-identical to XNA's own output, and
the source WAVs contain normal barking and meowing. The defect was in CNA's ordering of
`SoundEffect.DopplerScale`.

The sample sets the global scale to `0.1`. CNA previously calculated the physical Doppler factor
using only `AudioEmitter.DopplerScale`, then multiplied the completed frequency ratio by `0.1`.
Consequently, even a stationary listener/emitter pair played at one tenth speed. Microsoft XNA
4.0's `KernelSoundEffectInstance.Apply3D` IL instead multiplies the global and emitter scales
before `X3DAudioCalculate`, and applies only the resulting `dspSettings.DopplerFactor` to pitch.
FNA's current ordering diverges from XNA here.

CNA commit `e1d3aa5d5` fixes the general runtime path, with no sample workaround. Two new tests were
red before the repair (`0.1` instead of `1.0`, and `0.4` instead of `8/9`) and green afterward;
all 95 `SoundEffectInstanceTest.*` cases also pass. The native and WEBGL2 Release sample targets
both compile from clean build trees against that implementation.

## Fresh source and project audit

The complete upstream directory was copied unchanged to `xna4-original/`. Every C# file, Windows
and Xbox project/solution, configuration, content declaration, source asset, HTML topic, icon and
license was reviewed against the port. Both original projects compile the same seven source files
and seven content items; the active Windows project targets Reach. The old port diverged in four
material ways:

- it merged `IAudioEmitter`, `AudioManager`, `QuadDrawer`, `SpriteEntity`, `Cat`, `Dog` and
  `Audio3DGame` into one 418-line header;
- it loaded four loose WAV files and three TGA-to-PNG conversions instead of the original XNBs;
- it invented an F1 input/timer/`SpriteBatch` overlay and packaged a non-original `help.png`;
- it flattened original properties, made the cat random generator per-instance, omitted
  `AudioManager.Dispose(bool)`, added constructors absent from `Cat`/`Dog`, and returned the wrong
  logical game type name.

All four differences are removed. The original seven-type decomposition and member order are
restored. `AudioManager` retains its component lifecycle, static four-name table, dictionary/list
ownership, stopped-instance disposal loop and `try`/`finally`-equivalent disposal. `Cat` uses the
original static random generator, circular position, per-update velocity and 1.25-second
single-shot schedule. `Dog` retains the stationary emitter and exact six-second loop/four-second
rest cycle, including non-immediate `Stop(false)`. Camera input, listener updates, billboard and
ground transforms, alpha test, blend state and update/draw order follow the source line by line.
Logical type names are `Audio3D.AudioManager` and `Audio3D.Audio3DGame`; the assembly title is
`Audio 3D`.

## Exact official content

The unchanged XNA 4.0 content project builds successfully for both Windows Reach and Xbox Reach.
The seven checked-in files are byte-identical to the active Windows Reach output:

| asset | size | SHA-256 |
|---|---:|---|
| `CatSound0.xnb` | 61,739 | `bc9139ed7cd14be8df1c840d5e177c7329b643ec38a7e9098d8903783c2690e1` |
| `CatSound1.xnb` | 56,865 | `eb7112b60c237526bbcdd020d0e21b9ef2dd71e8a8e890adc3075235c0bdda8f` |
| `CatSound2.xnb` | 63,725 | `b8ea8c180ffaba17bfa823620d425053d514a4535ff1fc25b6040cf20814532b` |
| `CatTexture.xnb` | 349,743 | `d41b34efb6694c6c2a2be08694ec025d926c724950b26a19eceec3da26983efc` |
| `DogSound.xnb` | 378,591 | `d22abfe7de9c410b5e4cf6fda7285e3f9e79f436f19a55f22bb9c5be2a85b233` |
| `DogTexture.xnb` | 349,743 | `2d7e1e0108849e3e54e2be459e0e2da9d0ec211e8af9b5161356116648d2be3e` |
| `checker.xnb` | 87,595 | `cb2f15eafaec7e8e6394e19646bd122a610e66a73e7066ace6c9457b9ba04891` |

Reader-table evidence records one `SoundEffectReader` for each sound and one stock
`Texture2DReader` for each texture, with no shared resources. The four loose WAVs and three
converted PNG substitutes are gone. Historical `help.png` is retained at the sample root and is
neither packaged nor loaded.

## Original execution

The unchanged Windows source compiles to `xna4-build/bin/Audio3D.exe` and runs under the campaign
Wine prefix with WineD3D. The harness captures the initial orbiting cat, centered dog and repeated
checker ground, records the real stereo output, rotates the camera left and verifies clean Escape
exit. The original capture is PCM16 stereo at 44.1 kHz. After aligning past Wine startup, its
250 ms left/right balance spans -26.89 to +10.32 dB and includes the expected loop-rest gaps.

## Native XNA/CNA fidelity

The Release OPENGLES3 target builds and runs directly from the seven exact XNBs. It captures the
same three visual/input states, exits cleanly and logs no fatal runtime error. Timed comparisons
remain strong even though the cat is continuously animated and the two independent loops can
advance by slightly different frame counts:

| comparison | exact pixels | within 8 | after 4 px blur, within 8 |
|---|---:|---:|---:|
| XNA vs OPENGLES3 at 2 s | 86.50% | **96.02%** | **97.62%** |
| XNA vs OPENGLES3 at 7 s | 86.82% | **96.31%** | **97.95%** |
| XNA vs WEBGL2 at 2 s | 87.69% | **96.51%** | **97.53%** |

The frames agree on the exact textures and alpha masks, dog and cat scale, camera/horizon,
checker frequency, clear color and 800x480 output. Residual differences are the moving cat's
phase and texture filtering around high-contrast checker edges, not missing content or geometry.

The original native capture was real PCM16 stereo at 44.1 kHz, but its audit checked signal,
spatial balance and looping without checking playback rate; that is why it missed the audible
Doppler defect. In that recording, 99.83% of frames after first signal were active and there was no
10 ms silent block at all, consistent with the 4.291-second dog recording being stretched to
roughly 42.91 seconds.

The fresh post-fix OPENGLES3 capture is again PCM16 stereo at 44.1 kHz and exits cleanly. It has an
84.82% post-start active-frame ratio and a 0.61-second measured silent run, demonstrating that the
dog loop now finishes and reaches its authored rest interval instead of remaining inside a
ten-times-stretched first pass. Its peak sample magnitude is 18,484 and moving spatial balance is
still present (-32.12 to +5.04 dB in 250 ms windows). A
temporary owning-layer diagnostic verified the dog's non-immediate stop changes SDL_mixer's
active loop count from infinite (`-1`) to no further loops (`0`) successfully; the final runtime
source and capture contain no diagnostic code. This matches FNA's `FAudioSourceVoice_ExitLoop`
behavior: the current pass completes rather than being cut off.

## Web audio and input

The previously audited Release Emscripten `WEBGL2` bundle ran in the system Google Chrome over
local HTTP.
The gate uses Chrome's normal autoplay policy, waits for the live 800x480 canvas and dispatches a
trusted click before measuring WebAudio; it does not use an autoplay-disable flag. It verifies a
real WebGL 2 context, original title, renderer and audio-mixer logs, moving cat, left-camera input,
and Escape. Two post-Escape frames are byte-identical, proving the game loop stopped.

The `.html`, `.js`, `.wasm` and `.data` requests all return successfully, with no promise
rejection, runtime exception, relevant HTTP failure or fatal console message. Browser audio is
PCM16 stereo at 44.1 kHz with peak magnitude 23,875, 250 ms balance from -20.01 to +24.18 dB and
measured silent gaps up to 1.29 seconds during the dog rest interval. Thus the browser gate covers
decoded XNB audio, loop transitions, spatial panning, movement and user activation, not merely a
successful visual load.

That old gate did not compare playback rate or timbre and therefore could not close the newly found
Doppler regression. A fresh clean WEBGL2 build against CNA commit `e1d3aa5d5` succeeds and emits
the complete `.html`, `.js`, `.wasm` and `.data` bundle. Its Emscripten worker build requires
cross-origin isolation; the first manual load through a plain `python -m http.server` correctly
failed with `WebAssembly.Memory object cannot be serialized`. The retained
`serve-cross-origin-isolated.py` helper and updated `capture-web.sh` supply
`Cross-Origin-Opener-Policy: same-origin` and
`Cross-Origin-Embedder-Policy: require-corp`. After reloading through that server, the owner
confirmed the page and corrected animal sounds work. This closes the fresh real-browser gate.

## Framework and runtime result

CNA required one general audio repair: `SoundEffectInstance::Apply3D` now combines global and
emitter Doppler scale inside the physical calculation, matching XNA rather than FNA's divergent
post-calculation multiplication. Distance attenuation, stereo spatial matrix and decoded
`SoundEffectReader` output were unaffected. No EasyGL, MetaGL or sample-side change was needed.

The source uses C# `double` for `GameTime.TotalGameTime.TotalSeconds`; SharpRuntime lacked the
corresponding primitive alias. `sharp-runtimenext` commit `eebebd86` adds the general
`SharpRuntime::Double = double` alias and focused integration coverage. This is the only
cross-repository repair found by the audit.

## Intentional C++ mappings

- C# reference ownership maps to `std::unique_ptr`, `std::shared_ptr` and `std::optional` while
  preserving object lifetimes and initialization order.
- C# properties map to `getXProperty()`/`setXProperty()` accessors; the protected velocity setter
  remains protected.
- The nullable emitter and texture references map to non-owning pointers.
- `System.Collections.Generic.Dictionary` and `List` retain the original collection semantics.
- C# `float`, `double`, `string` and `int` use SharpRuntime aliases; `Math.Cos`/`Sin`, `Random`,
  `Int32.ToString` and `TimeSpan` remain System/SharpRuntime concepts.
- `static void Main()` plus `using` maps to `int main()` plus stack RAII.

These are lossless language mappings. There is no owner-approved behavioral addition and no
`diff.md` is needed.

## Documentation and evidence

`Audio3D.htm` and `Microsoft Permissive License.rtf` are byte-identical to upstream. Important
artifact paths are:

- `xna4-original/`: complete untouched upstream snapshot;
- `xna4-build/`: unchanged executable and Windows/Xbox Reach pipeline products;
- `cna-native-opengles3/` and `cna-web-webgl2/`: clean post-fix Release products retained after
  pruning;
- `scripts/`: original pipeline/build, XNB inspection, audio analysis and all three capture gates;
- `evidence/xna-original/`: original XNA frames, logs and stereo capture;
- `evidence/cna-native-opengles3/`: post-fix native frames, run log and real stereo capture;
- `evidence/cna-web-webgl2/`: owner confirmation and hashes for the post-fix browser bundle;
- `evidence/cna-{native-opengles3,web-webgl2}-pre-doppler-fix/`: retained visual/input evidence
  from the earlier audit only; the defective pre-fix WAV recordings and their audio metadata were
  deliberately removed during pruning;
- `evidence/{xnb-reader-tables,xnb-sha256,pixel-comparison,audio-analysis,loop-diagnostic}.txt`:
  content, visual, audio and loop measurements.

There is no remaining SAMPLE-059 blocker, source-port omission, substitute or sample-side
workaround.
