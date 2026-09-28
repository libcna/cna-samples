# SAMPLE-104 — differences from the original XNA sample

## Owner-approved partial browser release — 2026-09-28

The owner subsequently requested a playable browser build and explicitly selected **🟡 partially
implemented** for its web status. This replaces the earlier decision to defer the entire web
release; it does not accept remote networking as complete.

The WEBGL2 build retains the complete original Windows source, NET dependency and `remote`
command. Local rendering, counters, timing ruler/log and command console are available. **The
network part does not work on the web yet:** there is no browser account transport and no qualified
public SystemLink discovery/join/hosting/relay route. With no signed-in gamer, `remote` displays
the original `Please signed in.` message. That is an unavailable feature, not a successful session.

The common CNA platform boundary now makes unsupported browser account/relay operations fail
explicitly instead of trying to link native libcurl. Native HTTPS/WSS retain libcurl and their
validation. There is no sample-local replacement transport, fake gamer/session or removed command.
This is deliberately incomplete framework platform support under the owner's partial-release
scope, not a workaround-free full-network qualification. The default framework does not acquire
new simulated network behavior.

### Completion after the shared network layer lands

1. Finish the shared browser account/deployment transport in CNA Gamer Services, including secure
   origin handling, asynchronous completion, sign-in and genuine signed-in identities.
2. Connect the shared directory and browser-capable realtime transport to public
   `NetworkSession` SystemLink find/join and host/address-handoff routes. Private native WSS/ENet
   progress alone is insufficient. Replace CNA's explicit browser refusals with the implemented
   platform adapters and their regression tests; do not change the sample's protocol or roles.
3. Rebuild this unchanged Windows-client sample and a genuine counterpart executing the original
   Xbox-host logic. Qualify authenticated discovery/join, one local gamer/two total gamers, all
   six packet headers, ReliableInOrder commands/echo/errors/warnings/start/quit, disconnection and
   clean disposal with a real browser peer.
4. Re-run local native/browser comparisons, replace the gallery bundle, update `missing.md`,
   this file, `plan.md`, the gallery limitation and CNA's readiness record. Promote 🟡 only after
   the missing network acceptance passes; linking successfully is not that acceptance.

Implementation locations and shared follow-up ownership are recorded in
`../cna/docs/browser-network-readiness.md` (from the samples repository root) and CNA's living
`plans/plan_gamer_services_server.md`. Build/runtime receipts are in [missing.md](missing.md).

## Implementation checkpoint — 2026-09-28

This checkpoint is historical with respect to browser release scope.

The owner requested a faithful implementation, then explicitly chose to **preserve `remote`
and defer web completion until the shared network layer is finished**. This is a deferral,
not permission to remove networking or certify an incomplete browser build. No sample runtime
workaround or native/local-only completion exception was approved.

| Translation or owning-layer correction | Result / evidence |
|---|---|
| Help listing and collections | The port now uses System Dictionary/List/Queue/Stack. Shared Dictionary enumeration follows live .NET entry slots, including LIFO reuse after removals. No command-specific sort or fixed help table. Real original/native help images are AE=0. |
| Bare echo and exceptions | Original System.String.Substring, caught System.Exception and InvalidOperationException are restored. Sharp Runtime supplies correct Substring validation/parameters and an opt-in Framework diagnostic profile; the CNA XNA host selects it, respecting explicit caller settings. Bare-echo images are AE=0, with no sample error-text rewriting. |
| String/culture operations | Shared System string/Char operations and IList callback arguments replace STL approximations. Original splitting, empty arguments, command history, console queue limits and case behavior are retained. |
| Remote packet matching | Shared System.Text.RegularExpressions.Regex uses the original named header/text captures. The six packet headers, SystemLink client/host roles and ReliableInOrder behavior remain. A successful authenticated peer exchange is still unqualified. |
| Project metadata | The existing CNA project-metadata API now selects Windows/Xbox HiDef and Phone Reach; original icon, thumbnail and tile bytes are restored. Component GetTypeName overrides preserve fully qualified managed names and are CNAEXT-marked. |

### C# to C++ representation

- CLR 4/x86 keeps wider precision for these unstored Single layout expressions. A real .NET 4
  probe measures `(int)(800 * .01f)` as 7, a stored float product as 8, and the widened product
  as 7. The translation widens the original expression before its truncating cast; it adds no
  positional offset and retains alignment/safe-area logic. All seven static reference frames
  now match, and variable timing panels no longer have the fixed placement difference.
- The original conditional symbol `TRACE` is spelled `PERFORMANCEUTILITY_TRACE` to avoid
  colliding with CNA's C++ `LogLevel::TRACE` enum. All original enabled/disabled bodies remain;
  syntax checks cover Windows TRACE-off, Phone on/off and Xbox on. This is not platform runtime
  qualification.
- Managed MarkerInfo objects retain reference identity through shared ownership. The original
  unchecked counters use unsigned 32-bit C++ arithmetic to preserve wrap behavior without
  signed-overflow undefined behavior. No timing or sampling algorithm was replaced.
- C++ component shutdown retains resources until their Game/graphics owner can release them.
  A disconnected owned remote session is disposed at the original point and its object is
  retained until the current Update/receive stack unwinds. This prevents premature deletion;
  the previously identified peer callback risk was not reproduced in an authenticated session.

No renderer helper, content substitute, sign-in UI, fabricated gamer, direct-address command or
mouse/keyboard control was added. Tap/Flick/Guide, physical gamepad/Phone/Xbox and positive remote
exchange remain unqualified. Browser CURL/account/SystemLink transport gaps remain shared work.
See [missing.md](missing.md) and artifact `evidence/implementation-20260928/` for receipts and the
owner-approved deferral. The earlier help/error/layout differences are fixed, not waived.
