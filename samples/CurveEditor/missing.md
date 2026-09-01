# Missing / Differences from XNA 4.0 original

**Status: freshly audited and owner-decision blocked. No C++ port, fake game, reduced CLI or
alternate UI has been started.** The upstream product is a three-project Windows Forms curve
authoring package, not an XNA `Game`. Its reusable XNA `Curve` mathematics and runtime content
routes are already present and tested in CNA; the unported product is the full graphical editor,
reusable WinForms control and example desktop application.

Source: `/rv/tmp/XNAGameStudio/Samples/CurveEditor_4_0/`.

Retained audit root: `/rv/tmp/samples/SAMPLE-093-CurveEditor_4_0/`.

## Audited original

The exact 79-file snapshot contains three products and 6,868 C# lines in total:

| Project | C# lines | Product |
|---|---:|---|
| `CurveControl` | 5,421 | reusable WinForms display/editing component and command model |
| `CurveEditor` | 1,115 | standalone Windows desktop authoring utility |
| `CurveControlUsageSample` | 332 | separate WinForms application demonstrating editable and read-only controls |

The package has no `Game`, `Game1`, graphics device, game loop, content project or browser entry
point. The supplied documentation explicitly describes it as a utility for graphically authoring
XML assets that a separate XNA game later builds and loads as `Curve` objects. The full behavior
includes:

- multiple visible/hidden and single/multi-selected curves with create/open/rename/delete and
  per-curve save/save-as/dirty-close prompts;
- key creation, deletion, box selection and numeric or mouse-based movement;
- independent in/out tangents, manual tangent handles and Flat, Linear, Smooth, Stepped and Fixed
  editing modes;
- Constant, Cycle, CycleOffset, Linear and Oscillate pre/post infinity behavior;
- grid calculation and GDI+ drawing, configurable curve tessellation, key/tangent visibility,
  pan, drag zoom, wheel zoom, Frame All, Frame Selection and Auto Frame;
- grouped undo/redo command history for curve list, key, selection and curve-state changes;
- WinForms designer resources, menus/toolbars/list views, standard open/save/message dialogs,
  custom cursors and embedded PNG/bitmap resources;
- `IntermediateSerializer.Serialize<Curve>` and `Deserialize<Curve>` for the XNA Content Pipeline
  XML wire format;
- a separate usage executable that displays the documented three-key editable red curve and
  three-key noneditable green curve.

Porting only the three-key usage graph into a `Game`, or exposing only `Curve::Evaluate` through a
CLI, would discard the defining authoring product and is forbidden by the campaign rules.

## Original build and execution evidence

The legacy project file spells `GridControl.designer.cs` while the distributed file is
`GridControl.Designer.cs`. That is harmless on its intended case-insensitive Windows filesystem
but prevents an unchanged `xbuild` project invocation on Linux. The retained
`scripts/build-original.sh` therefore feeds every exact source/resource file directly to the C# 4
compiler; it changes no source and omits only the unused `System.Deployment` reference absent from
the host Mono profile. It produces these PE32 assemblies:

| Output | SHA-256 |
|---|---|
| `CurveControl.dll` | `73e1f874015fd862f7302cbdae0350d11c7cbac7aafdf0ee50c4b37a04c727e8` |
| `CurveEditor.exe` | `922a1908d378f9675e1689e4e30f5fb5aa00704c02b097d2c96123a69b1945a9` |
| `CurveControlUsageSample.exe` | `5a9485d9111c5e654b6095c5582f0db568b9f16d83a8f5bd8ff3373744b62ebd` |

Linux Mono alone cannot execute the official XNA 4 framework/content-pipeline C++/CLI module; it
fails at the module's native initializer before the first `Vector2`/`Curve` operation. That failed
host route is retained rather than relabeled as an application defect. The same exact assemblies
run through the established offline Wine prefix with the official XNA 4 runtime:

- `evidence/xna-wine/curve-editor-startup.png` captures the empty 808x333 editor;
- `evidence/xna-wine/curve-editor-new-curve.png` captures real CTRL+N behavior and its editable key;
- `evidence/xna-wine/curve-control-usage.png` captures both documented 491x580 red/green graphs;
- both applications close normally through their own form lifecycle.

The separate diagnostic `CurveRoundTripProbe.cs` calls the unchanged public `EditCurve.Save` and
`EditCurve.LoadFromFile` methods with three keys, both continuity modes, all tangent fields and
non-default loop modes. Official XNA writes and reads:

```xml
<XnaContent xmlns:Framework="Microsoft.Xna.Framework"><Asset Type="Framework:Curve"><PreLoop>Oscillate</PreLoop><PostLoop>CycleOffset</PostLoop><Keys>-1.5 2.25 -0.5 1.25 Smooth 0.75 -3.5 2 -4 Step 4 7.125 0.25 0.75 Smooth</Keys></Asset></XnaContent>
```

The reloaded object retains all fields and produces the expected seven evaluated values; the exact
XML has SHA-256
`2de8e8c21ede9a673f3a7e808ba5d5d77af5542656a58ecde6cdd02bb6276130`.
The XML, log, build hashes and reproducible scripts are retained under the audit root.

## Live CNA and Sharp Runtime audit

The audit used CNA `e5ae0820e234` and Sharp Runtime `next` at `bd282d101640`. Neither dependency was
modified for this sample. The owner's separate `sharp-runtimenext` `xml` branch was not inspected,
modified or depended upon.

CNA already exposes the XNA-shaped `Curve`, `CurveKey`, `CurveKeyCollection`, `CurveContinuity`,
`CurveLoopType` and `CurveTangent` APIs. All 82 focused math tests pass, including every tangent
mode, cloning/collection semantics, Step continuity and all pre/post loop calculations. Another
29 focused content tests pass for the standard XNB `CurveReader`, CNJ curve documents, CNB curve
round trips/compiler/tool/content-manager routes, malformed input and golden vectors. Evidence:

- `evidence-cna-curve-tests.log`: 82/82 from six suites;
- `evidence/cna-content-curve-tests.log`: 29/29 from seven suites.

This means SAMPLE-093 does not justify another curve representation or a sample-local evaluator.
CNA can load compiled XNA curve content and its native formats already preserve positions, values,
tangents, continuity and loop modes.

Sharp Runtime has no `System::Windows::Forms` or `System::Drawing` implementation. The absent scope
is substantial: `Form`, `UserControl`, WinForms layout/designer/resources, menus/toolbars/list
views/dialogs, mouse/keyboard/control events, cursors, `Graphics`/pens/brushes/fonts/bitmaps and the
GDI+ paint/measurement model. CNA's `Game`/SDL/browser presentation stack is not a drop-in
implementation of that desktop component contract.

The editor's serializer is also **not** `System.Xml.Serialization.XmlSerializer` from
`SAMPLES-DEC-008`. It is the XNA design-time Content Pipeline `IntermediateSerializer`, and its
wire contract is measured above. A future general `XmlSerializer` implementation may help other
samples but does not supply this WinForms product or automatically implement this pipeline API.

## Browser and scope boundary

The mandatory browser gate is a material product decision here. A Web canvas editor could preserve
most visible curve-editing semantics, but it would replace the reusable WinForms control,
designer/component hosting, native dialogs, desktop resource model and local-file workflow. A
native ImGui/Qt/SDL editor would likewise be a modernization, not a mechanical C#-to-C++ port.
Either can be worthwhile only after the owner defines the accepted UI/component/file/browser
contract and qualification baseline.

No `CMakeLists.txt`, source target, alternate serializer, sample workaround or CNA/Sharp Runtime
change has been added. The documentation and license are retained beside this report so the row is
not lost while it awaits `SAMPLES-DEC-005`.

## Owner choices

1. Accept this evidence-backed non-game/design-time-tool non-port boundary.
2. Authorize a faithful Windows desktop-tool scope, including a chosen C++ UI/component strategy,
   the exact XNA curve XML contract, both executables and a separate ruling for the browser gate.
3. Authorize an explicit cross-platform modernization and define which desktop component,
   designer integration, file/dialog, rendering and browser differences are acceptable, while
   retaining the complete editor behavior and measured XNA XML format.

Until one is selected, a fake `Game`, graph-only demo, headless converter or copied three-key usage
screen would conceal rather than resolve the missing product.
