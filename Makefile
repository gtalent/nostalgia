OS=$(shell uname | tr [:upper:] [:lower:])
HOST_ENV=${OS}-$(shell uname -m)
DEVENV=devenv$(shell pwd | sed 's/\//-/g')
DEVENV_IMAGE=wombatant/nostalgia-devenv
ifneq ($(shell which docker),)
	ifeq ($(shell docker inspect --format="{{.State.Status}}" ${DEVENV} 2>&1),running)
		ENV_RUN=docker exec -i -t --user $(shell id -u ${USER}) ${DEVENV}
	endif
endif

make:
	${ENV_RUN} make -j -C build HOST_ENV=${HOST_ENV}
preinstall:
	${ENV_RUN} make -j -C build ARGS="preinstall" HOST_ENV=${HOST_ENV}
install:
	${ENV_RUN} make -j -C build ARGS="install" HOST_ENV=${HOST_ENV}
clean:
	${ENV_RUN} make -j -C build ARGS="clean" HOST_ENV=${HOST_ENV}
purge:
	${ENV_RUN} rm -rf $$(find build -mindepth 1 -maxdepth 1 -type d)
test:
	${ENV_RUN} make -j -C build ARGS="test" HOST_ENV=${HOST_ENV}

run: make
	./build/current/src/player/nostalgia -debug
gba-run: make
	${ENV_RUN} mgba-qt build/current/src/player/nostalgia.bin
gdb: make
	gdb ./build/current/src/wombat/wombat

devenv:
	docker pull ${DEVENV_IMAGE}
	docker run -d -v $(shell pwd):/usr/src/project \
		-e LOCAL_USER_ID=$(shell id -u ${USER}) \
		-e DISPLAY=$(DISPLAY) \
		-e QT_AUTO_SCREEN_SCALE_FACTOR=1 \
		-v /tmp/.X11-unix:/tmp/.X11-unix \
		-v /run/dbus/:/run/dbus/ \
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
	${ENV_RUN} ./scripts/setup_build ${HOST_ENV}
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s ${HOST_ENV}-release build/current

debug:
	${ENV_RUN} rm -rf build/${HOST_ENV}-debug
	${ENV_RUN} ./scripts/setup_build ${HOST_ENV} debug
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s ${HOST_ENV}-debug build/current

windows:
	${ENV_RUN} rm -rf build/windows
	${ENV_RUN} ./scripts/setup_build windows
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s windows build/current

windows-debug:
	${ENV_RUN} rm -rf build/windows
	${ENV_RUN} ./scripts/setup_build windows debug
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s windows build/current

gba:
	${ENV_RUN} rm -rf build/gba-release
	${ENV_RUN} ./scripts/setup_build gba
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s gba-release build/current

gba-debug:
	${ENV_RUN} rm -rf build/gba-debug
	${ENV_RUN} ./scripts/setup_build gba debug
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s gba-debug build/current
