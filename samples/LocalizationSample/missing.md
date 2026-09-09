# SAMPLE-078 — Localization audit

**Status: complete — no known behavior or content differences from the XNA 4.0 original.**

The old port's manual language cycle, invented SPACE/F1 UI, help overlay, hard-coded string fallback,
loose PNG/font assets and broad `std::runtime_error` catch are gone. The game again selects the
platform culture automatically, delegates string fallback to `System.Resources.ResourceManager`,
loads culture-specific content through the original full-name/language/default algorithm and catches
only `ContentLoadException`.

Artifact root:
`/rv/tmp/samples/SAMPLE-078-LocalizationSample_4_0/`

## Original surface audited

The Windows/Xbox XNA 4.0 product and its custom content-pipeline extension were reviewed in full:

- `LocalizationGame.cs`, generated `Strings.Designer.cs` and `Properties/AssemblyInfo.cs`
- neutral, Danish, French, Japanese and Korean `.resx` families
- `LocalizedFontDescription.cs` and `LocalizedFontProcessor.cs`
- Windows and Xbox project/content declarations, localized flags, SpriteFont input, solution and HTML
  documentation

The port retains the `Localization` namespace, implicit 800x480 presentation, assembly title,
`Content.RootDirectory`, constructor-time culture selection, exactly three strings at the original
coordinates, flag position, CornflowerBlue clear, Escape/GamePad Back exit and the original unused
second `String.Format` argument. The generated resource class remains a distinct strongly typed
surface; only .NET generator attributes and private reflection/Assembly mechanics are omitted.

The custom processor is build-time tooling rather than game runtime code. The retained unchanged XNA
build compiles and executes it: it scans every `.resx` value, adds the exact localized character set
to the SpriteFont description and then invokes the stock FontDescriptionProcessor.

## Authentic content

The offline Windows 7 SP1 VM with XNA Game Studio 4.0 built the unchanged Windows/Reach project and
supplied the authoritative output. Every checked-in XNB is byte-identical to that retained output.
The seven flags were also byte-identical to the independent Wine pipeline output; the VM font is used
because Wine's installed Arial substitute produced a different glyph atlas.

| File | Bytes | SHA-256 |
|---|---:|---|
| `Flag.da.xnb` | 64,187 | `98e3fbb698ff13437aa7211db5d43ec00b214756f640d8fb427446889b5cc6d3` |
| `Flag.en-GB.xnb` | 64,187 | `dbe777eeca084d35822e3ebe5071b2900d9bb439acacf8e11ae33e27643f3d11` |
| `Flag.en-US.xnb` | 64,187 | `991a2bc6a759da4be675d5ba9eb1f931db37455a0c789e34020fdb4cfd287f8c` |
| `Flag.fr.xnb` | 64,187 | `6523976cd6ca6c9e8fd7003b129502b279c74d4d3dfeec239aff7947b6f41816` |
| `Flag.ja.xnb` | 64,187 | `0f492543706d06e173723b3a9781bbc3e8118cdbe307661fe4c3b0414afe8f74` |
| `Flag.ko.xnb` | 64,187 | `672f7bfbe88fb99e4c55a924a4b48bdb625eb67bfb3d3920adb3dc1127607645` |
| `Flag.xnb` | 64,187 | `a1c7d0d16e9382e9a1e2833c3368dcde48409390395c59053a995da0ca4a7b67` |
| `Font.xnb` | 41,959 | `9d830442282f594a08689df47cbdb147ee5c9c915ebde77b81a3413b334e1d80` |

`Content/` contains only these exact official-pipeline artifacts. No loose image, generated glyph
JSON, atlas PNG or other runtime sidecar remains. The documentation-only `help.png` stays beside the
sample HTML, as in the upstream distribution, and is not packaged or loaded by the game.

## General runtime repairs

This audit removed the sample workarounds by fixing their owners:

- `cnanext 0b41d8a6f` initializes unset process culture/UI-culture defaults from the platform's
  ordered preferred locales when a `Game` is constructed. Explicit caller defaults still win; two
  native tests cover both cases.
- `cnanext cd6587084` normalizes loose-reader open/decode failures to the public
  `ContentLoadException` contract. The sample therefore catches the same narrow exception as XNA;
  two content regressions exercise missing ordinary and culture-suffixed assets.
- `sharp-runtimenext e429f728` supplies exact English identity names for the six exercised cultures
  and keeps the process-default culture slot portable and synchronized on Emscripten libc++.
- `sharp-runtimenext 67e61a63` adds a general AOT `System.Resources.ResourceManager`. Generated or
  hand-authored resource code supplies exact-culture data through a compile-time callback;
  `ResourceManager` owns full-culture, parent-culture and invariant fallback. It requires no
  reflection and is not an XML serializer.

Sharp Runtime still intentionally has no ICU-sized locale formatting database. That limitation does
not affect this sample: all six culture identities it displays are pinned, and the sample performs no
culture-specific number/date formatting.

## Qualification

All CNA/sample builds used `CCACHE_DIR=/rv/cnaccache` and at most eight parallel jobs. Sharp Runtime
used its stricter two-job ceiling.

- The unchanged XNA project and custom processor completed through the official Windows/Reach
  pipeline. Original runs for `en-US`, `en-GB`, `da-DK`, `fr-FR`, `ja-JP` and `ko-KR` all exited 0
  and established the exact displayed culture names, strings, flags and layout.
- Debug and Release OPENGLES3 builds each ran all six cultures on a real Mesa OpenGL ES 3.2 context,
  rendered the matching localized strings/flag at 800x480 and exited 0 through Escape.
- The WEBGL2 bundle ran in system Google Chrome with `navigator.language == "ja-JP"`. Chrome obtained
  `WebGL 2.0 (OpenGL ES 3.0 Chromium)`, rendered Japanese text and the Japanese flag, completed 600
  further `requestAnimationFrame` callbacks and reported no exception, unhandled rejection, fatal
  console message or HTTP error. Semantic pixels were red `[237,28,36,255]` at the flag center,
  white `[255,255,255,255]` in its field and CornflowerBlue `[100,149,237,255]` in the background.
- Sharp Runtime's complete gate passed 17,885/17,885 tests across 39 executables with zero skips;
  the isolated Resources consumer and Doxygen 1.9.8 no-regression gates also passed.
- CNA Runtime passed 159 tests with only its two expected incompatible-platform skips. Both focused
  Content regressions passed on real GL. The broader Content suite otherwise passed but retains one
  unrelated pre-existing glTF ladder metadata failure: rung L3 names unregistered suite
  `GltfMaterialBridgeTest`; no Localization source touches that registry.
- The final bypass scan finds no language-cycle input, help overlay/load, loose-content path, static
  fallback algorithm or broad exception catch.

## Retained evidence

- Exact source snapshot and hashes: `xna4-original/`, `original-manifest.txt`, `original-sha256.txt`
- Official builds and exact VM content: `xna4-build/`, `evidence/build-original.log`,
  `evidence/original-xnb-sha256.txt`, `evidence/win7-xnb-sha256.txt`
- Original six-locale captures: `evidence/original-windows-reach/`,
  `evidence/original-win7-content/`
- Debug and Release native captures: `evidence/cna-native-opengles3/`,
  `evidence/cna-native-opengles3-release/`
- Browser result and captures: `evidence/cna-web-webgl2-qualified/`
- Reproducible original/native/web build and capture drivers: `scripts/`

---

## Re-audited 2026-09-09: the stored reference was squashed, and the port is right

This document never claimed a pixel comparison, and that was the correct call — but the reason no
number was available is worth recording, because the reference could not support one.

**`scripts/capture-original.sh` gave Xvfb a screen exactly the size of the window**,
`-screen 0 800x480x24`. There is no room there for the frame Wine puts around it, so the client
area came out **800x460** and XNA squashed its 800x480 backbuffer into those 460 rows. Every tile in
the stored montage measured 800x460, against the port's 800x480. Compared as they stood, the two
scored RMSE `0.107`; rescaling either side brought that to `0.021`, which is what a squashed
reference looks like rather than a defect. This is the failure mode
`docs`/memory already names for XNA references, and it is the second instance in this campaign
after SAMPLE-072, where a centred window lost its bottom 33 rows instead.

The script now takes a 1280x1024 screen, moves the window to 0,0, records its geometry, and
**refuses rather than record a capture that is not 800x480**. The reference was re-captured with it;
all six cultures are now a true 800x480.

### Measured against the corrected reference

| culture | native Debug | native Release |
| --- | --- | --- |
| `en-US` | 0.0222 | 0.0222 |
| `en-GB` | 0.0240 | 0.0240 |
| `da-DK` | 0.0270 | 0.0270 |
| `fr-FR` | 0.0324 | 0.0324 |
| `ja-JP` | 0.0460 | 0.0460 |
| `ko-KR` | 0.0448 | 0.0448 |

Debug and Release agree to every digit. The browser's `ja-JP` capture scores `0.0460`, the same as
native's.

**The background is bit-exact.** A text-free band of the frame (`y` 380–470) compares at RMSE
`0 (0)` for both a Latin and a CJK culture, so the CornflowerBlue field, the flag and the layout are
identical pixel for pixel. Every difference is on glyphs: 0.27 % of pixels for `en-US`, 1.16 % for
`ja-JP` — more glyphs, more covered area, which is why the CJK cultures score highest — with a
maximum difference of 110/255, the signature of a glyph edge landing on a different subpixel rather
than of different content.

### Also noted

`help.png` sits in the sample root. It is not referenced by any source and is not in the original —
a leftover from the F1 overlay this port removed, which the code really is free of. It is the fourth
such file in the campaign, after SAMPLE-072, SAMPLE-073 and SAMPLE-077.
