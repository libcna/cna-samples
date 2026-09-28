# Differences from the XNA 4.0 original

## Current owner decision — deferred, 2026-09-28

The owner superseded the earlier scope below, requested complete removal of NetworkPrediction
from the web gallery and marked SAMPLE-100 deferred. Its source, products and qualification evidence
are retained for later work. This is not cancellation and does not authorize a sign-in bypass or
fabricated profiles. The current native product still needs a real configured CNA account service;
local/offline profiles and the browser networking gaps remain unfinished. See `missing.md`.

## Historical owner-approved native scope — 2026-09-28

The owner requested the current implementation scope and explicitly selected **a native port plus
an English gallery page listing the exact missing browser features**. Under that earlier scope, SAMPLE-100 had a
current native OPENGLES3 product and a clearly marked browser status page, not a runnable web
release. Browser account sign-in, session discovery/directory, relay/connection handoff and real
peer qualification remain shared CNA work. No local-only host menu is presented as multiplayer.

The current CNA account backend uses CNA service accounts rather than Xbox LIVE/GFWL profiles.
Native deployment needs an externally configured service/title and genuine Guide sign-in.
Guest/offline profiles are unfinished in current CNA and are not fabricated by the sample.
This deployment boundary was documented on the now-removed gallery page and remains in `missing.md`.

No sample workaround, replacement screen, new control or packet format was introduced.
The runtime source remains the direct original translation. Guide activity and network identity
were corrected generally in CNA, preserving the original sample's IsActive/sign-in guard and
inherited gamer-name display. Exact official XNBs and original metadata are retained. See
`missing.md` for measured native/reference results and all browser gaps.
