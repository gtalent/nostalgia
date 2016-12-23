all: gba_build native_build native_debug_build windows_release windows_debug

MAKE=make -j

gba_build:
	@if [ -d gba-release ]; then \
		${MAKE} -C gba-release ${ARGS}; \
	fi

native_build:
	@if [ -d ${TARGET}-release ]; then \
		${MAKE} -C ${TARGET}-release ${ARGS}; \
	fi

native_debug_build:
	@if [ -d ${TARGET}-debug ]; then \
		${MAKE} -C ${TARGET}-debug ${ARGS}; \
	fi

windows_release:
	@if [ -d windows-release ]; then \
		${MAKE} -C windows-release ${ARGS}; \
	fi

windows_debug:
	@if [ -d windows-debug ]; then \
		${MAKE} -C windows-debug ${ARGS}; \
	fi
