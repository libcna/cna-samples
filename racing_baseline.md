# Racing Game Milestone 0 baseline

> **Post-close source correction (2026-09-02):** the executable measurements in
> this file remain evidence, but its modern GLB/material/FNA-asset content strategy
> is withdrawn. Racing must use original `.X` sources processed by XNA 4 into XNB;
> the modern repository is only a behavior oracle. See `plan_racing.md`.

## Result

Milestone 0 closed on 2026-09-02. The original FNA game is reproducibly runnable,
the two source snapshots are checksum-locked, the active CNA stack is pinned and
qualified on a real software OpenGL context, and the content risks are concrete.
No Racing gameplay C++ or asset conversion was introduced.

Artifact root:

```text
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master
```

## Exact baselines

| Component | Revision/version |
|---|---|
| modern Racing source | `d8092633e4e43e014ff168d8e913a9373538b851` (clean) |
| historical XNA 4 snapshot | `/rv/tmp/XNAGameStudio/Samples/XNA-4-Racing-Game-Kit-master` |
| CNA at Milestone 0 exit | `51d61ef42d1105d97387feeba11eae91a2f3e2e9` |
| sharp-runtimenext | `9cc96cd57cde394940cc24d58743edf9bf63d3fb` |
| easy-gl | `deda7a426c3c166c0e03a4790f1ede610e2e46fb` |
| meta-gl | `eaf07887c51108fe9f133a6f6a54aecbae5b8ba4` |
| FNA | `1358793096d556388be798d24dc3728b2abb10ff` |
| FNA3D | `32401479a3ab5bd6b2e7f786e87bf4166aa03b0f` |
| FAudio | `c54eb8f1223fe3b89e46ce929dbc3fd1a0601029` |
| SDL2-CS / SDL3-CS | `1eb20e5c690aee9a5188ba9cf06207295c51d935` / `9bdcf5de2af86bf764a9ca3c42821c1a099ef150` |
| Theorafile / dav1dfile | `3497e1a3cda1fa1ce79256f20e210fd08aec546f` / `a1377b49d6b69097357618bde87b0294e32dc313` |
| XNAssets | `7ae71d0c8adf974c1ca59579d23bc099dcb1386c` |
| DigitalRiseModel | `ff6b72f46110b09cb2b226a6ef314fa7dd534b02` |
| AssetManagementBase / DdsKtxSharp / Newtonsoft.Json | NuGet `0.7.1` / `1.1.0.1` / `13.0.4` |
| build tools | CMake 3.31.6, Ninja 1.12.1, GCC 14.2.0 |
| SDL / GL | SDL 3.4.0; Mesa 25.0.7 llvmpipe, OpenGL 4.5 core in CNA |

CNA is configured `Debug`, `CNA_GRAPHICS_RENDERER=OPENGL33`,
`CNA_EASYGL_COMPILED_EFFECTS=ON`. Builds in this session use no more than eight
parallel jobs, as required by the owner.

## Frozen sources and checksums

The artifact root retains complete copies:

- `modern-fna-original/` — 441 files;
- `xna4-original/` — 325 files.

`evidence/modern-original.sha256` and `evidence/xna4-original.sha256` cover every
retained file. Both manifests pass `sha256sum -c` in their respective roots; the
complete results are `evidence/modern-original-check.log` and
`evidence/xna4-original-check.log`.
`evidence/*-source-inventory.tsv` records path, size and hash in a reviewable form.

The modern `RacingGame/Assets` tree is 322 files and 301,389,495 bytes:

| Kind | Count |
|---|---:|
| GLB / `.material` | 57 / 57 |
| TGA / PNG / DDS | 129 / 11 / 2 |
| FNA + MonoGameDX `.efb` | 20 |
| WAV / XGS / XSB / XWB / XAP | 28 / 1 / 1 / 1 / 1 |
| track / combimodel / data | 3 / 10 / 1 |

The modern root contains `LICENSE.txt` and `Microsoft Permissive License.rtf`.
They are retained. This establishes the repository-level Ms-PL notice, but not a
per-asset origin ledger. Redistribution of the complete 301 MB asset set remains a
release gate; development may proceed against the local frozen evidence.

## Authentic FNA reference

The reference uses the exact revisions above, `dotnet` Debug/net8.0, FNA3D OpenGL,
FAudio and SDL dummy audio. Build logs are retained as:

```text
evidence/fna3d-configure.log
evidence/fna3d-build.log
evidence/faudio-configure.log
evidence/faudio-build.log
evidence/fna-dotnet-restore.log
evidence/fna-dotnet-build.log
```

The primary `/rv/tmp/RacingGame` checkout stayed clean. A disposable copy at
`/tmp/racing-fna-reference-clean/RacingGame` carries one Linux launch correction:
`PathUtils.cs` derives the executable directory directly from
`Assembly.GetExecutingAssembly().Location` instead of passing an ordinary Unix
path through `UriBuilder`. The reference output also receives case aliases for
asset directory/name spellings because the original source assumes a
case-insensitive filesystem. Neither change alters game behavior or canonical
assets, and both are confined to the disposable reference build.

Run/capture command:

```bash
RACING_XVFB_DISPLAY=152 \
  /rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/scripts/run-fna-reference-xvfb.sh
```

The script creates its own isolated Xvfb display, forces FNA3D OpenGL and llvmpipe,
drives only the Racing window, and tears it down. It captured eight 800x480 frames:
loading, splash, main menu, car selection, track selection, race start, a driven
race frame, and the returned track selection. The driven frame reached roughly
130 mph in gear 3 before returning through the normal screen stack and exiting.

`evidence/fna-reference/game.log` proves FNA3D OpenGL 4.5 compatibility profile and
MojoShader `glsl120`. It contains no exception. It does contain FNA debug diagnostics
that many game resources were not explicitly disposed; those are reference-game
lifetime warnings, not silently reclassified as CNA success.

## CNA qualification

The OPENGL33 tests ran against a real OpenGL 4.5 core llvmpipe context on isolated
Xvfb. XML evidence is under `evidence/cna-opengl33/`:

| Suite | Result |
|---|---:|
| Graphics | 2,284 passed, 58 skipped, 0 failed |
| Content | 1,587 passed, 4 skipped, 0 failed |
| Audio | 703 passed, 0 failed |
| Input | 500 passed, 0 failed |
| Renderer, ordinary option-off baseline | 296 passed, 0 failed |
| Renderer, compiled Effects enabled after `FX-128` | 330 passed, 0 failed |

The Integration binary reports zero discovered tests and is retained only as a
harness observation, not claimed as qualification. The focused pre-fix compiled
Effect test crashed under ASan in MojoShader `impl_SPIRV_LinkProgram`; after
`FX-128`, the shared contract plus targeted pixel-only test pass 2/2 under
ASan/UBSan. CNA commit `51d61ef42` contains the fix, regression and documentation.

The live renderer reports MSAA 4x, MRT 4, indexed colour masks, anisotropy 16x,
texture support for Color, NormalizedByte4/2, Bgr565, Bgra5551/4444 and DXT1/3/5,
and render targets Color/RGBA16F/RGBA32F. `Rgba64` is truthfully reported
unsupported and construction throws; Racing must not assume it works.

## Content gates measured against live CNA

### Compiled Effects — accepted decision

The ten FNA `.efb` files are authentic `fxc /T:fx_2_0` Effect Framework binaries.
A temporary CNA EasyGL oracle created every file and selected/applied every pass of
every technique on OPENGL33:

| Effect | Parameters | Techniques |
|---|---:|---:|
| NormalMapping | 19 | 9 |
| LightingShader | 10 | 6 |
| ShadowMap | 16 | 2 |
| ReflectionSimpleGlass | 14 | 1 |
| PostScreenMenu | 22 | 1 |
| LandscapeNormalMapping | 15 | 6 |
| PostScreenShadowBlur | 10 | 1 |
| LineRendering | 2 | 2 |
| PreScreenSkyCubeMapping | 5 | 1 |
| PostScreenGlow | 19 | 1 |

The new portable-Effect proposal is superseded. Racing will retain these exact
bytes and use CNA's public `Effect` collections/parameters/passes. Pixel, state,
texture and render-target parity remain Milestone 3 work; parse/apply is not a
visual fidelity claim.

### GLB/model — accepted direction, two bounded gates

All 57 files were passed through live `cna_tool_gltf_to_cnb`. Fifty-six convert.
`Cube.glb` is rejected correctly because accessor 3 declares a 24-element float
VEC3 (288 bytes) over a 192-byte bufferView. DigitalRiseModel reads through the
view length and the reference game renders it, but the sky shader derives its cube
coordinate from position and does not consume this malformed `TEXCOORD_0`.
Milestone 2 must produce a deterministic, documented normalization from the frozen
asset or otherwise repair the authoring source; CNA's generic bounds check stays.

`Car.glb` also reports the archived `KHR_materials_pbrSpecularGlossiness`
approximation and unsupported `FB_ngon_encoding`. Geometry converts, and Racing's
own `.material` sidecars drive effects, but the four-model proof must confirm these
diagnostics are irrelevant before completion.

The accepted model direction is the current CNA `Model`/glTF/CNB path plus a thin
Racing sidecar binding layer. Add no public model API until a proof asset
demonstrates information that the live path cannot preserve.

### DDS cubes — one works, one general gap

`SkyCubeMap.dds` imports as a 512x512, six-face, one-mip cube. The supplied
`NormalizeCubeMap.dds` is 128x128 uncompressed RGB888 and CNA refuses it honestly:
the loose cube decoder currently accepts DXT1/DXT3/DXT5 only. Milestone 2 owns a
general decoder/test fix. Re-encoding or replacing this asset only to bypass the
framework is prohibited.

### XACT — parser/playback route qualified, listening still open

`evidence/cna-opengl33/racing-xact-probe.log` records a successful public-runtime
probe: XGS 4 categories/8 variables, XWB 28 entries, XSB 27 cues/28 sounds; all 27
known cue names resolved. Representative UI, music, gear, brake, collision,
checkpoint and victory cues entered playback through SDL dummy audio, and the
long-lived Gear1 cue round-tripped Pitch `0.25`. This proves real parse and mixer
submission, not audible output. Listening and variable/RPC behavior remain
Milestone 8 gates.

## Milestone 1 handoff

Build the smallest OPENGL33 Game target against the pinned CNA commit. It must
prove lifecycle, deterministic clear/present/readback, resize/fullscreen/input,
backend/profile diagnostics, render-target sampling, a custom vertex layout and a
cube. Keep all UI/browser/host GUI activity on an isolated virtual display. Do not
load Racing content or translate gameplay until this harness is stable.
