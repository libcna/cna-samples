# ShatterEffect — SAMPLE-042 requalification

Requalified 2026-09-25 from the exact physical `ShatterEffectSample_4_0` directory. This is one
runnable XNA game with Windows and Xbox 360 Reach projects, not two different products. The
unchanged Windows Debug/x86 game and the sample's own `ShatterProcessor` built and ran through the
Microsoft XNA 4.0 pipeline. The port preserves the single original game class and its entry point,
`Content.Load<Model>("tank")`, the compiled shatter effect, camera, lighting, update/draw order,
Up/Down and A/B holds, Back/Escape exit, and the two instruction lines. It has no audio. The
original `.htm`, licence, icon and screenshot are retained outside `Content/`.

## Exact original and content

- Upstream: `/rv/tmp/XNAGameStudio/Samples/ShatterEffectSample_4_0/`.
- Verbatim snapshot: `/rv/tmp/samples/SAMPLE-042-ShatterEffectSample_4_0/xna4-original/`;
  `diff -qr` against upstream reports no differences.
- Original projects: `ShatterEffectWindows.csproj` and `ShatterEffectXbox.csproj`, both Reach;
  `ShatterEffectContent.contentproj` lists `ShatterEffect.fx`, `tank.fbx` through
  `ShatterProcessor`, and `font.spritefont`. The FBX material brings in two texture XNBs.
- The unchanged processor splits each indexed triangle into three independent vertices,
  calculates its `TriangleCenter` (`TEXCOORD1`) and a random `RotationalVelocity` (`TEXCOORD2`),
  then replaces the material with the official compiled `ShatterEffect.fx`. The model XNB's
  reader table includes `ModelReader`, vertex/index buffers and declarations,
  `EffectMaterialReader`, `DictionaryReader<string,object>`, `ExternalReferenceReader` and
  `TextureReader`. The shader uses five vertex channels and its original `vs_2_0`/`ps_2_0` pass.
  No HLSL-to-GLSL sample translation or sidecar shader exists.
- `scripts/build-original.sh` compiled the original processor with the XNA C# compiler, then
  built Windows Reach, Windows HiDef and Xbox 360 HiDef content with the real pipeline. It linked
  the unchanged Windows Debug/x86 game with the `Windows.v4.0.Reach` profile resource. The Xbox
  project shares that source; no Xbox console execution was claimed.

The selected official Windows Reach outputs in `samples/ShatterEffect/Content/`, the retained XNA
executable's `Content/`, the new diagnostic XNA executable's pinned `Content/`, and the native
products are byte-identical:

| XNB | SHA-256 |
|---|---|
| `ShatterEffect.xnb` | `daa552ea4ae34155eddc6c8a1bee88718a82121f63e1e918a68d2edd717d2f68` |
| `engine_diff_tex_0.xnb` | `6f3218e67b4f2466f273ae817068df78d78e97798b23153d3f86774c9be98027` |
| `font.xnb` | `a838a1ec9d7831a79b2582b235f7bebb4e6864a73079ccd4dab41ee6700de9f2` |
| `tank.xnb` | `b09fd85c0f11636f72d2ec43ca6649f9c8736f2ec852183ce741d22a150b6f19` |
| `turret_alt_diff_tex_0.xnb` | `77bd0f0910728d7d384b2887343237a95cef76d4a29a0f9e592534d6fb0a0b4b` |

`ShatterProcessor` uses unseeded `System.Random`, so a fresh official rebuild produces a different
`tank.xnb` (`96090832b3fde6458df1e108583fcec7adb271b9c2e8a25cc6a676afe7ad8e11` on this
run). That new file is retained as `evidence/requal-20260925/tank-official-rebuild.xnb`.
The other four fresh Reach XNBs match the pinned set bit for bit. Both engines were compared using
the **same** pinned original model; the fresh executable was not compared against a different
random geometry. The new XNA start image is byte-identical to the old retained reference.

## Runs and comparison

The original ran in isolated Xvfb under Wine with
`CNA_XNA40_WINEPREFIX=/home/robertvokac/.wine-cna-xna40` (the script default) and
`WINEDLLOVERRIDES=d3d9=b`. The new unchanged executable is
`xna4-build/bin-requal-20260925/ShatterEffect.exe`. XNA and native OPENGLES3 each rendered an
800×480 tank, accepted a short and a long Up hold, reversed it with Down, returned to the intact
model and closed their own window on Escape. Current native and original start captures match at
**98.8299% of pixels within eight RGB levels**, with mean absolute channel difference
**0.423/255** and **100% after a 4 px Gaussian blur**. Native start and fully reversed PNGs have
identical SHA-256 hashes.

For an exact time comparison, `xna4-diag/ShatterEffectGame.cs` and
`cna-diag/ShatterEffectGame.cpp.frozen` add only the same `CNA_TIME` assignment to the original
and port, respectively. The production source remains untouched. A tiny isolated copy of the
sample built the current CNA diagnostic, and the XNA diagnostic was relinked without regenerating
content. Repeating the 0.5 s leg made byte-identical XNA and native screenshots; the four legs
have distinct hashes. `scripts/compare-frozen-images.py` produced this fresh 800×480 result:

| Shatter time | Pixels within 8 RGB levels | After 4 px blur | Foreground pixels XNA / native |
|---|---:|---:|---:|
| 0.0 s | 98.8299% | 100.0000% | 75,622 / 75,204 |
| 0.5 s | 98.3888% | 100.0000% | 90,290 / 89,884 |
| 1.0 s | 98.1089% | 100.0000% | 97,465 / 97,119 |
| 2.0 s | 99.1701% | 99.9904% | 27,947 / 27,663 |

The old 2026-08-27 figures of 89–97% unblurred described an earlier CNA renderer; they do not
describe this requalification. Most remaining differences are boundaries of the independent
triangles. One first attempt at the 0 s XNA screenshot was shifted by 23 px because the window
moved between geometry lookup and screenshot. It is retained as
`frozen/xna/t0_0/xna-0_0-misaligned.png`; the capture scripts now re-read the exact window
geometry before every screenshot. The corrected 0 and 0.5 s XNA screenshots repeat bit-identically.

### Manual original launch on GNOME/Xwayland — follow-up 2026-09-25

The owner found that `wx ShatterEffect.exe` from the retained `xna4-build/bin` can briefly show
the window and then exit 1 with X11 `BadWindow` on `X_UnmapWindow` (resource `0x600001`). The
interactive `wx` alias already sets the correct XNA Wine prefix and `d3d9=b`; the failure was
reproduced on the host's `DISPLAY=:0` with both the retained and freshly rebuilt unchanged EXEs.
The retained EXE on isolated Xvfb showed the game and closed on Escape with status 0. The same
two EXEs on `:0` each showed the game and closed on Escape with status 0 when launched through
Wine's per-command virtual desktop. The controlled display comparison points to a Wine/Xwayland
window-management interaction; the exact underlying X11 cause is not established.

For interactive use from the owner's directory:

```bash
cd /rv/tmp/samples/SAMPLE-042-ShatterEffectSample_4_0/xna4-build/bin
wx explorer /desktop=ShatterEffect-042,1024x768 ShatterEffect.exe
```

`scripts/run-original.sh` applies the same verified launch without relying on the shell alias.
The original capture script now waits for and checks Wine's actual exit status when
`CNA_VERIFY_EXIT=1`; the updated script passed again on Xvfb. Diagnostic output is in
`evidence/requal-20260925/manual-launch-followup.txt`, its referenced logs, and
`verified-capture-exit/`. No game, CNA or sharp-runtime source changed for this host-specific
launch issue.

Release WEBGL2 was built without Emscripten threads and served over local HTTP to system Google
Chrome. The work build and the byte-identical local gallery copy each pass the same real-browser
gate: WebGL2 initializes, the tank renders, held Up progressively scatters and drops it, held Down
reverses it, and the final PNG hash exactly equals the start. The below-midline pixel share moves
`0.329 → 0.579 → 0.957 → 0.407 → 0.329`. HTML, JS, WASM and data all return HTTP 200;
there are no relevant HTTP failures, runtime exceptions, rejected promises or fatal console
messages. The gallery copy retains the same tested `.html`, `.js`, `.wasm`, `.data` byte hashes.
The WASM has no `debug_info`; the JS has no pthread/shared-memory marker. The browser start frame
agrees with XNA at 98.62% and with native at 99.82% within eight RGB levels after dropping a 3 px
canvas border/focus-ring inset. The local gallery has 41 cards on 12/12/12/5 pages. It is prepared
but not pushed or deployed.

The browser gate was calibrated by suppressing its Up key-down event in a disposable copy of the
CDP script. That run exited nonzero and recorded `modelShatters=false` and
`shatterIsProgressive=false`; the normal work and gallery runs pass both. The local gallery card,
detail, two images and four bundle files each returned HTTP 200 from a gallery-root server, and
both adjacent detail-page navigation links were checked.

## Layer ownership and differences

There is no known behavior, visual, input or content difference needing an exception, and no
sample-side renderer, model or shader workaround. No CNA or sharp-runtime source change was needed
and therefore no dependency regression test was added. The C++ representation uses RAII/reference
mechanics (`std::optional` for reference-type content fields, a vector for the bone-transform
array, and CNA's pointer-valued effect parameter access). `GetTypeName()` supplies the original
logical name. These preserve the C# behavior.

The shared `../cna` checkout was on a separate `street-perf` task, so this audit did not switch or
edit it. `CNA_SAMPLES_CNA_ROOT` selected the isolated
`/rv/tmp/samples/SAMPLE-042-ShatterEffectSample_4_0/cna-next-source` at CNA `next`
`009d40f5dd085c4e674d3479675fac84b12b3e0a`; sharp-runtime `next` was
`41b918c97ed47288f87a0af176fe23942af24cdd`. The local EasyGL and MetaGL dependency copies
were `deda7a426c3c166c0e03a4790f1ede610e2e46fb` and
`20c8b2dc5bb80e32706784066db9fd9e15b3f46a`. The vendored SDL/Draco working files were
copied from the shared checkout at the identical pinned submodule revisions.

## Reproduction and retained artifacts

Artifact root: `/rv/tmp/samples/SAMPLE-042-ShatterEffectSample_4_0/`.

| Path | Purpose |
|---|---|
| `xna4-original/` | Exact full physical upstream directory, including solutions, documentation and licence. |
| `xna4-build/bin-requal-20260925/` | Fresh unchanged Windows Debug/x86 XNA executable with the pinned original content. |
| `xna4-build/bin-diag-requal-20260925/` | Fresh `CNA_TIME` diagnostic XNA executable. |
| `work-native-opengles3-20260925/` | Current Release OPENGLES3 work build and content. |
| `work-native-static-20260925/` | Same Release renderer and source, with `CNA_SHARED_LIBRARY=OFF` for the retained executable. |
| `work-native-frozen-20260925/` | Current Release frozen-time native diagnostic. |
| `work-web-webgl2-20260925/` | Current Release nonthreaded WEBGL2 work build. |
| `cna-native-opengles3/samples/ShatterEffect/` | Retained native product with its content and runtime libraries. |
| `cna-web-webgl2/samples/ShatterEffect/` | Retained complete `.html`, `.js`, `.wasm`, `.data` bundle. |
| `evidence/requal-20260925/` | New logs, captures, exact-time metrics, content and bundle hashes. |
| `scripts/` | Build/capture helpers; `compare-frozen-images.py` calculates the table above. |

The retained native executable is stripped (6.5 MB), links CNA statically and has `$ORIGIN` first
in its runtime library path. Its two required SDL shared libraries are beside it; `ldd` resolves
both from that directory. Thus it does not need the isolated CNA build tree to remain in place.
The retained product's first PNG is byte-identical to the shared-library work build's first PNG,
and its Up/Down and clean Escape gate passed separately. The retained WEBGL2 bundle is
byte-identical to both the tested work and gallery copies.

The original build was run as:

```bash
root=/rv/tmp/samples/SAMPLE-042-ShatterEffectSample_4_0
CNA_XNA_BIN_DIR="$root/xna4-build/bin-requal-20260925" \
  CNA_BUILD_DISPLAY=:217 "$root/scripts/build-original.sh"
# For live comparison, pin all five retained official XNBs in that new bin/Content first.
CNA_XNA_BIN_DIR="$root/xna4-build/bin-requal-20260925" \
  CNA_EVIDENCE_SUBDIR=requal-20260925/xna-original CNA_CAPTURE_DISPLAY=:218 \
  "$root/scripts/capture-original.sh"
```

Native and web used `CCACHE_DIR=~/.cache/ccache`, `CCACHE_BASEDIR=/rv`,
`-DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache`,
`-DCNA_SAMPLES_ONLY=ShatterEffect`,
`-DCNA_SAMPLES_CNA_ROOT="$root/cna-next-source"`, `-DCMAKE_BUILD_TYPE=Release`, and
`cmake --build ... --target ShatterEffect_cna_samples --parallel 4`.
The native renderer is `OPENGLES3`; web is `WEBGL2` with
`-DCNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF`. The exact work commands were:

```bash
export CCACHE_DIR="$HOME/.cache/ccache" CCACHE_BASEDIR=/rv
samples=/rv/data/development/github.com/libcna/cna-samples
cmake -S "$samples" -B "$root/work-native-opengles3-20260925" \
  -DCMAKE_BUILD_TYPE=Release -DCNA_SAMPLES_ONLY=ShatterEffect \
  -DCNA_SAMPLES_CNA_ROOT="$root/cna-next-source" \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
cmake --build "$root/work-native-opengles3-20260925" \
  --target ShatterEffect_cna_samples --parallel 4
/home/robertvokac/emsdk/upstream/emscripten/emcmake cmake \
  -S "$samples" -B "$root/work-web-webgl2-20260925" \
  -DCMAKE_BUILD_TYPE=Release -DCNA_SAMPLES_ONLY=ShatterEffect \
  -DCNA_SAMPLES_CNA_ROOT="$root/cna-next-source" \
  -DCNA_SAMPLES_ENABLE_EMSCRIPTEN_THREADS=OFF \
  -DCMAKE_CXX_COMPILER_LAUNCHER=ccache -DCMAKE_C_COMPILER_LAUNCHER=ccache
cmake --build "$root/work-web-webgl2-20260925" \
  --target ShatterEffect_cna_samples --parallel 4
```

The retained native variant used the same native command with
`-DCNA_SHARED_LIBRARY=OFF '-DCMAKE_BUILD_RPATH=$ORIGIN'` and
`-B "$root/work-native-static-20260925"`. The exact outputs and first failed configure attempts
are retained in `evidence/requal-20260925/`. The retained browser bundle is suitable for static
hosting. No push or artifact-root prune was requested. The guarded prune **dry run only**
proposed six reproducible paths and 578.5 MB to free from a 1.3 GB root; it leaves the
530.1 MB isolated CNA source checkout and other unfamiliar top-level source copies in place.
`evidence/requal-20260925/prune-dry-run.txt` records the exact list. The owner must separately
authorize any `--apply` or further cleanup after manual review.
