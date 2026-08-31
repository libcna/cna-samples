# SAMPLE-066 — ShipGame_4_0 audit

## Status: 🛑 owner decision — `System.Xml.Serialization`

ShipGame is a runnable 1280x720 Windows/HiDef game, not a shader placeholder. The exact original
contains 28 game source units, ten `BoxCollider` library units and a two-unit custom
`NormalMappingModelProcessor` project (**40 C# files / 10,094 lines**). It implements the complete
single-player and split-screen game, ship and level selection, collision, projectiles, powerups,
particles, glow post-processing, normal-mapped models and XACT audio.

A faithful C++ port is not started because its live gameplay path requires the same missing .NET
subsystem already established by `SAMPLE-014` Spacewar:

```csharp
XmlSerializer serializer = new XmlSerializer(typeof(EntityList));
EntityList entities = (EntityList)serializer.Deserialize(stream);

XmlSerializer serializer = new XmlSerializer(typeof(LightList));
LightList lights = (LightList)serializer.Deserialize(stream);
```

`EntityList.cs` and `Graphics/LightList.cs` each implement both `Save` and `Load` through
`System.Xml.Serialization.XmlSerializer`. Three `EntityList.Load` and three `LightList.Load` call
sites supply ship collision entities, level spawns and powerups, level lights, player-screen lights
and end-screen lights. The unchanged original was driven through intro, ship selection and level
selection into live gameplay, proving these calls are required rather than inactive utility code.

`sharp-runtimenext` has comprehensive `System.Xml` and `System.Xml.Linq` modules but no
`System.Xml.Serialization` namespace or `XmlSerializer`. Replacing the calls with a handwritten
sample parser would repeat the workaround that reopened `SAMPLE-014`, and is prohibited by
`rules.md`. C++ has no automatic runtime reflection; the faithful reusable solution needs a
type/member declaration mechanism and a generic serializer in Sharp Runtime, analogous to the
registered reflective XNB path. That is a new runtime subsystem, and the owner already chose
"mark it and decide later" for this exact issue on 2026-08-28. No sample workaround or speculative
Sharp Runtime implementation was added here. Tracked by `SAMPLES-DEC-008`.

## Historical shader and audio claims are disproved

The old placeholder claimed these four original HLSL effects had to be rewritten as GLSL and
described by sample-local sidecars:

- `shaders/AnimSprite.fx`
- `shaders/Blur.fx`
- `shaders/NormalMapping.fx`
- `shaders/Particle.fx`

That claim is false on the live CNA stack. The unchanged XNA 4.0 pipeline compiled all four to
normal Effect XNBs, and CNA's shared compiled-effect route is the required runtime path. Their
retained output hashes are:

| Effect | SHA-256 |
|---|---|
| `AnimSprite.xnb` | `8e91c6d75a5823ba93da1b6c6fed3ba9be066e70516d2a990f3565a5cc9cd557` |
| `Blur.xnb` | `b0e6ce1ff25f7122219cf8480c696668749b0270c58abfd687040df17c4e1d08` |
| `NormalMapping.xnb` | `b80e68ae2310663b6a9e93e38b7c9dda4013781affedb83c55aebc655f822715` |
| `Particle.xnb` | `86e41bc7acab0b3ea87c8f96a06f2d1929ebb7da0aa5e1cc2e87a1632ad0f0a6` |

The source's old `PointList` draw is commented out; the active XNA 4.0 Windows source uses
`PrimitiveType.LineList`, so a new point-sprite API is not a demonstrated blocker.

The content project has 72 compiled items and ten verbatim copied XML files. An exact
Windows/HiDef build now follows XNA's own task split: 71 ordinary items pass `BuildContent`, while
`sounds/sounds.xap` passes the dedicated `BuildXact` task. The result is 159 XNBs plus the authentic
`sounds.xgs`, `Sound Bank.xsb` and `Wave Bank.xwb` (162 compiled runtime files / 87,200,183 bytes).
The custom normal-mapping processor successfully processes every ship, projectile, powerup and
level model. Representative XACT hashes are:

| Asset | SHA-256 |
|---|---|
| `Sound Bank.xsb` | `261897d7e302c67c6a68c71fa566e5666c645c1c79e4ba17325b4c08859058a2` |
| `Wave Bank.xwb` | `c76d27d22f58f82084eb13d4fc2e5230be4b872625866c1c75f7577c994b356f` |

No MonoGame-derived content or source was used as authority.

## Original qualification and retained evidence

The unchanged Windows/x86 source builds against the local XNA 4.0 assemblies. The retained
`ShipGameWindows.exe` has SHA-256
`16bd9fa04adaf922e35a354d7ebea8426d331838f63936fcfea91dbe780bfb78`.
It ran under the isolated XNA Wine prefix with WineD3D/software GL and no source modification.
Automated original inputs exercised intro -> ship selection -> level selection -> gameplay; the
process remained alive and rendered the original normal mapping, glow, HUD and level. The gameplay
capture has SHA-256
`8872116aa7a9ecca1f2c45f62e21fe19bbb91cf118a2615ec424fdba8e41d7bd`.

All snapshots, exact pipeline products, build/capture scripts, logs and screenshots are retained
unpruned at:

`/rv/tmp/samples/SAMPLE-066-ShipGame_4_0/`

There is no CNA sample target to qualify yet. Native OPENGLES3 and real-Chrome WEBGL2 gates wait
for the owner-approved `XmlSerializer` direction and the subsequent complete source port.
