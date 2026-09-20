# ShadowMapping — port notes

## Owner-reported visual correction — 2026-09-20

The previous `✅` was premature. The owner noticed dark stripes across the
character and black/red texels where XNA's shadow-map preview is cyan. The
previous full-frame tolerance score hid a localized, genuine defect: the port
was running with CNA's default **Reach** profile although both original
`.csproj` files select **HiDef**. In Reach, `RenderTarget2D` substituted an
8-bit `SurfaceFormat.Color` target for the requested 32-bit
`SurfaceFormat.Single`. The single-channel expansion test also used a Reach
device and only checked a white texel; it therefore never tested the actual
single-channel path or the cyan low-depth result.

The HiDef project setting is now declared as project metadata in
`src/Properties/AssemblyInfo.cpp` through CNA's general
`ProjectGraphicsProfileEXT`; the game logic is unchanged. CNA's EasyGL
compiled-effect row-order copy also preserved only RGBA8 before, discarding
the source target's 32-bit float depth precision. It now allocates the copy in
the source `SurfaceFormat` and invalidates that copy if the format changes.
Neither correction is a sample-specific rendering workaround. No change to
sharp-runtime or any original C# source/content was required.

Fresh native and WEBGL2 builds with at most four compiler jobs and the same
official HiDef XNBs show the corrected result. In the 128×128 map preview,
the five low-depth texels are now `(8–9,255,255)` in both native and browser,
matching XNA, rather than `(8–9,0,0)`. In a fixed 250×300 crop around the
character, agreement with XNA within eight channel levels rose from
67,345/75,000 (89.8%) to 74,904/75,000 (99.9%), and channel MAE fell from
4.5971 to 0.1242/255. Across the complete 800×480 native frame, agreement
rose from 376,340/384,000 (98.0%) to 383,904/384,000 (99.975%) within eight;
MAE fell from 0.9165 to 0.0406/255. No pixels were excluded. Real Chrome
passes the existing scene, shadow, movement, reset, HTTP and error gates; its
start frame now agrees with XNA at 380,078/384,000 (99.0%) within eight,
including the browser shell's one-pixel blue canvas border.

The new work captures are under `evidence/requal-20260920/fix-profile-native/`
and `fix-profile-web/`; the previous `cna-native-hidef/` and `cna-web-hidef/`
directories remain as before-fix evidence. The original HiDef capture remains
under `xna-hidef-clean-exit/`. The retained native executable reproduced the
work build's start frame byte-for-byte and exited through Escape. The final
local-gallery WEBGL2 bundle passed the real-Chrome gate independently, with
an identical start-frame hash and no runtime/HTTP errors; its evidence is in
`fix-profile-gallery/`.

The focused OPENGLES3 tests passed: project-profile default and explicit
override (1), genuine HiDef `Single` channel expansion including a low-depth
cyan texel plus Color identity (3), and exact float precision through the
compiled-effect flipped-source copy plus two existing render-target source
contracts (3). Their output is retained in
`evidence/requal-20260920/focused-tests.log`; rerun with
`scripts/run-focused-tests.sh` from the artifact root. Native and WEBGL2
products were copied into their retained artifact locations; the four web
assets, corrected screenshot and thumbnail were refreshed in the local gallery.
No push or prune was requested.

An exploratory run of the broader EasyGL compiled-effect draw suite reported
63 passing, one skipped and five failing tests. Four failures concern
`sampler3D` shader precision; the fifth exercises an ordinary vertex-sampled
`Texture2D`, not a render-target copy. Neither path uses the project-profile
declaration or the changed flipped-source allocation. They are separate CNA
issues, not evidence that this sample's shadow-map fix failed; the three
directly relevant compiled-effect tests above pass. The complete log is
`evidence/requal-20260920/easygl-draw-suite.log` in the artifact root.

## Previous requalification — 2026-09-20 (visual result superseded)

`SAMPLE-038` was re-audited against the entire byte-identical 32-file upstream
snapshot, the active `cna` (`95b7e14a2`) and `sharp-runtime` (`cb8fd7f8`), the
Windows and Xbox project files, both sample-owned content processors,
`DrawModel.fx`, and the complete C# and C++ game logic. The Windows project
selects **HiDef**, not Reach. The old reference harness embedded
`Windows.v4.0.Reach` and copied Windows/Reach content into its executable;
that made the historical 92.2% comparison a comparison of the wrong original
configuration. The harness now embeds `Windows.v4.0.HiDef`, requires the
official Windows/HiDef pipeline pass and copies its output. The unchanged
source builds and runs as Windows/HiDef; supplemental Windows/Reach and
Xbox/HiDef content builds pass too. Both engines' camera, character rotation,
reset and Escape input were captured on isolated displays, and Escape exits
cleanly. No original `.cs`, `.fx`, project or asset file was altered.

The sample's own `CustomEffectPipeline` builds `grid.fbx` (`Scale=2`) and
`dude.fbx` (`Scale=0.75`), both with `CustomEffect=DrawModel.fx`. Their effect
and texture dependencies produce 16 XNBs. All 16 port XNBs were replaced with
the exact official Windows/HiDef output and compared byte-for-byte; the two
matching SHA-256 manifests are under `evidence/requal-20260920/`. The
original `ShadowMapping.png`, `Game.ico` and Microsoft Permissive License
were restored byte-for-byte outside Content. The C++ game keeps the two
rendering passes, the 2048² `SurfaceFormat.Single`/`DepthFormat.Depth24`
target, both named effect
techniques, the camera-frustum light projection, preview, keyboard and gamepad
paths. It adds no sample input, shader, content or rendering workaround. No
new CNA or sharp-runtime source change, stub or intentional behavior deviation
was needed; the earlier general SpriteBatch single-channel expansion fix
remains in current CNA.

Fresh Release OPENGLES3 and non-threaded Release WEBGL2 builds used the active
checkouts, shared ccache and no more than four compiler jobs. The unchanged
XNA/HiDef and current native initial images cover all 384,000 pixels at
800×480: 376,340 pixels (98.0%) agree within eight channel levels, with zero
excluded pixels and 0.9165/255 channel MAE. The Chrome WEBGL2 initial image
agrees with the XNA/HiDef reference at 372,784 pixels (97.1%) within eight,
with 1.7134/255 channel MAE. The remaining visible differences are at model/shadow
boundaries, the preview and grid sampling; neither the geometry nor the two
shadow-map passes is missing. The old 92.2% figure below is retained as
historical Reach evidence, not the present acceptance result.

Real system Chrome passes scene, cast-shadow, white-not-red depth-preview,
character rotation, camera movement/reset, WebGL2 context, title and renderer
gates. The `.html`, `.js`, `.wasm` and `.data` assets all return HTTP 200; no
game-asset HTTP error, runtime exception, rejected promise or fatal console
message occurs. The favicon 404 in the test-only HTTP server is unrelated to
the game bundle. The WASM contains no `debug_info`, and the JavaScript has no
pthread/shared-memory marker. The byte-identical local-gallery copy is gated
separately under `evidence/requal-20260920/gallery-final/`.

The stable artifact root is
`/rv/tmp/samples/SAMPLE-038-ShadowMappingSample_4_0/`. Its exact upstream
snapshot, build/capture helpers, corrected Windows/HiDef executable and
official pipeline outputs, retained native executable/content, complete web
bundle and fresh evidence are respectively in `xna4-original/`, `scripts/`,
`xna4-build/`, `cna-native-opengles3/`, `cna-web-webgl2/` and
`evidence/requal-20260920/`. The fresh
`work-native-opengles3-20260920/` and `work-web-webgl2-20260920/` build trees
remain reusable; no artifact pruning was requested. The local gallery adds a
37th card on page 4 (12/12/12/1). Publication was not requested.

Reproduction (all paths explicit; no more than four compiler jobs):

```bash
root=/rv/tmp/samples/SAMPLE-038-ShadowMappingSample_4_0
samples=/rv/data/development/github.com/libcna/cna-samples
cna=/rv/data/development/github.com/libcna/cna
sharp=/rv/data/development/github.com/libcna/sharp-runtime
export CCACHE_DIR=/home/robertvokac/.cache/ccache CCACHE_BASEDIR=/rv
"$root/scripts/build-original.sh"
cmake -S "$samples" -B "$root/work-native-opengles3-20260920" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCNA_GRAPHICS_RENDERER=OPENGLES3 \
  -DCNA_SAMPLES_CNA_ROOT="$cna" -DCNA_SHARP_RUNTIME_ROOT="$sharp" \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
cmake --build "$root/work-native-opengles3-20260920" \
  --target ShadowMapping_cna_samples --parallel 4
/home/robertvokac/emsdk/upstream/emscripten/emcmake cmake \
  -S "$samples" -B "$root/work-web-webgl2-20260920" -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DCNA_GRAPHICS_RENDERER=WEBGL2 \
  -DCNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF \
  -DCNA_SAMPLES_CNA_ROOT="$cna" -DCNA_SHARP_RUNTIME_ROOT="$sharp" \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
cmake --build "$root/work-web-webgl2-20260920" \
  --target ShadowMapping_cna_samples --parallel 4
CNA_RUN_DIR="$root/work-native-opengles3-20260920" \
  CNA_EVIDENCE_SUBDIR=requal-20260920/cna-native-hidef \
  CNA_CAPTURE_DISPLAY=:219 "$root/scripts/capture-cna-native.sh"
CNA_WEB_PRODUCT_DIR=/rv/data/development/github.com/libcna/samples.libcna.com/ShadowMapping \
  CNA_EVIDENCE_SUBDIR=requal-20260920/gallery-final \
  CNA_WEB_DISPLAY=:246 "$root/scripts/capture-web.sh"
```

## Historical port record — August 2026

Upstream: `ShadowMappingSample_4_0` (SAMPLE-038). Ported whole — both shadow-map passes, the
floating-point render target, the compiled two-technique effect and every key binding. Nothing
was missing or stubbed in the historical port. Its later framework fix is
documented below.

## Content

Two models, both through the sample's own pipeline extension (`CustomEffectPipeline`, like
SAMPLE-032 and SAMPLE-034), built for all three targets in Debug. Each carries **two** processor
parameters that the content project sets and that the build is wrong without:

```xml
<Compile Include="dude.fbx">
  <Processor>CustomEffectModelProcessor</Processor>
  <ProcessorParameters_CustomEffect>DrawModel.fx</ProcessorParameters_CustomEffect>
  <ProcessorParameters_Scale>0.75</ProcessorParameters_Scale>
</Compile>
```

`DrawModel.fx` and texture dependencies are pulled in by that processor chain
rather than listed directly; the build produces sixteen XNBs from two source assets.

## What this sample exercises in CNA

The historical port exercised several CNA paths at once:

- a **`SurfaceFormat.Single` render target** with `DepthFormat.Depth24`, 2048×2048;
- a **compiled effect with two techniques**, switched per draw by name
  (`CreateShadowMap` / `DrawWithShadowMap`);
- that render target **bound back as an effect texture parameter** while the second pass reads it;
- `BoundingFrustum.GetCorners` and `BoundingBox.CreateFromPoints` driving the light's
  orthographic projection every frame;
- `SpriteBatch` drawing the float render target to the screen.

## Comparison against the original

The camera and the character's rotation both move by `time * k` per frame, so neither is
reproducible from a key press. The historical `CNA_ROTATE` diagnostic pinned
rotation in both engines (`scripts/compare-frozen.sh`, `cna-diag/README.md`).
Those old diagnostic products used Reach content and are not the acceptance
reference for the HiDef requalification above; its unchanged start frame and
real input runs were captured afresh.

**The scene matches.** One framework defect was found and fixed; one difference remains and is
not one.

### Framework defect found and fixed in `cnanext`: one- and two-channel formats did not expand

Direct3D 9 expands a texture's missing channels when a shader samples it — a one-channel format
arrives as `(R, 1, 1, 1)`, a two-channel one as `(R, G, 1, 1)`. OpenGL expands the same storage
to `(R, 0, 0, 1)` and `(R, G, 0, 1)`. So where the shadow map holds 1.0, the original's 128×128
preview read **(255, 255, 255)** and CNA's read **(255, 0, 0)** — a red square instead of a white
one, on a sample whose whole subject is that map.

CNA was FNA-faithful here (`FNA3D_Driver_OpenGL.c:378` maps `SurfaceFormat.Single` to
`GL_R32F`/`GL_RED` and swizzles nowhere), but this campaign's oracle is XNA, so CNA now matches
XNA.

**Why the fix is not `GL_TEXTURE_SWIZZLE`.** That parameter is exactly D3D9's rule and is core in
GL ES 3.0 and desktop GL 3.3, and it would have covered every sampling path at once. **WebGL 2
does not have it** — measured in a real browser rather than assumed: the constant is absent from
the context object and `texParameteri(TEXTURE_SWIZZLE_G, ONE)` raises `INVALID_ENUM`. Since this
campaign ships a native and a `WEBGL2` build of every sample, a swizzle-based fix would have made
the two disagree. The expansion is applied in the sprite fragment shader instead, from the bound
texture's own `SurfaceFormat`, where every profile does it identically.

Two things that fix cost, both worth keeping:

- **A uniform location belongs to the program it came from.** Caching the two locations from the
  sprite program broke `WeightedBlendedTransparencyTest`: a SpriteBatch drawn with a custom
  `ShaderEffect` runs that effect's OWN program, and handing it a foreign location is
  `GL_INVALID_OPERATION`, not a silent no-op — it surfaced as *"native GL errors were pending
  before MRT setup"* two passes later. The locations are now looked up per flush on the program
  actually in use, which also gives the right semantics: a custom effect samples the texture
  itself and has no such uniform, so nothing is written and its own sampling is left alone.
- **The expansion had to be proven identity for everything else.** SAMPLE-036's ten frames are
  **byte-identical** across the change, and a second test asserts a `Color` texture drawn the
  same way is untouched — a shader that expanded unconditionally would turn its green pixel
  white.

Both tests are in `modules/graphics/tests/.../SingleChannelExpansionTests.cpp`, and the first was
confirmed to fail with the fix reverted.

The boundary this leaves: a **custom** effect that samples a one- or two-channel texture still
sees GL's expansion, since CNA does not author that shader. `DrawModel.fx` reads the map's `.r`,
which is identical either way, so this sample is unaffected by it.

### Historical Reach-profile comparison and filtering hypothesis

The historical Reach-profile whole-frame agreement was ~92 % of pixels within
8 levels. The old audit attributed the remaining differences to minification
of the checkerboard floor, using this band breakdown:

| Band | Mean absolute difference | Within 8 levels |
|---|---|---|
| far third | 4.63 / 255 | 87.45 % |
| middle third | 4.58 / 255 | 86.03 % |
| **near third** | **1.95 / 255** | **94.95 %** |

Near the camera the floor's pixels are **exactly equal** — sampled at (255,380) and (255,430),
XNA and CNA both read (186, 98, 110) and (208, 208, 208). The difference appears only where the
checkerboard is minified. The current HiDef requalification shows that this
was not sufficient to identify the cause: the port was using Reach XNBs while
the original project selects HiDef, and correcting the content raised the
whole-frame agreement from 92.2% to 98.0%. The old band figures are retained
as historical measurements, not proof of an active filtering defect.

## `WEBGL2`

Built and driven in real Google Chrome (`scripts/capture-web.sh`). The gate asserts the floor
and character render, **the shadow is actually cast** (the second pass darkens the floor), the
shadow map is previewed, **that preview is white and not red** (the channel-expansion regression
pin), rotating the character moves the shadow, and the camera keys move the view. Walking two
steps forward fills the frame with floor and leaves almost no sky — the gate requires the clear
only on the frames that look at it.

The white-preview check exists because of a mistake worth recording: the channel expansion was
first verified natively only, and the WEBGL2 bundle shipped from a build made *before* the fix,
so the square was still red in the browser while the native frame was already white. The gate
checked that the preview was *present*, not what colour it was, so nothing caught it. **A
framework fix is not verified until both targets are rebuilt and re-captured**, and this sample's
gate now measures the pixel that distinguishes them.

That also settles the open question the earlier version of this file left: WebGL 2 has no
`GL_TEXTURE_SWIZZLE`, but the shader-side expansion reaches it fine — measured, not assumed.

## Deviations

None in behavior. Three C++ shapes worth naming:

- `spriteBatch->Begin(0, ...)` becomes `SpriteSortMode::Deferred`, and CNA takes the sampler,
  depth and rasterizer states by pointer, so the original's two nulls stay null.
- `GraphicsDevice.Textures[0] = null` becomes `getTexturesProperty()(0, nullptr)`; CNA's
  `TextureCollection` exposes assignment as a call operator, `operator[]` being read-only.
- `effect.Techniques[name]` returns a pointer in CNA, so it is passed to
  `setCurrentTechniqueProperty` directly rather than address-taken.
