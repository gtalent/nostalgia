# Nostalgia

## Prerequisites

* Install GCC, Clang, or Visual Studio with C++17 support (including std::filesystem)
* Install [devkitPro](https://devkitpro.org/wiki/Getting_Started) to build for GBA
* Install Python 3
* Install Conan
* Install Ninja, Make, and CMake
* Consider also installing ccache for faster subsequent build times

## Platform Specific Setup

### macOS

Install and use gmake instead of the make that comes with the system.

## Build

Build options: release, debug, asan, gba, gba-debug

	make conan-config
	make purge conan configure-{gba,release,debug} install

## Run

### Studio

	make run-studio

### Native Platform

	make run

### GBA

	make gba-run

## Contributing

Please read the [Developer Handbook](developer-handbook.md) for information on
coding standards.
