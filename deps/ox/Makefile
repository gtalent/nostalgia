OS=$(shell uname | tr [:upper:] [:lower:])
HOST_ENV=${OS}-$(shell uname -m)
DEVENV=devenv$(shell pwd | sed 's/\//-/g')
DEVENV_IMAGE=wombatant/devenv
ifneq ($(shell which docker 2>&1),)
	ifeq ($(shell docker inspect --format="{{.State.Status}}" ${DEVENV} 2>&1),running)
		ENV_RUN=docker exec -i -t --user $(shell id -u ${USER}) ${DEVENV}
	endif
endif

make:
	${ENV_RUN} ./scripts/run-make build
preinstall:
	${ENV_RUN} ./scripts/run-make build preinstall
install:
	${ENV_RUN} ./scripts/run-make build install
clean:
	${ENV_RUN} ./scripts/run-make build clean
purge:
	${ENV_RUN} rm -rf $(shell find build -mindepth 1 -maxdepth 1 -type d)
test:
	${ENV_RUN} ./scripts/run-make build test

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

asan:
	${ENV_RUN} rm -rf build/${HOST_ENV}-asan
	${ENV_RUN} ./scripts/setup_build ${HOST_ENV} asan
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s ${HOST_ENV}-asan build/current

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
