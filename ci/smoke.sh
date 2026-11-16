#!/usr/bin/env bash
set -euo pipefail
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
cd "$work"
printf '%s\n' 'GetPatched archive round trip' > payload
bsdtar -czf sample.tar.gz payload
mkdir restored
bsdtar -xzf sample.tar.gz -C restored
cmp payload restored/payload
