all: gba_build gba_debug_build native_build native_debug_build native_asan_build windows_release windows_debug

ifneq ($(shell which gmake),)
MAKE="gmake"
else
MAKE="make"
endif

gba_build:
	@if [ -d gba-release ]; then \
		${MAKE} -C gba-release ${ARGS}; \
	fi

gba_debug_build:
	@if [ -d gba-debug ]; then \
		${MAKE} -C gba-debug ${ARGS}; \
	fi

native_build:
	@if [ -d ${HOST_ENV}-release ]; then \
		${MAKE} -C ${HOST_ENV}-release ${ARGS}; \
	fi

native_debug_build:
	@if [ -d ${HOST_ENV}-debug ]; then \
		${MAKE} -C ${HOST_ENV}-debug ${ARGS}; \
	fi

native_asan_build:
	@if [ -d ${HOST_ENV}-asan ]; then \
		${MAKE} -C ${HOST_ENV}-asan ${ARGS}; \
	fi

windows_release:
	@if [ -d windows-release ]; then \
		${MAKE} -C windows-release ${ARGS}; \
	fi

windows_debug:
	@if [ -d windows-debug ]; then \
		${MAKE} -C windows-debug ${ARGS}; \
	fi
