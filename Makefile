OS=$(shell uname | tr [:upper:] [:lower:])
HOST_ENV=${OS}-$(shell uname -m)
DEVENV=devenv$(shell pwd | sed 's/\//-/g')
DEVENV_IMAGE=nostalgia-devenv
CURRENT_BUILD=$(file < .current_build)
ifneq ($(shell which docker 2> /dev/null),)
	ifeq ($(shell docker inspect --format="{{.State.Status}}" ${DEVENV} 2>&1),running)
		ENV_RUN=docker exec -i -t --user $(shell id -u ${USER}) ${DEVENV}
	endif
endif

ifeq ($(OS),windows)
	RM_RF=Remove-Item -ErrorAction Ignore -Path -Recurse
else
	RM_RF=rm -rf
endif

ifeq ($(OS),darwin)
	NOSTALGIA_STUDIO=./dist/${CURRENT_BUILD}/nostalgia-studio.app/Contents/MacOS/nostalgia-studio
	NOSTALGIA_STUDIO_PROFILE=dist/${CURRENT_BUILD}/nostalgia-studio.app/Contents/Resources/nostalgia-studio.json
	MGBA=/Applications/mGBA.app/Contents/MacOS/mGBA
else
	NOSTALGIA_STUDIO=./dist/${CURRENT_BUILD}/bin/nostalgia-studio
	NOSTALGIA_STUDIO_PROFILE=dist/${CURRENT_BUILD}/share/nostalgia-studio.json
	MGBA=mgba-qt
endif

.PHONY: build
build:
	$(foreach file, $(wildcard build/*), cmake --build $(file) --target;)
.PHONY: pkg-gba
pkg-gba:
	$(foreach file, $(wildcard build/*), cmake --build $(file) --target install;)
	${ENV_RUN} ./scripts/gba-pkg
.PHONY: install
install:
	$(foreach file, $(wildcard build/*), cmake --build $(file) --target install;)
.PHONY: clean
clean:
	$(foreach file, $(wildcard build/*), cmake --build $(file) --target clean;)
.PHONY: purge
purge:
	${ENV_RUN} ${RM_RF} build .current_build
.PHONY: test
test:
	$(foreach file, $(wildcard build/*), cmake --build $(file) --target test;)

.PHONY: run
run: install
	${ENV_RUN} ./dist/${CURRENT_BUILD}/bin/nostalgia sample_project
.PHONY: run-studio
run-studio: install
	${ENV_RUN} ${NOSTALGIA_STUDIO} -profile ${NOSTALGIA_STUDIO_PROFILE}
.PHONY: gba-run
gba-run: pkg-gba
	${MGBA} nostalgia.gba
.PHONY: gdb
gdb: install
	${ENV_RUN} gdb --args ./dist/${CURRENT_BUILD}/bin/nostalgia sample_project
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
	${ENV_RUN} ${RM_RF} build/${HOST_ENV}-release
	${ENV_RUN} ./scripts/setup-build ${HOST_ENV} release

.PHONY: configure-debug
configure-debug:
	${ENV_RUN} ${RM_RF} build/${HOST_ENV}-debug
	${ENV_RUN} ./scripts/setup-build ${HOST_ENV} debug

.PHONY: configure-asan
configure-asan:
	${ENV_RUN} ${RM_RF} build/${HOST_ENV}-asan
	${ENV_RUN} ./scripts/setup-build ${HOST_ENV} asan

.PHONY: configure-gba
configure-gba:
	${ENV_RUN} ${RM_RF} build/gba-release
	${ENV_RUN} ./scripts/setup-build gba release

.PHONY: configure-gba-debug
configure-gba-debug:
	${ENV_RUN} ${RM_RF} build/gba-debug
	${ENV_RUN} ./scripts/setup-build gba debug
