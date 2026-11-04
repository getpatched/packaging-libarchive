# GetPatched recipe: libarchive

GetPatched maintains selected, tested security backports for package versions whose normal public support has ended. This repository contains the recipe and evidence inputs; it does not vendor a duplicate upstream source tree.

## Targets

| Target ID | Distribution and pinned source | Candidate version | Binaries | State |
|---|---|---|---|---|
| `ubuntu-focal-amd64` | Ubuntu 20.04 LTS, public updates; `3.4.0-2ubuntu1.5` | `3.4.0-2ubuntu1.5+getpatched1` | `libarchive13`, `libarchive-tools` | Development candidate; release checks passed previously, no production package is published |
| `debian-trixie-amd64` | Debian 13; `3.7.4-4+deb13u1` | `3.7.4-4+deb13u1+getpatched1` | `libarchive13t64`, `libarchive-tools` | Development candidate; hosted package verification is pending, not a release claim |

Both targets are public immediately after their own release checks; neither has a sponsor delay. The Trixie target is not release-ready until an exact-source UBSan check reproduces the baseline overflow and passes after the backport, installed-package regressions, package tests, scans, consumer checks, and signed APT client checks pass.

## Recipe structure and target support

package-manifest.json is the package index. It points to one target file per distro, release, and architecture. Each target file holds its exact base, source publication, pinned .dsc and source archives, supported source versions, package version, binary names, patch IDs, and changelog entry. The platform repository's packages.json separately pins the target's build image, snapshot repositories, runner, scan ecosystem, release policy, and whether repository publication is supported.

A target ID names a distribution, release codename, and architecture, such as `ubuntu-focal-amd64` or `debian-trixie-amd64`. The package manifest and platform policy keep each target's source pins, patches, binary names, and test evidence independent. Do not claim support or enable publication until that exact combination passes a clean build, installed regressions, vulnerability scans, and an APT client test. This repository has no arm64 target.

The fetched source is the distribution's complete source package, including distro-specific packaging and existing patches. An upstream Git submodule alone would omit that state. Source files are pinned by URL, size, and SHA-256. The source descriptor and detached upstream archive signature have not been cryptographically verified; the recipe records that limitation.

## Patch and version policy

patches/metadata.json records each patch's purpose, checksum, upstream provenance, CVEs, regression tests, and appliesTo targets. An applicability entry may pin one exact source version or express inclusive/exclusive Debian-version bounds. The target's supportedSourceVersions remains an explicit allowlist: a version inside a patch range is not enough to build unless the target lists it and the build and regressions pass. Current patches are pinned to the exact version above.

The output version retains the full distro source version and adds `+getpatched1`. A new patch release from the same base advances the serial (`+getpatched2`); a changed distro source version gets its own reviewed target pin and release policy.

`CVE-2026-4111.patch` backports libarchive's RAR5 filter-block bound check for Ubuntu Focal. Its encoded fixture must time out against the vulnerable baseline and be rejected promptly by the installed patched package; a normal `bsdtar` create/extract consumer check follows. `CVE-2026-16517.patch` is scoped only to Debian Trixie source `3.7.4-4+deb13u1`; it avoids signed overflow in encrypted ZIP size checks. An exact-source UBSan build must reproduce `INT64_MAX + 28` before the patch and complete without the overflow after it. The installed baseline and patched packages must both generate the AES-256 ZIP header cleanly, followed by `bsdtar` consumer checks. Hosted Trixie package verification is pending. `zstd-test-two-byte-tolerance.patch` is a separate test-only compatibility adjustment for the pinned Focal source; it changes no runtime package code and claims no CVE.

## Sponsorship and licensing

This target is public immediately after release checks pass, so it has no sponsor-only binary period. Every active individual GitHub Sponsors tier receives early access to targets explicitly marked sponsor-first; there is no tier allowlist or minimum amount. A one-time sponsorship qualifies only while GitHub reports it active. Organization sponsorships are not yet supported because the service cannot securely map one to a specific GitHub user; organization membership alone never grants access. For sponsor-first targets, access ends when GitHub reports the sponsorship inactive. GetPatched does not permit redistribution of official early-channel binaries before public release; upstream and distribution license rights remain in force.

GetPatched-authored recipes, patches, tests, and documentation are MIT-licensed from day one. Anyone may build from public source and redistribute an independent build under MIT and applicable third-party licenses. The MIT file does not replace upstream or Ubuntu copyright and licensing obligations in the source or compiled package. Preserve LICENSES/ubuntu-source-copyright and comply with each included component's license. The prior full-source snapshot remains on the legacy branch: https://github.com/getpatched/packaging-libarchive/tree/legacy

## Build and verification

From a clean checkout on a disposable Linux Docker host:

    git clone https://github.com/getpatched/platform.git
    git clone --branch getpatched/focal https://github.com/getpatched/packaging-libarchive.git
    python3 platform/scripts/build-package.py --source packaging-libarchive --target ubuntu-focal-amd64 --output build/libarchive

For the Debian Trixie candidate, use its target branch and exact target ID:

    git clone --branch getpatched/debian-trixie https://github.com/getpatched/packaging-libarchive.git packaging-libarchive-trixie
    python3 platform/scripts/build-package.py --source packaging-libarchive-trixie --target debian-trixie-amd64 --output build/libarchive-trixie

The Trixie command is documented for reproducibility; do not treat it as verified until a hosted run succeeds against both installed phases.

The builder consumes the target's pinned inputs, runs baseline and patched installed-package checks, retains scan evidence, and tests signed APT installation of the binary and source packages. It does not publish binaries. Ubuntu Pro/ESM compatibility is not tested or claimed.
