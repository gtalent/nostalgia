#! /usr/bin/env bash

set -e

echo NOSTALGIA_MEDIA_HEADER_________ > media_header.txt

./dist/current/bin/oxfs format 32 1k nostalgia_media.oxfs
./dist/current/bin/nost-pack -fs nostalgia_media.oxfs -img charset.png -inode 101 -tiles 40 -bpp 4 -c

${DEVKITARM}/bin/padbin 32 build/gba-release/nostalgia.bin
cat build/gba-release/nostalgia.bin media_header.txt nostalgia_media.oxfs > nostalgia.gba
rm -f media_header.txt
${DEVKITARM}/bin/gbafix nostalgia.gba
