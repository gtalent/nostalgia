ifeq (${OS},Windows_NT)
	SHELL := powershell.exe
	.SHELLFLAGS := -NoProfile -Command
	OS=windows
	HOST_ENV=${OS}
else
	OS=$(shell uname | tr [:upper:] [:lower:])
	HOST_ENV=${OS}-$(shell uname -m)
endif

DEVENV=devenv$(shell pwd | sed 's/\//-/g')
DEVENV_IMAGE=nostalgia-devenv
SETUP_BUILD=python3 ./scripts/setup-build.py
PYBB=python3 scripts/pybb.py
CMAKE_BUILD=${PYBB} cmake-build
RM_RF=${PYBB} rm
ifndef VCPKG_DIR_BASE
	VCPKG_DIR_BASE=.vcpkg
endif
VCPKG_VERSION=2020.06
VCPKG_DIR=$(VCPKG_DIR_BASE)/$(VCPKG_VERSION)-$(HOST_ENV)
CURRENT_BUILD=$(HOST_ENV)-$(file < .current_build)
ifneq ($(shell which docker 2> /dev/null),)
	ifeq ($(shell docker inspect --format="{{.State.Status}}" ${DEVENV} 2>&1),running)
		ENV_RUN=docker exec -i -t --user $(shell id -u ${USER}) ${DEVENV}
	endif
endif

ifeq ($(OS),darwin)
	NOSTALGIA_STUDIO=./dist/${CURRENT_BUILD}/nostalgia-studio.app/Contents/MacOS/nostalgia-studio
	MGBA=/Applications/mGBA.app/Contents/MacOS/mGBA
else
	NOSTALGIA_STUDIO=./dist/${CURRENT_BUILD}/bin/nostalgia-studio
	MGBA=mgba-qt
endif

.PHONY: build
build:
	${ENV_RUN} ${CMAKE_BUILD} build
.PHONY: pkg-gba
pkg-gba:
	${ENV_RUN} ${CMAKE_BUILD} build install
	${ENV_RUN} ./scripts/pkg-gba sample_project
.PHONY: install
install:
	${ENV_RUN} ${CMAKE_BUILD} build install
.PHONY: clean
clean:
	${ENV_RUN} ${CMAKE_BUILD} build clean
.PHONY: purge
purge:
	${ENV_RUN} ${RM_RF} .current_build
	${ENV_RUN} ${RM_RF} build
	${ENV_RUN} ${RM_RF} dist
.PHONY: test
test: build
	$(foreach file, $(wildcard build/*), ${ENV_RUN} cmake --build $(file) --target test;)

.PHONY: run
run: install
	${ENV_RUN} ./dist/${CURRENT_BUILD}/bin/nostalgia sample_project
.PHONY: run-studio
run-studio: install
	${ENV_RUN} ${NOSTALGIA_STUDIO}
.PHONY: gba-run
gba-run: pkg-gba
	${MGBA} nostalgia.gba
.PHONY: gdb
gdb: install
	${ENV_RUN} gdb --args ./dist/${CURRENT_BUILD}/bin/nostalgia sample_project
.PHONY: gdb-studio
gdb-studio: install
	${ENV_RUN} gdb --args ${NOSTALGIA_STUDIO}

.PHONY: devenv-image
devenv-image:
	docker build . -t ${DEVENV_IMAGE}
.PHONY: devenv-create
devenv-create:
	docker run -d \
		-e LOCAL_USER_ID=$(shell id -u ${USER}) \
		-e DISPLAY=$(DISPLAY) \
		-e QT_AUTO_SCREEN_SCALE_FACTOR=1 \
		-v /tmp/.X11-unix:/tmp/.X11-unix \
		-v /run/dbus/:/run/dbus/ \
		-v $(shell pwd):/usr/src/project \
		-v /dev/shm:/dev/shm \
		--restart=always \
		--name ${DEVENV} \
		-t ${DEVENV_IMAGE} bash
.PHONY: devenv-destroy
devenv-destroy:
	docker rm -f ${DEVENV}
.PHONY: devenv-shell
devenv-shell:
	${ENV_RUN} bash

.PHONY: vcpkg
vcpkg: ${VCPKG_DIR} vcpkg-install

${VCPKG_DIR}:
	${ENV_RUN} ${RM_RF} ${VCPKG_DIR}
	${ENV_RUN} mkdir -p ${VCPKG_DIR_BASE}
	${ENV_RUN} git clone -b release --depth 1 --branch ${VCPKG_VERSION} https://github.com/microsoft/vcpkg.git ${VCPKG_DIR}
ifneq (${OS},windows)
	${ENV_RUN} ${VCPKG_DIR}/bootstrap-vcpkg.sh
else
	${ENV_RUN} ${VCPKG_DIR}/bootstrap-vcpkg.bat
endif

.PHONY: vcpkg-install
vcpkg-install:
ifneq (${OS},windows)
	${VCPKG_DIR}/vcpkg install sdl2 jsoncpp
else
	${VCPKG_DIR}/vcpkg install --triplet x64-windows sdl2 jsoncpp
endif

.PHONY: configure-xcode
configure-xcode:
	${ENV_RUN} ${SETUP_BUILD} --vcpkg_dir ${VCPKG_DIR} --build_tool xcode

.PHONY: configure-release
configure-release:
	${ENV_RUN} ${SETUP_BUILD} --vcpkg_dir ${VCPKG_DIR} --build_type release

.PHONY: configure-debug
configure-debug:
	${ENV_RUN} ${SETUP_BUILD} --vcpkg_dir ${VCPKG_DIR} --build_type debug

.PHONY: configure-asan
configure-asan:
	${ENV_RUN} ${SETUP_BUILD} --vcpkg_dir ${VCPKG_DIR} --build_type asan

.PHONY: configure-gba
configure-gba:
	${ENV_RUN} ${SETUP_BUILD} --target gba --build_type release

.PHONY: configure-gba-debug
configure-gba-debug:
	${ENV_RUN} ${SETUP_BUILD} --target gba --build_type debug
