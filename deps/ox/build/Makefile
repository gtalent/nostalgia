all: gba_build native_build native_debug_build windows_release windows_debug

gba_build:
	@if [ -d gba-release ]; then \
		cmake --build gba-release --target ${ARGS}; \
	fi

native_build:
	@if [ -d ${HOST_ENV}-release ]; then \
		cmake --build ${HOST_ENV}-release --target ${ARGS}; \
	fi

native_debug_build:
	@if [ -d ${HOST_ENV}-debug ]; then \
		cmake --build ${HOST_ENV}-debug --target ${ARGS}; \
	fi

windows_release:
	@if [ -d windows-release ]; then \
		cmake --build windows-release --target ${ARGS}; \
	fi

windows_debug:
	@if [ -d windows-debug ]; then \
		cmake --build windows-debug --target ${ARGS}; \
	fi
