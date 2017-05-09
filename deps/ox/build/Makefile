ifneq ($(shell which gmake),)
	MAKE=gmake -j
else
	MAKE=make
endif

all: gba_build native_build native_debug_build windows_release windows_debug

gba_build:
	@if [ -d gba-release ]; then \
		${MAKE} -C gba-release ${ARGS}; \
	fi

native_build:
	@if [ -d ${HOST_ENV}-release ]; then \
		${MAKE} -C ${HOST_ENV}-release ${ARGS}; \
	fi

native_debug_build:
	@if [ -d ${HOST_ENV}-debug ]; then \
		${MAKE} -C ${HOST_ENV}-debug ${ARGS}; \
	fi

windows_release:
	@if [ -d windows-release ]; then \
		${MAKE} -C windows-release ${ARGS}; \
	fi

windows_debug:
	@if [ -d windows-debug ]; then \
		${MAKE} -C windows-debug ${ARGS}; \
	fi
