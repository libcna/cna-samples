# SkinnedModelExtensions — port notes

Upstream: `SkinnedModelExtensions_4_0` (SAMPLE-055). The complete Part 2 extension of the
canonical Skinning Sample is now ported: named-bone lookup, direct head and arm overrides,
animated collision spheres, the rigid baseball bat attached to a hand bone, all original input,
camera and rendering behavior, and the sample-owned `SkinnedModel` and `Primitives3D` sources.

Artifact root: `/rv/tmp/samples/SAMPLE-055-SkinnedModelExtensions_4_0/`.

## Fresh source and project audit

The old note repeated DEFERRED #13's historical category error and called the sample blocked on
missing framework animation types. `AnimationClip`, `Keyframe`, `AnimationPlayer`, `SkinningData`
and `SkinnedSphere` are owned by this sample, not XNA framework APIs. The first four are the Part 1
library extended in Part 2; `SkinnedSphere` and the sphere primitive are additional game code.

Every C# game/runtime file, both Windows and Xbox projects, both content projects, the processor,
the XML sphere descriptions, source assets and the tutorial HTML were reviewed. Relative to Part
1, the exact functional changes are retained:

- `SkinningData` gains the processor-built `Dictionary<string, int> BoneIndices`;
- animation update is split into local, world and skin stages so the game can modify `Head` and
  `L_UpperArm` between them;
- `SkinnedSphere[]` is loaded from content and transformed by each named bone every frame;
- Enter or gamepad A toggles the original white wireframe debug spheres;
- `baseballbat` is a stock `Model` attached to `L_Index1` and drawn with `BasicEffect`;
- PageUp/PageDown rotate the head, Space raises the left arm, and every Part 1 camera/reset/exit
  input remains unchanged.

The upstream has Windows Reach and Xbox Reach projects and no Windows Phone game project. The
unchanged official source builds and runs under XNA 4.0. The content pipeline was also exercised
for Windows HiDef and Xbox Reach as additional checks.

## Exact official content

The unchanged `SkinnedModelProcessor` builds `dude`; the stock `ModelProcessor` builds
`baseballbat` with the project's `Scale=30` and `RotationX=120` parameters; and
`XmlImporter`/`PassThroughProcessor` builds `CollisionSpheres`. The seven checked-in runtime files
are byte-identical to the Windows Reach products:

| asset | SHA-256 |
|---|---|
| `CollisionSpheres.xnb` | `540580faecb72f8dc0982a8712c4b43a47111c16cb3481e921a58eb398d54d0b` |
| `baseballbat.xnb` | `25cd035734300485146c49b816179aa37b0d870fb21c728c17133b867ffa52f4` |
| `dude.xnb` | `909e8e9aaa3d2be42e9f63f80903a56e99570dfd19b94488971f37c8018944e0` |
| `head_0.xnb` | `baeaf3ad2fa90fe5c721e2fa1e563248bd446fe8e6456df8a6f93b78b83d2d74` |
| `jacket_0.xnb` | `bdbcf3f4e3b45ed137f5214496dc4c23864f14fc01853f586858a45925363d20` |
| `pants_0.xnb` | `847b874455caee44e5b5d761755a176591b7c67f875482f7bed9b5223d2b1e92` |
| `upBodyC_0.xnb` | `d11e1c66db4d1b716547bb18169c021bed375e6f3556ee02dcef39fc3e9a5b52` |

The reader tables were decoded before the port was written. `dude.xnb` has 17 readers: the stock
model/buffer/declaration/`SkinnedEffect` graph, the Part 1 reflective animation graph and closed
collections, plus `DictionaryReader<string, int>` for named bones. `baseballbat.xnb` uses the
stock model/string/buffer/declaration/index/`BasicEffect` graph. `CollisionSpheres.xnb` names
`ArrayReader<SkinnedSphere>`, `ReflectiveReader<SkinnedSphere>`, `StringReader` and `SingleReader`.
The port consumes those files directly; there is no converted model, generated sidecar, manual XML
parser or reconstructed runtime content.

## Framework repair found by the live asset

CNA already implemented generic `DictionaryReader<TKey,TValue>`, but its built-in registration
omitted the standard primitive-only `DictionaryReader<string, int>` shape. A real `.xnb` reader
table must resolve completely before any object can be decoded, so the unmodified `dude.xnb`
correctly failed instead of silently dropping `BoneIndices`.

`cnanext` commit `72262a33e` registers that stock shape in
`RegisterPrimitiveXnbReaders()`, updates the built-in-reader contract and adds two regression
checks: the umbrella registry contains the canonical name, and a reader created through that name
decodes real string dispatch indices and `int32` values. This is framework content infrastructure;
the game does not repeat the registration or parse the dictionary itself. Closed collections that
contain the sample's own types remain in the sample's AOT metadata, as documented in `diff.md`.

## Native XNA/CNA fidelity

The unchanged original and the Release OPENGLES3 port both run at the original 800x480 viewport
with the original `Skinned Model Extensions` title. Ordinary live captures prove that animation
advances, Enter adds the complete moving wireframe-sphere set, and PageDown+Space changes the head,
arm, attached bat and collision volumes. CNA and XNA show the same textured Dude, red bat and sphere
placement; Escape shuts down both.

For a stricter comparison, audit-only copies pin the clip to the same absolute time in both
engines. Each pair stays byte-identical for two seconds, proving the clock is actually frozen; the
two time legs differ, proving the comparison observes different animation states.

| pinned clip time | exact pixels | within 8 | within 16 | after 4 px blur | foreground coverage XNA / CNA |
|---|---:|---:|---:|---:|---:|
| 0.5 s | 94.76 % | **99.95 %** | 99.98 % | **100.00 %** | 10.585 % / 10.583 % |
| 0.9 s | 93.66 % | **99.92 %** | 99.98 % | **100.00 %** | 10.441 % / 10.441 % |

At 0.5 s both foreground bounds are exactly `329–497 x 48–479`; their centroids differ by 0.02 px.
At 0.9 s both bounds are exactly `327–484 x 56–479`; their centroids again differ by 0.02 px.
This covers the reflective `Model.Tag`, bone-name dictionary, animation stages, skinning, attached
stock model, textures, stock effects and draw path without averaging away a geometry error.

## Web

The complete Emscripten `WEBGL2` bundle builds and runs in the system Google Chrome. The browser
gate verifies an 800x480 WebGL 2 canvas, the `WEBGL2` renderer log, the original title, advancing
animation, Enter's collision-sphere toggle, PageDown+Space bone manipulation and Escape shutdown.
All `.html`, `.js`, `.wasm` and `.data` requests return successfully, with no promise rejection,
runtime exception, HTTP failure or fatal console message. The captured browser frames visibly show
the textured animated character, attached bat and moving wireframe spheres.

## Intentional C++ mappings

- C# arrays/lists/dictionaries map to `std::vector`/`std::unordered_map`; `CopyTo` becomes a vector
  assignment and indices are converted to `std::size_t` only at C++ container boundaries.
- Sample-owned serialized reference types derive `System::Object` and use `std::shared_ptr` so the
  generic reflective reader and `Model.Tag` retain their XNA reference semantics.
- Private parameterless C# deserializer constructors are public in C++, allowing the declared AOT
  reader to construct the same final types.
- C# reference fields for `Model` map to `std::optional<Model>` and owned helper objects to RAII;
  null checks that cannot occur through non-null C++ references are omitted.
- C# `foreach (SkinnedEffect/BasicEffect ...)` casts are checked `dynamic_cast`s that throw
  `InvalidCastException` on a mismatched effect, preserving the observable cast failure.
- `static void Main()` maps to `int main()`, and C# properties use CNA's established property
  getter/setter convention.
- The sample-owned AOT field declaration is the sole runtime addition; see `diff.md`.

## Verification and evidence

- Original: unchanged Windows Reach game/content build and live WineD3D run; Windows HiDef and
  Xbox Reach content builds also succeeded.
- CNA: Release OPENGLES3 target built and passed all ordinary controls and shutdown checks.
- Browser: Release WEBGL2 bundle built and passed the real-Chrome behavior/error gate.
- Framework: aggregate `CnaTests` built; both new focused tests pass under Xvfb.
- Artifact evidence: `scripts/`, `xna4-original/`, `xna4-build/`, `cna-diag/`, `xna4-diag/`,
  `evidence/{xna-original-final,cna-native-opengles3,cna-web-webgl2,frozen}/` and both complete CNA
  build trees under the artifact root.

There is no remaining SAMPLE-055 blocker or sample-side workaround. DEFERRED #13 remains only an
optional CNA-specific open-format/tooling question; this original XNB sample does not need it.
