#! /usr/bin/env bash

set -e

BIN=./dist/current/bin/
NOSTALGA_BIN=build/gba-release/src/nostalgia/player/nostalgia.bin
NOSTALGA_MEDIA=nostalgia_media.oxfs
NOSTALGA_GBA=nostalgia.gba
MEDIA_HEADER=media_header.txt
CHARSET_FILE=src/nostalgia/core/studio/charset.png

echo NOSTALGIA_MEDIA_HEADER_________ > $MEDIA_HEADER

$BIN/oxfs format 32 1k $NOSTALGA_MEDIA
$BIN/nost-pack -fs $NOSTALGA_MEDIA -img $CHARSET_FILE -inode 101 -tiles 127 -bpp 4 -c

${DEVKITARM}/bin/padbin 32 $NOSTALGA_BIN
cat $NOSTALGA_BIN $MEDIA_HEADER $NOSTALGA_MEDIA > $NOSTALGA_GBA
rm -f $MEDIA_HEADER $NOSTALGA_MEDIA
${DEVKITARM}/bin/gbafix $NOSTALGA_GBA
