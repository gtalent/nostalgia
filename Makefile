DEVENV=devenv-$(shell pwd | sed 's/\//-/g')

make:
	docker exec ${DEVENV} make -j -C build
preinstall:
	docker exec ${DEVENV} make -j -C build ARGS="preinstall"
install:
	docker exec ${DEVENV} make -j -C build ARGS="install"
clean:
	docker exec ${DEVENV} make -j -C build ARGS="clean"
test:
	docker exec ${DEVENV} make -j -C build ARGS="test"
run: make
	./build/current/src/wombat/wombat -debug
debug: make
	gdb ./build/current/src/wombat/wombat
devenv:
	docker pull wombatant/devenv
	docker rm -f ${DEVENV}
	docker run -d -v $(shell pwd):/usr/src/project -e LOCAL_USER_ID=$(shell id -u ${USER}) --name ${DEVENV} -t wombatant/devenv bash
devenv-destroy:
	docker rm -f ${DEVENV}

sdl:
	docker exec ${DEVENV} ./scripts/setup_build
	rm -f build/current
	ln -s sdl build/current

sdl_debug:
	docker exec ${DEVENV} ./scripts/setup_build_debug
	rm -f build/current
	ln -s sdl_debug build/current

gba:
	docker exec ${DEVENV} ./scripts/setup_build_gba
	rm -f build/current
	ln -s gba build/current
