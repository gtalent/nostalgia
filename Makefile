DEVENV=devenv$(shell pwd | sed 's/\//-/g')
ifeq ($(shell docker inspect --format="{{.State.Status}}" ${DEVENV} 2>&1),running)
	ENV_RUN=docker exec --user $(shell id -u ${USER}) ${DEVENV}
endif

make:
	${ENV_RUN} make -j -C build
preinstall:
	${ENV_RUN} make -j -C build ARGS="preinstall"
install:
	${ENV_RUN} make -j -C build ARGS="install"
clean:
	${ENV_RUN} make -j -C build ARGS="clean"
test:
	${ENV_RUN} make -j -C build ARGS="test"
run: make
	./build/current/src/wombat/wombat -debug
debug: make
	gdb ./build/current/src/wombat/wombat
devenv:
	docker pull wombatant/devenv
	docker run -d -v $(shell pwd):/usr/src/project \
		-e LOCAL_USER_ID=$(shell id -u ${USER}) \
		--name ${DEVENV} -t wombatant/devenv bash
devenv-destroy:
	docker rm -f ${DEVENV}

sdl:
	${ENV_RUN} ./scripts/setup_build
	rm -f build/current
	ln -s sdl build/current

sdl_debug:
	${ENV_RUN} ./scripts/setup_build_debug
	rm -f build/current
	ln -s sdl_debug build/current

gba:
	${ENV_RUN} ./scripts/setup_build_gba
	rm -f build/current
	ln -s gba build/current
