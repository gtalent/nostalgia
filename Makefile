PROJECT_NAME=nostalgia
BUILDCORE_PATH=deps/buildcore
VCPKG_PKGS=sdl2 jsoncpp
include ${BUILDCORE_PATH}/base.mk

ifeq ($(OS),darwin)
	NOSTALGIA_STUDIO=./dist/${CURRENT_BUILD}/nostalgia-studio.app/Contents/MacOS/nostalgia-studio
	MGBA=/Applications/mGBA.app/Contents/MacOS/mGBA
else
	NOSTALGIA_STUDIO=./dist/${CURRENT_BUILD}/bin/nostalgia-studio
	MGBA=mgba-qt
endif

.PHONY: pkg-gba
pkg-gba:
	${ENV_RUN} ${CMAKE_BUILD} build install
	${ENV_RUN} ./scripts/pkg-gba sample_project

.PHONY: run
run: install
	${ENV_RUN} ./dist/${CURRENT_BUILD}/bin/nostalgia sample_project
.PHONY: run-studio
run-studio: install
	${ENV_RUN} ${NOSTALGIA_STUDIO}
.PHONY: gba-run
gba-run: pkg-gba
	${MGBA} nostalgia.gba
.PHONY: debug
debug: install
	${ENV_RUN} gdb --args ./dist/${CURRENT_BUILD}/bin/nostalgia sample_project
.PHONY: debug-studio
debug-studio: install
	${ENV_RUN} gdb --args ${NOSTALGIA_STUDIO}

.PHONY: configure-gba
configure-gba:
	${ENV_RUN} ${SETUP_BUILD} --toolchain=cmake/modules/GBA.cmake --target=gba --current_build=0 --build_type=release

.PHONY: configure-gba-debug
configure-gba-debug:
	${ENV_RUN} ${SETUP_BUILD} --toolchain=cmake/modules/GBA.cmake --target=gba --current_build=0 --build_type=debug
