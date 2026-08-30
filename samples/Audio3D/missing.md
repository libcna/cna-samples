# Missing / Differences from XNA 4.0 original

**Current status: complete (fresh audit 2026-08-30).** The port now uses the original seven stock
XNA content products and preserves the complete positional-audio sample without loose media,
converted textures, a merged header, an invented overlay or a sample-side runtime bypass.

Artifact root: `/rv/tmp/samples/SAMPLE-059-Audio3DSample_4_0/`.

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

Native audio is real PCM16 stereo at 44.1 kHz rather than a log-only check: its first signal is at
0.300 s, peak sample magnitude is 18,408, and its 250 ms balance spans -19.38 to +18.31 dB. A
temporary owning-layer diagnostic verified the dog's non-immediate stop changes SDL_mixer's
active loop count from infinite (`-1`) to no further loops (`0`) successfully; the final runtime
source and capture contain no diagnostic code. This matches FNA's `FAudioSourceVoice_ExitLoop`
behavior: the current pass completes rather than being cut off.

## Web audio and input

The complete Release Emscripten `WEBGL2` bundle runs in the system Google Chrome over local HTTP.
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

## Framework and runtime result

No CNA, EasyGL or MetaGL repair was needed. Current CNA already implements the sample's required
distance attenuation, stereo spatial matrix and velocity/Doppler pitch path, and its exact
`SoundEffectReader` output works on native SDL3 audio and browser WebAudio. The superseded report's
claims that Doppler was a no-op and only linear attenuation existed are no longer true.

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
- `cna-native-opengles3/` and `cna-web-webgl2/`: reusable native and browser build trees;
- `scripts/`: original pipeline/build, XNB inspection, audio analysis and all three capture gates;
- `evidence/xna-original/`, `evidence/cna-native-opengles3/` and
  `evidence/cna-web-webgl2/`: visual/audio captures, logs and browser result;
- `evidence/{xnb-reader-tables,xnb-sha256,pixel-comparison,audio-analysis,loop-diagnostic}.txt`:
  content, visual, audio and loop measurements.

There is no remaining SAMPLE-059 blocker, omission, substitute or sample-side workaround.
