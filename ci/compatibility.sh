#!/usr/bin/env bash
set -euo pipefail

work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT
cd "$work"

# A pathname beyond the ustar name field requires a PAX extended header.
printf -v name 'record-%0100d.txt' 1
printf 'PAX archive interoperability\n' > "$name"
mkdir from-gnu from-bsd

tar --format=pax -cf gnu.tar "$name"
bsdtar -xf gnu.tar -C from-gnu
cmp "$name" "from-gnu/$name"

bsdtar --format pax -cf bsd.tar "$name"
tar -xf bsd.tar -C from-bsd
cmp "$name" "from-bsd/$name"
