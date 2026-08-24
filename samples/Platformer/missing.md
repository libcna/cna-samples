# SAMPLE-013 audit — Platformer_4_0

No known behavioral or visual differences remain after the `SAMPLE-013` audit. There is no
unresolved CNA or sharp-runtime implementation gap for this sample.

## Reference and source audit

- The unchanged Microsoft XNA 4.0 snapshot is retained at
  `/rv/tmp/samples/SAMPLE-013-Platformer_4_0/xna4-original`.
- All fourteen game source files, the Windows, Xbox 360 and Windows Phone projects, the three
  levels and the complete Content project were reviewed. The real XNA run and source are the
  authority; MonoGame was not used as a behavioral substitute.
- `scripts/build-original.sh` reproducibly invokes the Microsoft XNA Content Pipeline and the
  installed .NET 4 `csc.exe` against the XNA 4.0 GAC assemblies. Using Mono `mcs` for the game
  executable is specifically avoided because its emitted `ExtensionAttribute` is incompatible
  with this real .NET 4 runtime.

The C++ translation retains the original class boundaries and source order across `.hpp` and
`.cpp` files. It preserves the three-level loop, two-minute timer, score, overlays, animation
timing, player physics, collision resolution, enemy behavior, gems, exit logic, death/restart
flow, repeated music, all seven sound effects and the original HUD. The level loader again checks
that every row has the same width and throws the original line-numbered failure instead of reading
past a malformed row. `Level` again owns and disposes its level-scoped `ContentManager`.

The full input logic is present: A/D and arrow movement, Space/Up/W jump, analog stick, D-pad,
gamepad A/Back, touch Pressed/Moved input and accelerometer steering with the
`LandscapeRight` reversal. The original `WINDOWS_PHONE` fullscreen, 333,333-tick and physical
accelerometer branches are retained. `CNA_WINDOWS_PHONE_EMULATOR` is the build-time equivalent
of the original WP7 emulator/device selection and enables the unchanged arrow-key accelerometer
simulation; it is inactive in the audited Windows, Linux and browser builds.

Normal C++ ownership (`std::unique_ptr`/`std::optional`), CNA's property naming convention and
`CNAEXT GetTypeName()` are the only representation-level adaptations. There is no invented
Escape/F1 input, help overlay, omitted target branch or simplified gameplay path.

## Faithful content

The former port substituted loose PNG/WAV files and a generated DejaVu Sans atlas for the original
content. Those substitutes have been removed. The runtime now consumes 46 pregenerated XNB files:
37 textures, the original Pericles Regular SpriteFont, seven SoundEffects and one Song. The
checked-in assets are byte-identical to the retained audit outputs; the only additional files under
`Content` are the three unchanged level texts.

The official Wine pipeline can build the texture and SpriteFont assets directly. For the seven
original WMA sound effects, the build script first decodes the source to PCM and then invokes the
real XNA `WavImporter` and `SoundEffectProcessor`; this preserves the decoded audio without a
loose-file runtime path. Wine's old Windows Media components cannot complete `SongProcessor`, so
`BuildSongXnb.cs` emits the canonical uncompressed XNA `SongReader` payload for
`Music.wma` and its original 119,338 ms duration. The original WMA remains byte-identical.

CNA/FNA's faithful `SongReader` extension probe needs a supported companion for that XNA
`.wma` reference. `Music.oga` is therefore a lossless Ogg-FLAC encoding of the decoded original,
not a replacement composition or sample-side media API bypass. The script uses
`-map_metadata -1 -fflags +bitexact -flags:a +bitexact -serial_offset 0`, making repeated builds
byte-identical.

| Representative asset | SHA-256 |
|---|---|
| `Fonts/Hud.xnb` | `e91a8ad00094668b17c2dac4c9ccf2f82b0df265b46ca9cd450e49afbd53351e` |
| `Sounds/ExitReached.xnb` | `0d5ff8aa42610d3c5586c5a61fc63d1b55223a32fd587a88934238b32826d109` |
| `Sounds/Music.xnb` | `4d4d09eaa9b2c8344ac16f3e01359205122ada69807b7a0c767cf79854dcdc53` |
| `Sounds/Music.wma` | `f766a0aa5237128a0c4b43d97aa636f1974094b61ab3d9b70fa79b328d688acf` |
| `Sounds/Music.oga` | `6a4d8090fae4082a38bd89b91b24190b6b59d0cc91778af29fa5be2842583ad7` |

Historical `help.png` is retained beside the sample's `CMakeLists.txt`, outside `Content`, and
is never loaded, copied or preloaded.

## Framework correction and no-workaround review

The old port passed a `GraphicsDevice` manually to each `Level` and called the non-XNA
`ContentManager::setGraphicsDevice` extension. The sample now uses the exact original constructor:
`Level(game.Services, stream, index)`. This exposed and fixed a general CNA defect in
`cnanext` commit `33ff296f5`: a child `ContentManager` now resolves
`IGraphicsDeviceService` from its supplied `IServiceProvider`, matching XNA. A focused
`ResolvesGraphicsDeviceFromServiceProvider` regression test plus the four neighboring real-XNB
tests all pass.

Mechanical scans and line-by-line review found no `RawMesh`/`RawModel`, direct `SetData`
content substitute, backend call, handwritten graphics helper, loose runtime asset, F1/help path,
invented input, silent content simplification or sample-specific framework bypass. No
sharp-runtime change was necessary.

## Verification evidence

All sources, reproducible scripts, binaries and captures are under
`/rv/tmp/samples/SAMPLE-013-Platformer_4_0`:

- `xna4-build/bin/Platformer.exe` is the real x86 .NET 4/XNA 4.0 reference. Run it with:

  ```bash
  cd /rv/tmp/samples/SAMPLE-013-Platformer_4_0/xna4-build/bin
  WINEPREFIX=/home/robertvokac/.wine-cna-xna40 \
  WINEDLLOVERRIDES=d3d9=b WINEDEBUG=-all wine Platformer.exe
  ```

  It rendered the original 800x480 level and accepted the same movement/jump input. The retained
  capture is `evidence/xna-original/platformer-level0.png`.
- `cna-native-opengles3/samples/Platformer/Platformer_cna_samples` reports EasyGL OpenGL ES 3.2
  and `CNA graphics renderer: OPENGLES3`. It loaded every XNB and both music/effect audio paths,
  rendered the same 800x480 level and accepted movement/jump input. Captures are retained under
  `evidence/native/`.
- `cna-web-webgl2/samples/Platformer/Platformer_cna_samples.{html,data,js,wasm}` is the complete
  browser bundle. System Google Chrome reported `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, a live
  800x480 canvas and a completed Emscripten runtime. It rendered the same level, accepted
  movement/jump input and activated audio after the required browser click. No application,
  JavaScript, wasm or WebGL exception occurred. The only HTTP 404 was Chrome's unrelated
  `/favicon.ico`; the only runtime warning was Emscripten's `ScriptProcessorNode` deprecation.
  Captures are retained under `evidence/web/`.
