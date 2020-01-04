OS=$(shell uname | tr [:upper:] [:lower:])
HOST_ENV=${OS}-$(shell uname -m)
DEVENV=devenv$(shell pwd | sed 's/\//-/g')
DEVENV_IMAGE=nostalgia-devenv
ifneq ($(shell which gmake 2> /dev/null),)
	MAKE=gmake -s
else
	MAKE=make -s
endif
ifneq ($(shell which docker 2> /dev/null),)
	ifeq ($(shell docker inspect --format="{{.State.Status}}" ${DEVENV} 2>&1),running)
		ENV_RUN=docker exec -i -t --user $(shell id -u ${USER}) ${DEVENV}
	endif
endif

ifeq ($(OS),darwin)
	NOSTALGIA_STUDIO=./dist/current/nostalgia-studio.app/Contents/MacOS/nostalgia-studio
	NOSTALGIA_STUDIO_PROFILE=dist/current/nostalgia-studio.app/Contents/Resources/nostalgia-studio.json
	MGBA=/Applications/mGBA.app/Contents/MacOS/mGBA
else
	NOSTALGIA_STUDIO=./dist/current/bin/nostalgia-studio
	NOSTALGIA_STUDIO_PROFILE=dist/current/share/nostalgia-studio.json
	MGBA=mgba-qt
endif

.PHONY: build
build:
	${ENV_RUN} ./scripts/run-make build
.PHONY: pkg-gba
pkg-gba:
	${ENV_RUN} ./scripts/run-make build install
	${ENV_RUN} ./scripts/gba-pkg
.PHONY: preinstall
preinstall:
	${ENV_RUN} ./scripts/run-make build preinstall
.PHONY: install
install:
	${ENV_RUN} ./scripts/run-make build install
.PHONY: clean
clean:
	${ENV_RUN} ./scripts/run-make build clean
.PHONY: purge
purge:
	${ENV_RUN} rm -rf build
.PHONY: test
test:
	${ENV_RUN} ./scripts/run-make build test

.PHONY: run
run: install
	${ENV_RUN} ./dist/current/bin/nostalgia sample_project
.PHONY: run-studio
run-studio: install
	${ENV_RUN} ${NOSTALGIA_STUDIO} -profile ${NOSTALGIA_STUDIO_PROFILE}
.PHONY: gba-run
gba-run: pkg-gba
	${MGBA} nostalgia.gba
.PHONY: gdb
gdb: install
	${ENV_RUN} gdb --args ./dist/current/bin/nostalgia sample_project
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

.PHONY: conan
conan:
	@mkdir -p conanbuild && cd conanbuild && conan install ../

.PHONY: configure-release
configure-release:
	${ENV_RUN} rm -rf build/${HOST_ENV}-release
	${ENV_RUN} ./scripts/setup-build ${HOST_ENV} release

.PHONY: configure-debug
configure-debug:
	${ENV_RUN} rm -rf build/${HOST_ENV}-debug
	${ENV_RUN} ./scripts/setup-build ${HOST_ENV} debug

.PHONY: configure-asan
configure-asan:
	${ENV_RUN} rm -rf build/${HOST_ENV}-asan
	${ENV_RUN} ./scripts/setup-build ${HOST_ENV} asan

.PHONY: configure-gba
configure-gba:
	${ENV_RUN} rm -rf build/gba-release
	${ENV_RUN} ./scripts/setup-build gba release

.PHONY: configure-gba-debug
configure-gba-debug:
	${ENV_RUN} rm -rf build/gba-debug
	${ENV_RUN} ./scripts/setup-build gba debug
