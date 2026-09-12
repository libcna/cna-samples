# PrimitivesSample audit — SAMPLE-001

## Result

No known differences or sample-side workarounds remain for the selected Windows XNA 4.0
configuration. The Windows project is a runnable game; the phone project is a platform variant and
is not the native/web target of this row.

All retained source, build and comparison artifacts for this audit are stored under:

```text
/rv/tmp/samples/SAMPLE-001-PrimitivesSample_4_0/
├── xna4-original/
├── cna-native-opengles3/
├── cna-web-webgl2/
└── evidence/
```

## Source and fidelity evidence

- The authoritative source is
  `/rv/tmp/XNAGameStudio/Samples/PrimitivesSample_4_0`. The runnable Windows project contains the
  two relevant C# files `PrimitiveBatch.cs` and `PrimitivesSampleGame.cs`; both were compared
  line-by-line with the C++ port. The accompanying `PrimitivesSample.htm` is byte-identical to the
  copy retained here.
- Constants, initialization, 853x480 backbuffer, 500-star generation, large-star construction,
  brightness range, primitive batching and flush boundary, ships, sun, integer center positions,
  draw order and Back/Escape exit behavior match the original.
- `PrimitiveBatch` now preserves the original `IDisposable` shape, exception types and messages,
  `BasicEffect` setup, accepted primitive types and vertex-buffer behavior.
- The former F1 help overlay was removed because it was a cna-samples addition, not part of the
  original sample or either original C# file. Its historical `help.png` is retained beside
  `CMakeLists.txt`, outside `Content`, and is not loaded by the sample.
- The audit found no `RawMesh`/`RawModel`, backend access, direct content substitute, sidecar data,
  invented input, simplified branch or other CNA bypass. This sample has no runtime content assets.
- `System::Random` and `System::Collections::Generic::List` come from `sharp-runtime`.
  `std::vector<VertexPositionColor>` is the backing representation used by SharpRuntime's
  `System::Array` API, while `std::unique_ptr` is the C++ ownership equivalent of the original
  managed lifetime. No SharpRuntime gap was found. The default random constructor deliberately
  gives a non-deterministic star field, so individual star positions are not expected to match
  between separate launches.

## Original XNA 4.0 reference

The unchanged original sources retained in `xna4-original/` were rebuilt with the installed .NET 4
compiler and XNA 4.0 GAC assemblies in Wine prefix
`/home/robertvokac/.wine-cna-xna40`. The resulting Windows executable is
`xna4-original/Primitives/bin/x86/Release/Primitives-audit.exe`.

An ordinary Wine launch is **not** a working reference route on this host. Wine's default DXVK
D3D9 path creates the 853x480 swapchain and then the process exits with code 1; the complete output
is retained as `evidence/xna4-default-dxvk-failure.log`, and
`evidence/primitives-xna-original.png` is the resulting blank capture. The working route explicitly
uses WineD3D instead:

```bash
/rv/tmp/samples/SAMPLE-001-PrimitivesSample_4_0/xna4-original/run-wine-wined3d.sh
```

That relocatable wrapper selects `WINEDLLOVERRIDES=d3d9=b` and the dedicated Wine prefix. It stayed
alive for the full eight-second verification interval instead of exiting during initialization.
The working capture `evidence/primitives-xna-original-wined3d.png` is 853x480 and contains 1,144
non-black pixels, showing the expected random stars, two ships and central sun.

Compiler and output used for the repeatable reference build:

```text
WINEPREFIX=/home/robertvokac/.wine-cna-xna40 wine C:\windows\Microsoft.NET\Framework\v4.0.30319\csc.exe
  /target:winexe /platform:x86 /optimize+
  /out:Z:\rv\tmp\samples\SAMPLE-001-PrimitivesSample_4_0\xna4-original\Primitives\bin\x86\Release\Primitives-audit.exe
  /reference:C:\windows\Microsoft.NET\assembly\GAC_32\Microsoft.Xna.Framework\v4.0_4.0.0.0__842cf8be1de50553\Microsoft.Xna.Framework.dll
  /reference:C:\windows\Microsoft.NET\assembly\GAC_32\Microsoft.Xna.Framework.Game\v4.0_4.0.0.0__842cf8be1de50553\Microsoft.Xna.Framework.Game.dll
  /reference:C:\windows\Microsoft.NET\assembly\GAC_32\Microsoft.Xna.Framework.Graphics\v4.0_4.0.0.0__842cf8be1de50553\Microsoft.Xna.Framework.Graphics.dll
  Z:\rv\tmp\samples\SAMPLE-001-PrimitivesSample_4_0\xna4-original\Primitives\PrimitiveBatch.cs
  Z:\rv\tmp\samples\SAMPLE-001-PrimitivesSample_4_0\xna4-original\Primitives\PrimitivesSampleGame.cs
  Z:\rv\tmp\samples\SAMPLE-001-PrimitivesSample_4_0\xna4-original\Primitives\Properties\AssemblyInfo.cs
```

## CNA finding and regression

The faithful port exposed an EasyGL stock-effect bug: the XNA/D3D9 pixel-center convention was not
applied, so the original one-pixel star triangles disappeared. This was fixed in `cnanext` commit
`76f1f6ebe` rather than enlarged or replaced in the sample. The new
`easygl_xna_pixel_center_test` failed before the fix with zero pixels for the one-pixel triangle
and passes after it with one pixel; its large control triangle also passes.

The following neighboring EasyGL regression executables also pass on OPENGLES3: cull mode (6/6),
real-camera culling (30/30), BasicEffect vertex color (3/3), viewport (5/5), and render-target
orientation (62/62).

The port also exposed a Linux platform regression without changing the sample. The original C#
correctly calls `GamePad.GetState(PlayerIndex.One)` in its first `Update()` before the first
`Draw()`. CNA already initialized SDL's controller subsystem lazily, but the first query still
performed SDL's synchronous udev event-device scan and left the new window black for about 1.6
seconds. This was fixed in `cnanext` commit `5b9287a41`: desktop Linux defaults to SDL's classic
`/dev/input/js*` discovery before the lazy subsystem starts. That path retains SDL hotplug through
its udev callback, or its inotify/fallback scanner where udev integration is unavailable. A host can
explicitly request the event-device path with `SDL_JOYSTICK_LINUX_CLASSIC=0`. The XNA API,
synchronous state-query behavior and sample source are unchanged.

The same Xvfb/Escape timing method measured 1,587 ms from window availability to process exit before
the fix, 38 ms with the classic driver selected manually, and 59 ms after the CNA fix with no SDL
joystick environment override. Keeping `/dev/input/event*` while forcing SDL's non-udev fallback
was tested as the less invasive alternative, but took 2,769 ms and was rejected. The raw record is
retained as
`evidence/linux-gamepad-startup-timing.txt`. Focused CNA tests verify both lazy acquisition and that
an embedding host's explicit SDL setting wins.

## Native CNA verification

```bash
CCACHE_DIR=/home/robertvokac/.cache/ccache CCACHE_BASEDIR=/rv cmake -S . \
  -B /rv/tmp/samples/SAMPLE-001-PrimitivesSample_4_0/cna-native-opengles3 \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCNA_SAMPLES_ONLY=PrimitivesSample \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
CCACHE_DIR=/home/robertvokac/.cache/ccache CCACHE_BASEDIR=/rv cmake --build \
  /rv/tmp/samples/SAMPLE-001-PrimitivesSample_4_0/cna-native-opengles3 \
  --target PrimitivesSample_cna_samples --parallel 2
```

The active-checkout requalification on 2026-09-12 configured against `../cna` and
`../sharp-runtime` with OPENGLES3 and Release mode. CNA was at `1b3151f2f`, sharp-runtime at
`0c82d9b8`, easy-gl at `deda7a4` and meta-gl at `20c8b2d`. The executable opened its 853x480
`Primitives` window, initialized OpenGL ES 3.2 through EasyGL and exited with code 0 after the
original Escape control was sent. The current capture
`evidence/primitives-cna-native-current.png` shows the complete stars, ships and sun scene. The
native executable is retained at
`cna-native-opengles3/samples/PrimitivesSample/PrimitivesSample_cna_samples`.

The earlier qualification capture `evidence/primitives-cna-native-fixed.png` contained 1,169
non-black pixels and matched the same original scene structure; the differing random star layout
is expected. After the historical CNA `5b9287a41` fix, the executable processes the original
first-frame gamepad query without the former black-window pause.

## Browser verification

```bash
CCACHE_DIR=/home/robertvokac/.cache/ccache CCACHE_BASEDIR=/rv \
/home/robertvokac/emsdk/upstream/emscripten/emcmake cmake \
  -S . \
  -B /rv/tmp/samples/SAMPLE-001-PrimitivesSample_4_0/cna-web-webgl2 \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCNA_SAMPLES_ONLY=PrimitivesSample \
  -DCNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
CCACHE_DIR=/home/robertvokac/.cache/ccache CCACHE_BASEDIR=/rv cmake --build \
  /rv/tmp/samples/SAMPLE-001-PrimitivesSample_4_0/cna-web-webgl2 \
  --target PrimitivesSample_cna_samples --parallel 2
```

The reproducible bundle is retained under
`cna-web-webgl2/samples/PrimitivesSample/` and contains `.html`, `.js` and `.wasm`; no `.data` is
expected because this sample has no content. The 2026-09-12 bundle is a Release build made with
Emscripten 6.0.9: HTML is 19,623 bytes, JavaScript 249,271 bytes and wasm 7,484,909 bytes. LLVM's
section reader reports no custom or debug sections, and the JavaScript contains neither
`SharedArrayBuffer` nor a pthread path. Local HTTP returned all three files with status 200; Chrome
152 rendered the full stars/ships/sun scene with WebGL 2. The complete browser capture is retained
as `evidence/primitives-cna-web-current.png`.

The earlier Chrome 151 canvas capture `evidence/primitives-cna-web-fixed-canvas.png` contained
1,087 non-black pixels and 187 grayscale values. A held Escape key stopped that render loop: its
WebGL draw-call counter rose from 33,864 to 34,104 before input, reached 34,112 while the exit was
processed, and remained exactly 34,112 afterward.
