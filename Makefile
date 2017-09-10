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
	${ENV_RUN} ${MAKE} -j -C build HOST_ENV=${HOST_ENV}
build_rom:
	${ENV_RUN} ${MAKE} -j -C build ARGS="install" HOST_ENV=${HOST_ENV}
	${ENV_RUN} ${MAKE} -j -C build HOST_ENV=${HOST_ENV}
	${ENV_RUN} ./scripts/build_rom.sh
preinstall:
	${ENV_RUN} ${MAKE} -j -C build ARGS="preinstall" HOST_ENV=${HOST_ENV}
install:
	${ENV_RUN} ${MAKE} -j -C build ARGS="install" HOST_ENV=${HOST_ENV}
clean:
	${ENV_RUN} ${MAKE} -j -C build ARGS="clean" HOST_ENV=${HOST_ENV}
purge:
	${ENV_RUN} rm -rf $$(find build -mindepth 1 -maxdepth 1 -type d) dist
test:
	${ENV_RUN} ${MAKE} -j -C build ARGS="test" HOST_ENV=${HOST_ENV}

run: install
	./dist/current/bin/nostalgia -debug
run-studio: install
	./dist/current/bin/nostalgia-studio -profile dist/current/share/nostalgia-studio.json
gba-run: make
	${ENV_RUN} mgba-qt build/current/src/player/nostalgia.bin
gdb: make
	gdb ./build/current/src/wombat/wombat
gdb-studio: make
	gdb "./dist/current/bin/nostalgia-studio -profile dist/current/share/nostalgia-studio.json"

devenv-build:
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
