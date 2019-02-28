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

all:
	${ENV_RUN} ./scripts/run-make build
pkg-gba:
	${ENV_RUN} ./scripts/run-make build install
	${ENV_RUN} ./scripts/run-make build
	${ENV_RUN} ./scripts/gba-pkg
preinstall:
	${ENV_RUN} ./scripts/run-make build preinstall
install:
	${ENV_RUN} ./scripts/run-make build install
clean:
	${ENV_RUN} ./scripts/run-make build clean
purge:
	${ENV_RUN} rm -rf build
test:
	${ENV_RUN} ./scripts/run-make build test

run: install
	${ENV_RUN} ./dist/current/bin/nostalgia -debug
run-studio: install
	${ENV_RUN} ./dist/current/bin/nostalgia-studio -profile dist/current/share/nostalgia-studio.json
gba-run: pkg-gba
	mgba-qt nostalgia.gba
gdb: make
	${ENV_RUN} gdb ./build/current/src/wombat/wombat
gdb-studio: install
	${ENV_RUN} gdb --args ./dist/current/bin/nostalgia-studio -profile dist/current/share/nostalgia-studio.json

devenv-image:
	docker build . -t ${DEVENV_IMAGE}
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
devenv-destroy:
	docker rm -f ${DEVENV}
devenv-shell:
	${ENV_RUN} bash

setup-release:
	${ENV_RUN} rm -rf build/${HOST_ENV}-release
	${ENV_RUN} ./scripts/setup-build ${HOST_ENV} release

setup-debug:
	${ENV_RUN} rm -rf build/${HOST_ENV}-debug
	${ENV_RUN} ./scripts/setup-build ${HOST_ENV} debug

setup-asan:
	${ENV_RUN} rm -rf build/${HOST_ENV}-asan
	${ENV_RUN} ./scripts/setup-build ${HOST_ENV} asan

setup-windows:
	${ENV_RUN} rm -rf build/windows
	${ENV_RUN} ./scripts/setup-build windows

setup-windows-debug:
	${ENV_RUN} rm -rf build/windows
	${ENV_RUN} ./scripts/setup-build windows debug

setup-gba:
	${ENV_RUN} rm -rf build/gba-release
	${ENV_RUN} ./scripts/setup-build gba release

setup-gba-debug:
	${ENV_RUN} rm -rf build/gba-debug
	${ENV_RUN} ./scripts/setup-build gba debug
