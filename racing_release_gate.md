# Racing Game Kit release and asset gate

## Current decision

`REDISTRIBUTION_STATUS=BLOCKED_MISSING_CANONICAL_LICENSE`

The canonical XNA 4 snapshot contains Microsoft copyright notices but no license,
EULA, COPYING file or other grant that authorizes redistribution. The assets may
remain in the external local qualification artifact, but they must not be added to
cna-samples or a distributable package until the owner supplies and approves the
applicable license.

This decision is independent of runtime correctness: Milestone 9 passes while the
release gate remains blocked.

## Canonical inventory

The source manifest contains 325 hash-locked files. The content project includes:

| Kind | Count |
|---|---:|
| `.X` models | 57 |
| `.TGA` textures | 129 |
| `.PNG` textures | 11 |
| `.DDS` textures | 2 |
| `.fx` effects | 10 |
| `.wav` files | 28 |
| `.xap` project | 1 |
| `.Track` data | 3 |
| `.CombiModel` data | 10 |
| `.data` height field | 1 |

The unchanged XNA Game Studio 4 Debug build manifest contains 358 files: 28 Audio,
57 Model, 10 Shader and 244 Texture XNBs (339 total), plus one XGS, one XSB, one
XWB and 14 copied game-data files. Hashes live outside Git under:

```text
/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/evidence/
```

## Repeatable gate

Run:

```text
python3 samples/RacingGame/scripts/audit-release-assets.py \
  /rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master
```

Success means that both manifests and every expected asset class are intact. It
does not change the redistribution decision. The Release qualification script
requires both `RESULT PASS` and the blocked status so a missing license cannot be
silently mistaken for approval.

## Packaging rule

- Development and qualification consume the external authentic build by explicit
  content-root path or build-tree symlink.
- Source control contains code, tests, scripts, hashes and reports only.
- Windows, Android and Web packaging must remain local evidence builds while this
  gate is blocked.
- If a canonical license is later supplied, review its platform, modification and
  binary-content terms before changing this status; do not infer permission merely
  because XNBs were successfully generated.
