OS=$(shell uname | tr [:upper:] [:lower:])
TARGET=${OS}-$(shell uname -m)
DEVENV=devenv$(shell pwd | sed 's/\//-/g')
ifeq ($(shell docker inspect --format="{{.State.Status}}" ${DEVENV} 2>&1),running)
	ENV_RUN=docker exec --user $(shell id -u ${USER}) ${DEVENV}
endif

make:
	${ENV_RUN} make -j -C build TARGET=${TARGET}
preinstall:
	${ENV_RUN} make -j -C build ARGS="preinstall" TARGET=${TARGET}
install:
	${ENV_RUN} make -j -C build ARGS="install" TARGET=${TARGET}
clean:
	${ENV_RUN} make -j -C build ARGS="clean" TARGET=${TARGET}
purge:
	${ENV_RUN} rm -rf $(shell find build -mindepth 1 -maxdepth 1 -type d)
test:
	${ENV_RUN} make -j -C build ARGS="test" TARGET=${TARGET}
run: make
	./build/current/src/wombat/wombat -debug
gdb: make
	gdb ./build/current/src/wombat/wombat
devenv:
	docker pull wombatant/devenv
	docker run -d -v $(shell pwd):/usr/src/project \
		-e LOCAL_USER_ID=$(shell id -u ${USER}) \
		--name ${DEVENV} -t wombatant/devenv bash
devenv-destroy:
	docker rm -f ${DEVENV}
devenv-shell:
	docker exec -i --user $(shell id -u ${USER}) ${DEVENV} ls /usr/bin/x86_64-w64-mingw32-g++

release:
	${ENV_RUN} rm -rf build/${TARGET}-release
	${ENV_RUN} ./scripts/setup_build ${TARGET}
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s ${TARGET}-release build/current

debug:
	${ENV_RUN} rm -rf build/${TARGET}-debug
	${ENV_RUN} ./scripts/setup_build ${TARGET} debug
	${ENV_RUN} rm -f build/current
	${ENV_RUN} ln -s ${TARGET}-debug build/current

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
