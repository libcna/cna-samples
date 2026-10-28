AI must not be used to generate code for contributions to this project.

"AI" in this case means a Large Language Model ("LLM"), such as ChatGPT,
Claude, Copilot, Grok, etc.

AI-generated code is based upon sources of unknown origins and may not be 
compatible with the licenses applicable to this project, or may introduce 
conflicting license terms if it includes code from other projects.

AI can be used to identify issues with contributions to this project, but the
solutions to those issues should be authored by humans.

We have found that AI will frequently hallucinate issues that are not actually
problems in practice, report incorrect information, and describe problems that
are actually not issues at all. If AI identifies a problem with this codebase,
please make sure you understand what it is saying and have independently
confirmed that the issue exists before submitting a bug report or pull request.

By submitting a pull request to this project, you confirm that you are the
author of your changes, have the necessary rights to contribute them, and
comply with all applicable licensing requirements of this project.

# CNA Samples Agent Instructions

Before performing any work in this repository, every agent must read [`rules.md`](rules.md)
completely and obey it. This requirement applies to analysis, audits, source/content edits, builds,
runs, browser tests, documentation, commits and reviews.

Agents must also read [`plan.md`](plan.md), the affected sample's `missing.md`, and any applicable
instructions in `../cna` or `../sharp-runtime` before changing those repositories.
`rules.md` is the binding repository-wide policy for sample work.

Start with [`NEXT.md`](NEXT.md)'s **Active handoff** section. It names the sample to work on next,
the synchronized head of all three repositories, the open items a new session inherits, and the
reusable techniques this campaign has settled into — decoding an `.xnb`'s reader table, building a
sample's own content processors, the frozen diagnostic pair, and how to run CNA's tests
without chasing failures that were already there.
