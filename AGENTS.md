# CNA Samples Agent Instructions

Before performing any work in this repository, every agent must read [`rules.md`](rules.md)
completely and obey it. This requirement applies to analysis, audits, source/content edits, builds,
runs, browser tests, documentation, commits and reviews.

Agents must also read [`plan.md`](plan.md), the affected sample's `missing.md`, and any applicable
instructions in `../cnanext` or `../sharp-runtimenext` before changing those repositories.
`rules.md` is the binding repository-wide policy for sample work.

Start with [`NEXT.md`](NEXT.md)'s **Active handoff** section. It names the sample to work on next,
the synchronized head of all three repositories, the open items a new session inherits, and the
reusable techniques this campaign has settled into — decoding an `.xnb`'s reader table, building a
sample's own content processors, the frozen diagnostic pair, and how to run `cnanext`'s tests
without chasing failures that were already there.
