# SAMPLE-010 audit — InputSequenceSample_4_0

No known behavioral or visual differences remain after the `SAMPLE-010` audit. There is no
unresolved CNA or sharp-runtime implementation gap for this sample.

## Reference and source audit

- The unchanged XNA 4.0 source snapshot is retained under
  `/rv/tmp/samples/SAMPLE-010-InputSequenceSample_4_0/xna4-original/InputSequenceSample`.
- `Direction.cs`, `InputManager.cs`, `Move.cs`, `MoveList.cs`, `Game.cs`, the Content project and
  the Windows/Xbox projects were reviewed line by line. The Windows project is the runnable
  reference.
- The original executable and all 15 assets were built with the real XNA 4.0 assemblies and
  official Content Pipeline in the isolated Wine prefix. The original `Font.spritefont` uses the
  Miramonte Bold face shipped with XNA Game Studio; that exact font was installed into the
  isolated prefix before running the pipeline.

The C++ translation preserves the original 800x480 presentation, move order, layout, labels,
drop shadows, button/direction composition, two-player input managers, Windows keyboard mapping,
500 ms buffer timeout, 100 ms merge window, one-second active-move timeout and all nine move
sequences. The detection list is again a stable longest-first shallow copy, so equal-length moves
keep their source order and the returned objects are the same instances drawn by the master list.
An explicit logical buffer capacity reproduces `List<Buttons>.Capacity` without depending on an
implementation-specific `std::vector::capacity()` value.

Normal C++ ownership and initialization helpers, property calls, an enum-to-XNA-name helper and
`CNAEXT GetTypeName()` are the only representation-level adaptations.

## Exact XNA content

The former loose PNG substitutes and the DejaVu JSON/PNG font sidecar were removed. Every asset
now loads through the original `Content.Load` identifier, and all 15 checked-in XNBs are
byte-identical copies of the official pipeline output:

| Asset | SHA-256 |
|---|---|
| `A.xnb` | `02f0260ecc465cb8eec88e8fa7f8b647c7cc46744174535c3e46be790835072f` |
| `B.xnb` | `115148af000e39e515b1b9afae8b2b6b5ff4da80fe49a157e115e1471d6f3db6` |
| `Down.xnb` | `a7b2f227f5a3ecced611cb8ff8f65dee93c05ceb66fe67b5c5e7deb4f0e898f1` |
| `DownLeft.xnb` | `215dcf331cb4d4e5963585bad9fda52481fb5796f28bd2a0aa3a4e6573424534` |
| `DownRight.xnb` | `172022a4480838e9f2ff8fdefd8187d33143090ff60ec2af80871dbf7123c27a` |
| `Font.xnb` | `715531b0e6b27a5ff228017274f837695eb07a2688071cd87fc4e278136a4f24` |
| `Left.xnb` | `14d98179b03b0fb495e1a708ebe865eacf1dc8d6c494d02d773f434aa43f0961` |
| `PadFace.xnb` | `96ef4d2e01a163c40258e7ce5a2a17434acf403c1fe50430b367628087bf06d2` |
| `Plus.xnb` | `77b2a1096ac8f35dbca127f9d57554913c10f493282953e405fc4b0a3ac5ed29` |
| `Right.xnb` | `8980a408ece8a814ff7b7756f641da6aa79d1e4ab2ddda2f8655214aa206c464` |
| `Up.xnb` | `bc6e33670e16553211223d6261a039befbd8392f12f91812daa55aaa5369dfb2` |
| `UpLeft.xnb` | `74e1d56ccaf6e3ae16feeeaba82b410a3f79fbf50ff6f89c956a69ff723c18cf` |
| `UpRight.xnb` | `cb93a7977bb4b5dde236bbdc3ba99ef5c02078ba1df4b45f047a8d30cdf3bc55` |
| `X.xnb` | `bbf3a0d8af3b764b4edff0bb899d4057c3c8ec77167635791e7a972ebc4e3a35` |
| `Y.xnb` | `77cc73d0c44426535475b2a5c828cd7d6227a27c0b7bc199c378b7768ac43227` |

The old port-only F1 branch and help overlay are removed. Historical `help.png` is retained beside
the sample's `CMakeLists.txt`, outside `Content`, and is not loaded, copied or preloaded.

## No-workaround and framework review

The audited sample contains no loose content substitute, generated font sidecar, direct
`SetData`, raw mesh/model helper, backend call, handwritten shader, invented input, omitted branch
or help overlay. It uses only the XNA-facing CNA API. The audit found no framework defect: current
CNA already supplies the required XNB texture/font loading, SpriteBatch, keyboard/gamepad state,
`TimeSpan` arithmetic and EasyGL behavior, while sharp-runtime supplies the required .NET timing
semantics.

## Verification evidence

All source snapshots, generated files, builds, scripts, logs and captures are under
`/rv/tmp/samples/SAMPLE-010-InputSequenceSample_4_0`:

- `xna4-build/bin/InputSequenceSample.exe` is the real XNA 4.0 Windows reference. It runs through
  the dedicated Wine/WineD3D prefix and successfully recognizes `Jump`, `Double Jump`,
  `Jump Kick`, `Quad Punch`, `Fireball`, `Long Jump`, `Back Flip` and `30 Lives`; `Punch` is also
  present as the single-X sub-move exercised inside the longer sequences. Escape exits.
- `cna-native-opengles3/samples/InputSequence/InputSequence_cna_samples` reports EasyGL OpenGL ES
  3.2 and `OPENGLES3`. The same nine visible states and Escape pass. Each corresponding 800x480
  original/native capture is byte-for-byte identical, not merely visually similar.
- `cna-web-webgl2/samples/InputSequence/InputSequence_cna_samples.{html,data,js,wasm}` is the
  complete browser bundle. System Google Chrome fetched all four files with HTTP 200, reported
  WebGL 2.0 and `CNA: graphics renderer: WEBGL2`, recognized the same nine states and produced no
  application, wasm or WebGL runtime error. The baseline canvas is pixel-identical to XNA. After
  keyboard focus Chrome decorates the outer two-pixel canvas perimeter; the remaining 796x476
  rendered game area is pixel-identical for every active-move capture.

Reproduction scripts are in `scripts/`, and the corresponding captures and logs are in
`evidence/xna-original`, `evidence/cna-native-opengles3` and `evidence/cna-web-webgl2`.
