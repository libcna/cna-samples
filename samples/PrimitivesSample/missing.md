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
- The former F1 help overlay and `Content/help.png` were removed. They were cna-samples additions,
  not part of the original sample or either original C# file.
- The audit found no `RawMesh`/`RawModel`, backend access, direct content substitute, sidecar data,
  invented input, simplified branch or other CNA bypass. This sample has no runtime content assets.
- `System::Random` and `System::Collections::Generic::List` come from `sharp-runtimenext`.
  `std::vector<VertexPositionColor>` is the backing representation used by SharpRuntime's
  `System::Array` API, while `std::unique_ptr` is the C++ ownership equivalent of the original
  managed lifetime. No SharpRuntime gap was found. The default random constructor deliberately
  gives a non-deterministic star field, so individual star positions are not expected to match
  between separate launches.

## Original XNA 4.0 reference

The unchanged original sources retained in `xna4-original/` were rebuilt with the installed .NET 4
compiler and XNA 4.0 GAC assemblies in Wine prefix
`/home/robertvokac/.wine-cna-xna40`. The resulting Windows executable is
`xna4-original/Primitives/bin/x86/Release/Primitives-audit.exe`. It was run through WineD3D in
Xvfb. The working capture `evidence/primitives-xna-original-wined3d.png` is 853x480 and contains
1,144 non-black pixels, showing the expected random stars, two ships and central sun.

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

## Native CNA verification

```bash
cmake -S . \
  -B /rv/tmp/samples/SAMPLE-001-PrimitivesSample_4_0/cna-native-opengles3 \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug -DCNA_GRAPHICS_RENDERER=OPENGLES3
cmake --build \
  /rv/tmp/samples/SAMPLE-001-PrimitivesSample_4_0/cna-native-opengles3 \
  --target PrimitivesSample_cna_samples --parallel 8
```

The target configures against `../cnanext` and `../sharp-runtimenext`, builds and runs. Its captured
853x480 client image `evidence/primitives-cna-native-fixed.png` contained 1,169 non-black pixels
and matched the original scene structure; the differing random star layout is expected. The native
executable is retained at
`cna-native-opengles3/samples/PrimitivesSample/PrimitivesSample_cna_samples`.

## Browser verification

```bash
/home/robertvokac/Downloads/emsdk/upstream/emscripten/emcmake cmake \
  -S . \
  -B /rv/tmp/samples/SAMPLE-001-PrimitivesSample_4_0/cna-web-webgl2 \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCNA_GRAPHICS_RENDERER=WEBGL2
cmake --build \
  /rv/tmp/samples/SAMPLE-001-PrimitivesSample_4_0/cna-web-webgl2 \
  --target PrimitivesSample_cna_samples --parallel 8
```

The reproducible bundle is retained under
`cna-web-webgl2/samples/PrimitivesSample/` and contains `.html`, `.js` and `.wasm`; no `.data` is
expected because this sample has no content. It was served over local HTTP and exercised in Chrome
151 with WebGL 2. The 853x480 canvas capture `evidence/primitives-cna-web-fixed-canvas.png`
contained 1,087 non-black pixels and 187 grayscale values, and showed the full stars/ships/sun
scene without application console errors. A held Escape key stopped the render loop: a WebGL
draw-call counter rose from 33,864 to 34,104 before input, reached 34,112 while the exit was
processed, and remained exactly 34,112 afterward.
