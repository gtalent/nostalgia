# Nostalgia

## Prerequisites

* Install GCC, Clang, or Visual Studio with C++17 support (including std::filesystem)
* Install Make and CMake
* Install Conan
* Consider also install in Ninja build and ccache for faster build times

## Platform Specific Setup

### macOS

Install and use gmake instead of the make that comes with the system.

## Build

Build options: release, debug, asan, gba, gba-debug

	make setup-conan
	make purge conan configure-{gba,release,debug} install
