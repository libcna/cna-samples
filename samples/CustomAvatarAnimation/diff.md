# Owner-approved differences and C++ translation mechanics

The owner explicitly confirmed SAMPLE-094 as fourth in 085 → 086 → 101 → 094 and approved CNA's
standard original-art bodies, offline/service appearance and built-in Stand timing. The five
custom motions and facial expressions remain Microsoft's original FBX/CSV data, rebuilt through
the unchanged custom processor and official Windows/HiDef pipeline. Ground, camera, input,
walking, nine loaded animations and original idle Next(3) defect are retained. CNA body proportions and BindPose also differ from Xbox avatars; custom matrices are not
retargeted or tuned for the replacement bodies. Xbox pixel parity is not claimed. No new preview, substitute clip or loose-content runtime path is added.

One shared off-by-default CNAEXT GamePad keyboard opt-in uses the approved keys. WASD is the left
stick, arrows the right stick, K/L/J/I A/B/X/Y (Jump/Kick/Punch/Faint), E a new random avatar, Q the
signed-in profile avatar, Z/C triggers, Right Shift reset and Escape Back. The original starts with
no avatar when no profile is signed in; E loads a random one. CNA's Guide manages offline profiles.

One sample-owned CNAEXT reader registration declares exactly the original reflective type graph,
including nullable Lists, struct keyframes, AvatarExpression properties and its enums. This is
C++'s AOT counterpart of XNA reflection, using CNA's normal readers and XNB dispatch, as in earlier
custom animation ports. The design-time C# processor stays unchanged in ContentPipeline/ and in
the retained upstream tree. No hand-written alternate asset loader.

Smart pointers express managed resource/list ownership, the current player implements the original
interface, and a shared vector preserves the live ReadOnlyCollection over the bone array. C++
retains explicit null checks where managed code would dereference null. Original reverse-loop
NullReferenceException on clips without expressions and reverse-after-end list bounds exceptions
are preserved. Async-result ownership and removing the title's sign-in subscription on C++
destruction handle lifetime without altering callbacks or sign-in branches. Original source-unit,
class/type names and title/HiDef metadata are retained.
