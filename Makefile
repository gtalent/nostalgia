OS=$(shell uname | tr [:upper:] [:lower:])
HOST_ENV=${OS}-$(shell uname -m)
DEVENV=devenv$(shell pwd | sed 's/\//-/g')
DEVENV_IMAGE=wombatant/devenv
ifneq ($(shell which gmake),)
	MAKE=gmake -s
else
	MAKE=make -s
endif
ifneq ($(shell which docker 2>&1),)
	ifeq ($(shell docker inspect --format="{{.State.Status}}" ${DEVENV} 2>&1),running)
		ENV_RUN=docker exec -i -t --user $(shell id -u ${USER}) ${DEVENV}
	endif
endif

make:
	${ENV_RUN} ${MAKE} -j -C build HOST_ENV=${HOST_ENV}
preinstall:
	${ENV_RUN} ${MAKE} -j -C build ARGS="preinstall" HOST_ENV=${HOST_ENV}
install:
	${ENV_RUN} ${MAKE} -j -C build ARGS="install" HOST_ENV=${HOST_ENV}
clean:
	${ENV_RUN} ${MAKE} -j -C build ARGS="clean" HOST_ENV=${HOST_ENV}
purge:
	${ENV_RUN} rm -rf $(shell find build -mindepth 1 -maxdepth 1 -type d)
test:
	${ENV_RUN} ${MAKE} -j -C build ARGS="test" HOST_ENV=${HOST_ENV}
run: make
	./build/current/src/wombat/wombat -debug
gdb: make
	gdb ./build/current/src/wombat/wombat

devenv:
	docker pull ${DEVENV_IMAGE}
	docker run -d -v $(shell pwd):/usr/src/project \
		-e LOCAL_USER_ID=$(shell id -u ${USER}) \
		--name ${DEVENV} -t ${DEVENV_IMAGE} bash
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
