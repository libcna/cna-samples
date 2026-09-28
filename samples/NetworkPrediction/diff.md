# Differences from the XNA 4.0 original

## Owner-approved native scope — 2026-09-28

The owner requested the current implementation scope and explicitly selected **a native port plus
an English gallery page listing the exact missing browser features**. SAMPLE-100 therefore has a
current native OPENGLES3 product and a clearly marked browser status page, not a runnable web
release. Browser account sign-in, session discovery/directory, relay/connection handoff and real
peer qualification remain shared CNA work. No local-only host menu is presented as multiplayer.

The current CNA account backend uses CNA service accounts rather than Xbox LIVE/GFWL profiles.
Native deployment needs an externally configured service/title and genuine Guide sign-in.
Guest/offline profiles are unfinished in current CNA and are not fabricated by the sample.
This deployment boundary is documented on the gallery page and in `missing.md`.

No sample workaround, replacement screen, new control or packet format was introduced.
The runtime source remains the direct original translation. Guide activity and network identity
were corrected generally in CNA, preserving the original sample's IsActive/sign-in guard and
inherited gamer-name display. Exact official XNBs and original metadata are retained. See
`missing.md` for measured native/reference results and all browser gaps.
