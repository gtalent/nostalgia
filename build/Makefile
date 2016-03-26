all: gba_build sdl_build sdl_debug_build

gba_build:
	@if [ -d gba ]; then \
		make -C gba ${ARGS}; \
	fi

sdl_build:
	@if [ -d sdl ]; then \
		make -C sdl ${ARGS}; \
	fi

sdl_debug_build:
	@if [ -d sdl_debug ]; then \
		make -C sdl_debug ${ARGS}; \
	fi
