# CNA Samples Instructions for Claude

Before performing any work in this repository, read [`rules.md`](rules.md) completely and follow
it. This includes analysis, audits, source changes, content changes, builds, original-XNA runs,
native or web testing, documentation, commits and reviews.

Also read [`plan.md`](plan.md), the affected sample's `missing.md`, and the instructions in
`../cnanext` or `../sharp-runtimenext` before changing either dependency. `rules.md` is the binding
repository-wide porting policy; old statuses and contradictory historical guidance are not
authority.

Start with [`NEXT.md`](NEXT.md)'s **Active handoff** section. It names the sample to work on next,
the synchronized head of all three repositories, the open items a new session inherits, and the
reusable techniques this campaign has settled into — decoding an `.xnb`'s reader table, building a
sample's own content processors, the frozen diagnostic pair, and how to run `cnanext`'s tests
without chasing failures that were already there.
