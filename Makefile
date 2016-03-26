make:
	make -j -C build
preinstall:
	make -j -C build ARGS="preinstall"
install:
	make -j -C build ARGS="install"
clean:
	make -j -C build ARGS="clean"
test:
	make -j -C build ARGS="test"
run: make
	./build/current/src/wombat/wombat -debug
debug: make
	gdb ./build/current/src/wombat/wombat

sdl:
	./scripts/setup_build
	rm -f build/current
	ln -s sdl build/current

sdl_debug:
	./scripts/setup_build_debug
	rm -f build/current
	ln -s sdl_debug build/current

gba:
	./scripts/setup_build_gba
	rm -f build/current
	ln -s sdl_gba build/current
