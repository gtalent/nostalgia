ifeq (${OS},Windows_NT)
	SHELL := powershell.exe
	.SHELLFLAGS := -NoProfile -Command
	OS=windows
	HOST_ENV=${OS}
	RM_RF=Remove-Item -ErrorAction Ignore -Recurse -Path
else
	OS=$(shell uname | tr [:upper:] [:lower:])
	HOST_ENV=${OS}-$(shell uname -m)
	RM_RF=rm -rf
endif

DEVENV=devenv$(shell pwd | sed 's/\//-/g')
DEVENV_IMAGE=nostalgia-devenv
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
	NOSTALGIA_STUDIO_PROFILE=dist/${CURRENT_BUILD}/nostalgia-studio.app/Contents/Resources/nostalgia-studio.json
	MGBA=/Applications/mGBA.app/Contents/MacOS/mGBA
else
	NOSTALGIA_STUDIO=./dist/${CURRENT_BUILD}/bin/nostalgia-studio
	NOSTALGIA_STUDIO_PROFILE=dist/${CURRENT_BUILD}/share/nostalgia-studio.json
	MGBA=mgba-qt
endif

.PHONY: build
build:
	$(foreach file, $(wildcard build/*), ${ENV_RUN} cmake --build $(file) --target;)
.PHONY: pkg-gba
pkg-gba:
	$(foreach file, $(wildcard build/*), ${ENV_RUN} cmake --build $(file) --target install;)
	${ENV_RUN} ./scripts/pkg-gba sample_project
.PHONY: install
install:
	$(foreach file, $(wildcard build/*), ${ENV_RUN} cmake --build $(file) --target install;)
.PHONY: clean
clean:
	$(foreach file, $(wildcard build/*), ${ENV_RUN} cmake --build $(file) --target clean;)
.PHONY: purge
purge:
	${ENV_RUN} ${RM_RF} build .current_build dist
.PHONY: test
test: build
	$(foreach file, $(wildcard build/*), ${ENV_RUN} cmake --build $(file) --target test;)

.PHONY: run
run: install
	${ENV_RUN} ./dist/${CURRENT_BUILD}/bin/nostalgia sample_project
.PHONY: run-studio
run-studio: install
	${ENV_RUN} ${NOSTALGIA_STUDIO} -profile ${NOSTALGIA_STUDIO_PROFILE}
.PHONY: gba-run
gba-run: pkg-gba
	${MGBA} nostalgia.gba
.PHONY: gdb
gdb: install
	${ENV_RUN} gdb --args ./dist/${CURRENT_BUILD}/bin/nostalgia sample_project
.PHONY: gdb-studio
gdb-studio: install
	${ENV_RUN} gdb --args ${NOSTALGIA_STUDIO} -profile ${NOSTALGIA_STUDIO_PROFILE}

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
ifneq (,$(wildcard ${VCPKG_DIR}))
	${ENV_RUN} ${RM_RF} ${VCPKG_DIR}
endif
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

.PHONY: configure-release
configure-release:
ifneq (,$(wildcard build/${HOST_ENV}-release))
	${ENV_RUN} ${RM_RF} build/${HOST_ENV}-release
endif
	${ENV_RUN} python ./scripts/setup-build.py ${HOST_ENV} release ${VCPKG_DIR}

.PHONY: configure-debug
configure-debug:
ifneq (,$(wildcard build/${HOST_ENV}-debug))
	${ENV_RUN} ${RM_RF} build/${HOST_ENV}-debug
endif
	${ENV_RUN} python ./scripts/setup-build.py ${HOST_ENV} debug ${VCPKG_DIR}

.PHONY: configure-asan
configure-asan:
ifneq (,$(wildcard build/${HOST_ENV}-asan))
	${ENV_RUN} ${RM_RF} build/${HOST_ENV}-asan
endif
	${ENV_RUN} python ./scripts/setup-build.py ${HOST_ENV} asan ${VCPKG_DIR}

.PHONY: configure-gba
configure-gba:
ifneq (,$(wildcard build/gba-release))
	${ENV_RUN} ${RM_RF} build/gba-release
endif
	${ENV_RUN} python ./scripts/setup-build.py gba release ${VCPKG_DIR}

.PHONY: configure-gba-debug
configure-gba-debug:
ifneq (,$(wildcard build/gba-debug))
	${ENV_RUN} ${RM_RF} build/gba-debug
endif
	${ENV_RUN} python ./scripts/setup-build.py gba debug ${VCPKG_DIR}
