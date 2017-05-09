#! /usr/bin/env bash

set -e

echo NOSTALGIA_MEDIA_HEADER_________ > media_header.txt

./build/current/deps/ox/src/ox/fs/oxfs format 32 1k nostalgia_media.oxfs
./build/current/src/tools/nost-pack -fs nostalgia_media.oxfs -img charset.png -inode 101 -tiles 40 -bpp 4 -c

${DEVKITARM}/bin/padbin 32 build/gba-release/src/player/nostalgia.bin
cat build/gba-release/src/player/nostalgia.bin media_header.txt nostalgia_media.oxfs > nostalgia.gba
${DEVKITARM}/bin/gbafix nostalgia.gba
