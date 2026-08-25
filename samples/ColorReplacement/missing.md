# ColorReplacement — SAMPLE-028 audit record

Upstream: `ColorReplacementSample_4_0`, ported against the unchanged XNA 4.0 sources
snapshotted at `/rv/tmp/samples/SAMPLE-028-ColorReplacementSample_4_0/xna4-original`,
per-file SHA-256 in `evidence/xna4-original-sha256.txt`.

## 1. The old verdict, and what it turned out to be

The previous record said this sample was **not ported at all**, blocked by its custom
shader:

> `ReplaceColor.fx` HLSL shader must be rewritten as GLSL `.shader.json` (DEFERRED.md item
> #11) — no tooling exists yet for this conversion.
> There is no meaningful reduced-scope port: the colour-replacement effect *is* the entire
> point of this sample.

The second half is correct — a `BasicEffect` substitute would demonstrate nothing. The
first half is **stale**. It describes CNA's `.shader.json` route, where a game supplies
hand-written GLSL. Since then `plans/plan_fx.md` built the other route: CNA loads the
**compiled XNA Effect Framework bytecode the official `EffectProcessor` produces**, through
MojoShader. `docs/fx-compiled-effects.md` §10 lists the EasyGL family — including `WEBGL2`
— as supporting it behind `CNA_EASYGL_COMPILED_EFFECTS`, off by default only because
MojoShader is a fetched dependency EasyGL does not otherwise need.

So the sample is ported, with its own unmodified `ReplaceColor.fx` compiled by the official
pipeline. Nothing was rewritten by hand.

## 2. Framework work this sample required

Getting there needed four fixes, none of which was "add `.fx` support" — that already
existed and had never been reached through a Model.

**Three missing `.xnb` type readers** (`cnanext`, `modules/content`). The `Car.xnb` the
official `ModelProcessor` produces declares ten type readers; CNA registered seven. The
list was read out of the file itself rather than guessed:

| Missing reader | What needs it |
|---|---|
| `EffectMaterialReader` | the material of a mesh part whose `.x` material names an `.fx` |
| ``DictionaryReader`2[[System.String],[System.Object]]`` | that material's parameter table |
| `ExternalReferenceReader` | the `DiffuseTexture` value inside that table |

`EffectMaterial` and `ContentReader::ReadObject()` (returning `std::any`) already existed,
so this is the three readers and one explicit `ReadExternalReference<std::shared_ptr<Effect>>`
instantiation. `ExternalReferenceReader` loads its target as a `Texture2D`, the only asset
kind an XNA effect-parameter table holds a reference to; anything else fails loudly through
the existing type check rather than being mis-loaded. Documented in the reader's own header.

**`protected Effect(Effect cloneSource)`** (`cnanext`, `modules/graphics`). XNA has this
constructor and CNA did not — its copy constructor was `= delete`. `EffectMaterial` was
therefore built on `Effect(GraphicsDevice&)`, which keeps nothing of the source, so a
material cloned from a compiled effect had **zero parameters**: `effect.Parameters["World"]`
returned null and the sample dereferenced it. The new constructor clones the renderer
runtime, rebuilds the object graph and carries the source's mutable parameter values and
selected technique across, sharing one helper with the existing private clone constructor.
After the fix the material exposes all seven parameters `ReplaceColor.fx` declares.

`EffectMaterial` had a test file before this. It asserted that `Clone()` returns an
independent `EffectMaterial` with the right type name — and never that the clone carried
anything, which is why the stub looked finished.

**`Single`/`Double::ToString("0.000")`** (`sharp-runtimenext`). The HUD prints each channel
with that format. `"0.000"` is a *custom* numeric format string; sharp-runtime implemented
only the standard specifiers and threw `FormatException` for anything else. The custom
grammar is now implemented for the `0`/`#` placeholders, the decimal point and the group
separator, with unimplemented constructs (`;` sections, `%`, custom exponent forms,
escaping) refused explicitly rather than mis-emitted.

It was verified **differentially against the reference implementation** rather than against
expectations: 28 value/format pairs run through both mono and CNA, 28/28 identical,
including midpoint-away-from-zero (`2.5 → "3"`) and the unsigned near-zero case
(`-0.4 → "0"`). That measurement also overturned three pinned sharp-runtime tests which
asserted `"Fx"`/`"Fz"` throw `FormatException`: .NET reads them as custom formats of two
literal characters and returns `"Fx"`/`"Fz"`. Those tests are corrected, with the
measurement cited.

## 3. Content

Three assets and one implicit fourth. `Car.x` goes through `XImporter`/`ModelProcessor`,
which also builds the `Car.tga` its material references as `Car_0.xnb`; `ReplaceColor.fx`
through `EffectImporter`/`EffectProcessor`; `SpriteFont.spritefont` through
`FontDescriptionProcessor`. All **4** XNBs are byte-identical to this sample's own official
pipeline output (`cmp`), hashes in `evidence/content-sha256.txt`.

Building the model and effect needed two pipeline assemblies no earlier sample in this
campaign had used — `Content.Pipeline.XImporter.dll` and `Content.Pipeline.EffectImporter.dll`.

**There is no WindowsPhone target.** The sample ships only Windows and Xbox solutions, and
the official pipeline refuses the phone build outright: *"The Windows Phone platform does
not support custom shaders."* That refusal is the measurement; the phone target is removed
from this sample's pipeline runner rather than worked around.

## 4. C++ mapping notes — not deviations

- `Model` is loaded as `Load<Model>`, not `Load<std::shared_ptr<Model>>`: CNA's
  `ModelReader` targets `Model` by value. The field is `std::optional<Model>` because XNA's
  is null until `LoadContent` runs.
- `Properties/AssemblyInfo.cs` is ported as `src/Properties/AssemblyInfo.cpp`. Its
  `AssemblyTitle` is **`"Color Replacement"`**, with a space — both windows are titled that,
  not `ColorReplacement`.
- `MathHelper::Clamp(value, 0, 1)` is passed float literals; in C++ the int/float overload
  pair is ambiguous where C# picks the float one.

## 5. Verification

Both builds are EasyGL — native `OPENGLES3` and web `WEBGL2` under Emscripten, both
configured with `-DCNA_EASYGL_COMPILED_EFFECTS=ON`. MojoShader builds under the Emscripten
toolchain, which had not been demonstrated in this campaign before.

The car spins on `TotalGameTime`, so no frame is reproducible. What is deterministic is the
HUD: each channel moves by `ColorChangeRate` per frame while its letter key and an arrow are
both held, and `MathHelper.Clamp` pins it at 0 or 1, so holding for several seconds reaches
a value that does not depend on how many frames elapsed. All three builds were driven
through the same sequence — start, `R`+`Up`, `G`+`Down`, `B`+`Up`.

**The four overlay lines are drawn after the model, so nothing occludes them.** Their opaque
glyph masks were hashed identically in all three builds: **48 of 48 comparisons** (4 frames
× 4 lines × XNA / native / WebGL2) are byte-identical, at 1074 / 447 / 461 / 452 pixels and
the values each state produces. That also pins `ToString("0.000")` against the original's
own output.

The effect itself is measured by the mean colour of the lit body pixels, which does not
depend on where the car is pointing (`evidence/comparison.txt`):

| Frame | TargetColor | XNA mean RGB | CNA native mean RGB | max channel delta |
|---|---|---|---|---|
| start | (0, 1, 0) | (41, 117, 42) | (34, 113, 35) | 7 |
| red-max | (1, 1, 0) | (208, 124, 40) | (207, 123, 38) | 2 |
| green-min | (1, 0, 0) | (192, 46, 46) | (192, 37, 37) | 9 |
| blue-max | (1, 0, 1) | (167, 43, 169) | (167, 36, 169) | 7 |

Two entirely different graphics stacks — WineD3D over Direct3D 9, and MojoShader-generated
GLSL ES 3 — agree to within 2 to 9 of 255 per channel on a moving model.

The browser gate (`scripts/capture-web.sh`, `scripts/chrome-smoke.mjs`) drives the same
sequence in real Google Chrome over local HTTP and asserts the body mean's channel ordering
follows `TargetColor` at every step, `document.title` is `Color Replacement`, an 800×480
WebGL2 canvas, `CNA: graphics renderer: WEBGL2`, and no rejections, runtime exceptions, HTTP
errors or fatal console messages. Full record in
`evidence/cna-web-webgl2/browser-result.json`.

## 6. Deviations

None. The sample is ported whole, with its own compiled shader, and nothing about the
original's behaviour is left unreproduced.

## 7. What this means for the other samples blamed on `.fx`

**Twenty-two** ported samples' `missing.md` files currently blame a custom shader for
missing behaviour, and **thirty** upstream sample directories ship `.fx` files. The reason
those records give is the one this sample just disproved. They are not thereby unblocked --
`CustomModelEffect`, for one, needs custom content *processors* at build time, which
compiled-effect support does not address, and `docs/fx-compiled-effects.md` lists real
remaining limits (`Texture3D`/`TextureCube` on a compiled sampler slot, vertex-stage texture
sampling, effect recreation after context loss). Each has to be retested on its own
evidence. `DEFERRED.md` item #11 needs rewriting against this measurement.
