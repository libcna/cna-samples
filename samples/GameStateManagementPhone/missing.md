# SAMPLE-127 — `GSMSample_4_0_PHONE` audit and owner decision

## Status

The complete source, project, content, installed-toolchain and runtime-boundary audit is finished.
This is a distinct pre-Mango Windows Phone 7.0 Game State Management product, not an archive alias
for the already completed SAMPLE-072 desktop/Xbox port and not the later SAMPLE-125 Mango
generation. No desktop controls, fake Phone lifecycle, partial merge or sample-local reflection
substitute was added. The owner must choose a separate faithful Phone product, an explicitly
canonical merged generation, or an evidence-backed historical non-port under `SAMPLES-DEC-005`.

## Complete inventory

The retained directory has 29 files / 249,009 bytes. Its 14 C# files contain 2,589 lines. The
Phone project names exactly those 14 units, with no unreferenced C# source and no omitted compile
item. It has one game project, one content project and one `Debug|Windows Phone` /
`Release|Windows Phone` solution.

Against the physical SAMPLE-072 `GSMSample_4_0_WIN_XBOX` source, six units are byte-identical:

- `Properties/AssemblyInfo.cs`;
- `ScreenManager/InputState.cs`;
- `Screens/BackgroundScreen.cs`;
- `Screens/MenuEntry.cs`;
- `Screens/MessageBoxScreen.cs`;
- `Screens/PlayerIndexEventArgs.cs`.

Eight units materially differ: `Game`, `GameScreen`, `ScreenManager`, `GameplayScreen`,
`LoadingScreen`, `MainMenuScreen`, `MenuScreen` and `OptionsMenuScreen`. The base generation's
`PauseMenuScreen` has no Phone counterpart. The complete line diff is retained as
`evidence/source-vs-base.diff`; this is a complete variant, not a few conditional lines that can
be relabelled onto the existing target.

## Phone-specific behavior

The unchanged game deliberately requests full-screen 480x800 portrait rendering at 30 Hz
(`TimeSpan.FromTicks(333333)`). Its constructor restores the screen stack before installing the
default background/main-menu pair, and `OnExiting` serializes the stack.

The Phone menu is a different interaction model:

- every menu enables only `GestureType.Tap`;
- each entry has a full-viewport-width hit box with ten pixels of vertical padding;
- taps select the hit entry as `PlayerIndex.One`;
- the hardware/gamepad `Back` button cancels screens, while the menu does not use the desktop
  keyboard/gamepad selection helpers;
- the main menu omits `Exit`, and Back exits immediately without SAMPLE-072's confirmation;
- the options menu omits its explicit `Back` entry;
- gameplay Back loads a fresh background/main-menu pair instead of opening a pause popup;
- the loading screen is explicitly non-serializable.

The base port remains 853x480, uses its desktop/Xbox keyboard/gamepad menus, exit confirmations and
pause screen, and has none of `SerializeState`, `DeserializeState`, `IsSerializable`,
`GetMenuEntryHitBounds` or the padded touch-menu path. Only generic input collection and the
unchanged base units overlap. Calling it this Phone product would discard the sample's subject.

## Exact persisted-state contract

`ScreenManager.SerializeState` recreates an isolated-storage `ScreenManager` directory, writes one
assembly-qualified runtime type name per serializable stack entry to binary `ScreenList.dat`, then
calls each screen's virtual `Serialize` method with a separate `ScreenN.dat`. No shipped screen
overrides the default empty per-screen methods, so the concrete sample persists stack identity but
no gameplay coordinates. `DeserializeState` reads to end-of-stream, resolves each name through
`Type.GetType`, constructs it through `Activator.CreateInstance`, adds it as player one, then calls
the matching virtual `Deserialize`; corrupt state is deleted and the default stack is used.

Live Sharp Runtime now has the required isolated-storage, stream and binary-reader/writer
primitives, so storage is no longer the blocker. Its documented permanent no-reflection boundary
still excludes the runtime `AssemblyQualifiedName -> Type -> Activator` route. A closed
compile-time registry can represent this finite shipped screen set in C++, but it changes the
reusable component's open runtime-type contract and its persisted identifiers. That is the same
owner-level AOT factory decision already recorded for SAMPLE-125, not permission to hard-code a
silent sample workaround.

## Content provenance and qualification

The content project, 4x4 blank texture, 8x8 gradient and both Segoe UI SpriteFont descriptions are
byte-identical to the later Mango sample. SAMPLE-127 alone has a 480x800 background; Mango uses
800x480. Against SAMPLE-072, only the blank and gradient source assets match; the background and
both font descriptions differ. The shared documentation, license and shell artwork are
byte-identical to SAMPLE-072 even though the code/content product is not. The Phone manifest is
7.0, requests nine capabilities, uses `Apps.Normal` and supplies the tile title; the base physical
manifest differs in genre/title despite sharing ProductID and artwork.

The installed Win7 environment cannot produce Phone XNBs, so no authentic Phone content build is
claimed. As independent format evidence, the official XNA 4 Windows/Reach pipeline successfully
compiled the unchanged five content declarations. Four of those XNBs are byte-identical to the
same-host Mango output; only the portrait background differs. Only blank and gradient match the
existing SAMPLE-072 port, proving that port cannot donate the other three artifacts. Current CNA
converts all five reference XNBs, every CNB passes its info reader, and 13/13 focused live CNA
Texture2D, SpriteFont and XNB tests pass. Content representation is not the scope blocker.

## Authentic toolchain and runtime boundary

The unchanged directory was copied to the user's authorized XNA workspace in the offline/headless
Win7 VM. Standard `Debug|Windows Phone` Build selects the exact game project, then the installed
XNA target explicitly reports that this XNA Game Studio installation does not support
`XNA Platform = 'Windows Phone', XNA Framework Version = 'v4.0'`.

The original solution also omits every Content project configuration mapping. `Rebuild` therefore
fails earlier because `CleanContent` receives no `TargetPlatform`; ordinary `Build` reaches the
more authoritative installed-platform rejection above. Both facts belong to the unchanged
delivery and are retained separately.

A Windows/Reach host compile proves that all 14 unchanged C# units compile together and produces
the five format-reference XNBs, but it is not a Phone execution. Running that executable fails in
the game constructor: ordinary Windows execution has no Phone/XAP application identity for
`GetUserStoreForApplication`. The audit does not bypass that check and does not claim a rendered
original, touch interaction, persisted-state roundtrip or Phone lifecycle result. The VM finished
`poweroff` with `nic1` through `nic8` all `none`; no Internet or owner's real display was used.

## Owner choice required

Choose one boundary together with the related GSM generations:

1. Create a distinct faithful Phone 7.0 product. Port all 14 units and the exact portrait content,
   retain 30 Hz, touch-only menu selection, hardware Back semantics, binary multi-file stack state
   and corrupt-state recovery, and explicitly approve/document a closed AOT mapping for the
   shipped runtime type names. Define the portable/native/browser lifecycle and Back-button target
   without inventing keyboard or mouse gameplay controls.
2. Merge this generation into a canonical multi-platform GSM product. State exactly which
   SAMPLE-072, SAMPLE-127 and/or SAMPLE-125 behavior owns each platform branch, while preserving
   this complete Phone path rather than reducing it to the existing desktop target. Resolve the
   same closed-factory/lifecycle boundary once for the merged product.
3. Retain SAMPLE-127 as an audited historical Phone 7.0 predecessor/support delivery and accept an
   evidence-backed non-port. SAMPLE-072 remains the completed desktop/Xbox product and any Mango
   work proceeds under SAMPLE-125/SAMPLE-126.

Do not alias SAMPLE-072, add desktop controls, treat the host Windows constructor failure as a game
bug, or claim that Mango's later XML/factory/lifecycle design is the same persisted-state contract.

## Reproduction and evidence

Artifact root:

`/rv/tmp/samples/SAMPLE-127-GSMSample_4_0_PHONE/`

Important retained material:

- `xna4-original/` — exact 29-file snapshot;
- `scripts/build-reference.sh` — all-source host compile and official format-reference content
  build, explicitly labelled non-Phone;
- `scripts/win7-build-phone.cmd` — authentic offline standard Phone Build;
- `scripts/audit.py` and `qualify.sh` — complete inventory/hash/assertion and content qualification;
- `evidence/source-vs-base.tsv` and `source-vs-base.diff` — every C# relation and changed line;
- `evidence/content-variant-hashes.tsv` and `xnb-variant-hashes.tsv` — source/XNB provenance across
  base, Phone and Mango generations;
- `evidence/win7-phone-build-msbuild.log` — exact installed-platform rejection;
- `evidence/win7-phone-msbuild.log` — distinct malformed-solution Rebuild/CleanContent result;
- `evidence/xna4-windows-phone-source-reference-probe/game.log` — exact missing-Phone-identity
  constructor failure;
- `evidence/cna-xnb-transcode.log`, `cna-cnb-info.log` and
  `cna-focused-content-tests.log` — 5/5 conversion and 13/13 tests;
- `evidence/win7-reference-boundary.txt` and `win7-final-state.txt` — qualification limits and
  final offline VM state.

Re-run the host-side qualification with:

```bash
/rv/tmp/samples/SAMPLE-127-GSMSample_4_0_PHONE/scripts/qualify.sh
```

All compilation/content worker settings in these scripts are capped at eight for this session.
