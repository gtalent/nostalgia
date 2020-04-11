# Nostalgia

## Setup

### Linux

Make sure conan is using the C++11 version of libstdc++.

	conan profile update settings.compiler.libcxx=libstdc++11 default

### macOS

Install and use gmake instead of the make that comes with the system.

## Build

Build options: release, debug, gba, gba-debug

	make purge conan configure-{gba,release,debug} install
