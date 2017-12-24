OS=$(shell uname | tr [:upper:] [:lower:])
HOST_ENV=${OS}-$(shell uname -m)
DEVENV=devenv$(shell pwd | sed 's/\//-/g')
DEVENV_IMAGE=nostalgia-devenv
ifneq ($(shell which gmake),)
	MAKE=gmake -s
else
	MAKE=make -s
endif
ifneq ($(shell which docker),)
	ifeq ($(shell docker inspect --format="{{.State.Status}}" ${DEVENV} 2>&1),running)
		ENV_RUN=docker exec -i -t --user $(shell id -u ${USER}) ${DEVENV}
	endif
endif

make:
	${ENV_RUN} ./scripts/run-make build
gba-pkg:
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
	${ENV_RUN} rm -rf $$(find build -mindepth 1 -maxdepth 1 -type d) dist
test:
	${ENV_RUN} ./scripts/run-make build test

run: install
	./dist/current/bin/nostalgia -debug
run-studio: install
	./dist/current/bin/nostalgia-studio -profile dist/current/share/nostalgia-studio.json
gba-run: gba-pkg
	mgba-qt nostalgia.gba
gdb: make
	gdb ./build/current/src/wombat/wombat
gdb-studio: make
	gdb "./dist/current/bin/nostalgia-studio -profile dist/current/share/nostalgia-studio.json"

devenv-image:
	docker build . -t ${DEVENV_IMAGE}
devenv:
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

shell:
	${ENV_RUN} bash

release:
	${ENV_RUN} rm -rf build/${HOST_ENV}-release
	${ENV_RUN} ./scripts/setup_build ${HOST_ENV} release
	${ENV_RUN} rm -f build/current

debug:
	${ENV_RUN} rm -rf build/${HOST_ENV}-debug
	${ENV_RUN} ./scripts/setup_build ${HOST_ENV} debug
	${ENV_RUN} rm -f build/current

asan:
	${ENV_RUN} rm -rf build/${HOST_ENV}-asan
	${ENV_RUN} ./scripts/setup_build ${HOST_ENV} asan
	${ENV_RUN} rm -f build/current

windows:
	${ENV_RUN} rm -rf build/windows
	${ENV_RUN} ./scripts/setup_build windows
	${ENV_RUN} rm -f build/current

windows-debug:
	${ENV_RUN} rm -rf build/windows
	${ENV_RUN} ./scripts/setup_build windows debug
	${ENV_RUN} rm -f build/current

gba:
	${ENV_RUN} rm -rf build/gba-release
	${ENV_RUN} ./scripts/setup_build gba release
	${ENV_RUN} rm -f build/current

gba-debug:
	${ENV_RUN} rm -rf build/gba-debug
	${ENV_RUN} ./scripts/setup_build gba debug
	${ENV_RUN} rm -f build/current
