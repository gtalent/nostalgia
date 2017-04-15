#! /usr/bin/env bash

set -e

padbin 32 build/gba-release/src/player/nostalgia.bin
echo NOSTALGIA_MEDIA_HEADER_________ > media_header.txt
oxfs format 32 1m nostalgia_media.oxfs
./build/current/src/tools/nost-pack -fs nostalgia_media.oxfs -img charset.png -inode 1 -tiles 40 -c
cat build/gba-release/src/player/nostalgia.bin media_header.txt nostalgia_media.oxfs > nostalgia.gba
gbafix nostalgia.gba
