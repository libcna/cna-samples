# SAMPLE-104 — differences from the original XNA sample

## Analysis checkpoint — 2026-09-28

These are **open, unapproved differences**, recorded before implementation. No owner-approved
runtime deviation exists for this sample. This document does not waive `rules.md`.

| Difference | Evidence / required owning layer |
|---|---|
| Help command listing order | XNA lists help, cls, echo, remote, pos, fps, tr; current C++ lists tr, fps, pos, remote, cls, echo, help. Actual original/native captures differ at 2,748 pixels because the port uses std::unordered_map. Correct the samples translation through System collections. |
| Bare echo error | Original String.Substring reports startIndex / ArgumentOutOfRange; C++ std::string::substr reports a libstdc++ implementation diagnostic. Actual captures differ at 4,865 pixels. Correct shared-string API use; keep the original caught error behavior. |
| Exception types / string operations | Register/Unregister throw std::runtime_error rather than InvalidOperationException; legacy manual string/culture operations need shared System API review. Source-established, not all paths runtime-tested. |
| Remote regex | std::regex replaces original System.Text.RegularExpressions.Regex named header/text captures. Sharp Runtime now supplies this feature; use it faithfully, without altering packets or remote semantics. |
| FPS panel placement | Fresh x86 XNA starts its FPS rectangle at x=7; current C++ starts at x=8. All 397/532 timing captures’ differing pixels lie within the diagnostic panels; they include this fixed spatial difference as well as varying values. Higher intermediate precision is a possible cause inferred from the unchanged float layout formula, not established causality. Investigate numeric semantics; do not add a sample offset. |
| Project/runtime metadata | Original Windows/Xbox HiDef is not declared through CNA's existing ProjectGraphicsProfile API; current default is Reach. Original icon/thumbnail/tile are only in the snapshot. Component managed names and TRACE-off branches need translation review. Current visible baseline still matches exactly. |

No backend/content workaround was added by the analysis. Original one-pixel textures, timing loop,
formatting algorithm and C++ component shutdown remain legitimate. Current remote-session teardown
has an unmeasured callback-lifetime risk; browser CURL/account/SystemLink gaps and unmeasured
Tap/Flick/Guide/positive peer behavior are qualification boundaries, recorded in `missing.md`.

Reference: fresh unchanged Windows/x86 Debug HiDef XNA versus static Release OPENGLES3 at CNA
967305dd7 / owner Sharp branch 6c4a857d, artifact `evidence/current-head-analysis-20260928/`.
