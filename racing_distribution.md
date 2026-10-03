# Racing Game Web distribution analysis

## Decision boundary

No public deployment is authorized yet. The canonical snapshot still has no
redistribution licence, so the authentic XNA assets must remain in the external
qualification root until the owner supplies and approves a grant covering the
intended platforms. This document evaluates delivery architecture only; it does
not change `REDISTRIBUTION_STATUS=BLOCKED_MISSING_CANONICAL_LICENSE`.

The formerly considered dedicated source/deployment repository and
`racinggame.libcna.com` option on the existing Endora account was fully assessed
with intact generated `.data`
files and no 100 MB splitting. The owner subsequently chose not to publish the
playable Web bundle. The current public plan is an informational
`samples.libcna.com` entry with a screenshot, an explicit licence limitation, the
existing `cna-samples` source link and an inactive placeholder for a future
YouTube video. No dedicated repository is planned. The Endora analysis below is
retained as a historical contingency study, not an active deployment plan.

## Measured Release payload — 2026-10-03

The current-head WebGL2 build contains ten deployable files totalling
`300,037,131` bytes (`286.14 MiB`):

| File/group | Bytes | MiB | Ordinary Git consequence |
|---|---:|---:|---|
| `RacingGame-content-landscape.data` | 178,814,172 | 170.53 | Rejected: exceeds GitHub's 100 MiB object limit |
| `RacingGame-content-models.data` | 72,915,638 | 69.54 | Accepted by command-line Git with a warning above 50 MiB |
| `RacingGame_cna_samples.data` | 36,646,329 | 34.95 | Accepted |
| `RacingGame_cna_samples.wasm` | 10,509,098 | 10.02 | Accepted |
| Textures data, HTML and JavaScript | 1,151,894 | 1.10 | Accepted |

These are deployment files, not the 655 MiB CMake tree. The four data groups
contain unchanged authentic runtime products and already use browser IndexedDB
preload caching, so a repeat visit normally avoids downloading them again.

## GitHub-only options

GitHub enforces a 100 MB single-object limit and a 2 GB push limit. It recommends
keeping repositories ideally below 1 GB and warns for files above 50 MiB. GitHub
Pages separately recommends a 1 GB source repository, caps the published site at
1 GB, has a soft 100 GB/month bandwidth limit and may time out a deployment after
ten minutes. Git LFS cannot supply a GitHub Pages site.

The current local `demos.libcna.com` worktree excluding `.git` is approximately
`565,430,048` bytes (`539.24 MiB`), and its loose Git database is
`457,434,100` bytes (`436.24 MiB`). Adding the Racing payload would make the
published worktree about `865,467,179` bytes (`825.37 MiB`) before page art,
metadata and future revisions. This fits the current Pages site cap, but leaves
little growth room. The existing House Simulator already demonstrates a 64 MiB
part loader, so the 170.53 MiB Landscape file can be split mechanically; Models
should also be split to avoid the 50 MiB warning. Each new binary revision would
still grow Git history.

At `286.14 MiB` per cold visit, the Pages soft bandwidth allowance represents only
about 358 complete first loads per month. Browser cache hits reduce repeat traffic,
but a separate repository does not change that site-wide delivery limit. GitHub
may contact the owner if repository or Pages usage affects its infrastructure;
the current size alone is not evidence that it will reject the account.

A dedicated GitHub Pages repository would be technically possible after file
splitting and after requalifying the existing service-worker cross-origin-isolation
route. The owner rejected splitting, so GitHub Pages is not the selected host.
GitHub Releases accept individual assets below 2 GiB and have no stated aggregate
release or bandwidth quota. They remain suitable for downloadable archives; using
release-asset URLs as the game's live CDN needs a real-browser CORS/COEP and cache
qualification first and is not the primary plan.

## Retained Endora repository and hosting option

Create the repository only after the licence gate is cleared. Its intended
contents are the C++ source, shell, build/deployment scripts, manifests, checksums,
licence material and small presentation files. The 286.14 MiB generated product
is uploaded directly to Endora rather than committed to ordinary Git.

Publish all ten intact build files from one directory at
`https://racinggame.libcna.com/`. Endora currently advertises 1 GB, 3 GB and 50 GB
disk allocations for FREE, FUN and MAX respectively, so one 286.14 MiB release
fits every plan in isolation. Its PHP upload limits are 64 MB on FREE and 256 MB
on FUN/MAX. Those settings govern HTTP/PHP uploads rather than the advertised
encrypted FTP path; the public documentation states no FTP per-file maximum.
The 178,814,172-byte Landscape file should therefore be transferred by FTP and
then downloaded back or hash-checked on the server. Do not route it through a PHP
upload form on FREE.

The threaded Emscripten product must receive these headers from Endora for the
HTML and same-origin resources:

```text
Cross-Origin-Opener-Policy: same-origin
Cross-Origin-Embedder-Policy: require-corp
Cross-Origin-Resource-Policy: same-origin
```

Endora advertises Apache `.htaccess`, HTTP/2 and TLS. The prepared deployment uses
`.htaccess` for those headers plus `application/wasm` and
`application/octet-stream` MIME types. A live header check and real-Chrome run are
still mandatory because public documentation does not confirm that `mod_headers`
is enabled for this account.

`samples/RacingGame/scripts/prepare-endora-package.sh` copies the ten intact build
files into the local evidence root, renames the shell to `index.html`, installs the
reviewable `.htaccess`, rejects any `.part*` artifact, verifies the exact
178,814,172-byte Landscape file and writes `SHA256SUMS`. It deliberately prints the
blocked redistribution status and performs no network or DNS operation.

There is a material service-policy question to resolve before public upload.
Endora's hosting terms prohibit using the service as file hosting or for mass
distribution of software or large media. Racing is an interactive web application,
but each cold start transfers 286.14 MiB, and Endora also defines unlimited traffic
as usage that does not significantly exceed ordinary customer use or affect the
service. Obtain written support confirmation that this specific WebAssembly/WebGL
application and expected traffic are permitted. No source change can settle that
contract boundary.

Suggested support question:

> Na `racinggame.libcna.com` chceme provozovat interaktivní WebAssembly/WebGL hru.
> První spuštění stáhne 286,14 MiB v deseti statických souborech; největší `.data`
> má 178 814 172 bajtů a další návštěvy používají IndexedDB cache. Je tento provoz
> na našem tarifu povolen, lze největší soubor nahrát přes šifrované FTP a podporuje
> server přes `.htaccess` hlavičky COOP/COEP/CORP a MIME `application/wasm`?

Cloudflare R2 remains an optional contingency if Endora declines the traffic or
later load exceeds its fair-use boundary. It is not part of the owner's selected
initial deployment and does not require splitting the files.

## Release gates if the Endora option is revived

Before DNS or public upload:

1. clear the canonical asset licence and commit the reviewed licence/provenance;
2. build from pinned CNA, Sharp Runtime and Emscripten revisions;
3. produce an immutable manifest with size and SHA-256 for every deployable file;
4. test a cold load and an IndexedDB-cached reload over the real public network;
5. measure transfer time, peak Wasm memory and GPU residency on the supported
   desktop/mobile browser matrix;
6. receive Endora's written approval for the measured application/traffic and
   verify FTP transfer of the intact 178,814,172-byte file;
7. verify COOP, COEP, CORP, content types, range behavior and cache headers on the
   real Endora hostname;
8. finish audible XACT and physical mobile-browser input qualification;
9. retain a previous manifest/release for rollback and monitor request volume.

## Current service references

- [GitHub repository limits](https://docs.github.com/en/repositories/creating-and-managing-repositories/repository-limits)
- [GitHub large-file guidance](https://docs.github.com/en/repositories/working-with-files/managing-large-files/about-large-files-on-github)
- [GitHub Pages limits](https://docs.github.com/en/enterprise-cloud@latest/pages/getting-started-with-github-pages/github-pages-limits)
- [Git LFS and Pages restriction](https://docs.github.com/en/repositories/working-with-files/managing-large-files/about-git-large-file-storage)
- [GitHub Releases limits](https://docs.github.com/en/repositories/releasing-projects-on-github/about-releases)
- [Cloudflare R2 pricing](https://developers.cloudflare.com/r2/pricing/)
- [Cloudflare R2 custom-domain caching](https://developers.cloudflare.com/cache/interaction-cloudflare-products/r2/)
- [Cloudflare R2 CORS](https://developers.cloudflare.com/r2/buckets/cors/)
- [Cloudflare Pages limits](https://developers.cloudflare.com/pages/platform/limits/)
- [Endora hosting plans and limits](https://www.endora.cz/hosting)
- [Endora hosting use conditions](https://www.endora.cz/podminky-endora-hostingu)
- [Endora PHP upload limit](https://www.endora.cz/napoveda/nastaveni-upload_max_filesize-post_max_size)
