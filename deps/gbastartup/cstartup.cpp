
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

extern void (*__preinit_array_start[]) (void);
extern void (*__preinit_array_end[]) (void);
extern void (*__init_array_start[]) (void);
extern void (*__init_array_end[]) (void);

extern "C" {

void __libc_init_array() {
	auto preInits = __preinit_array_end - __preinit_array_start;
	for (decltype(preInits) i = 0; i < preInits; i++) {
		__preinit_array_start[i]();
	}
	auto inits = __init_array_end - __init_array_start;
	for (decltype(inits) i = 0; i < inits; i++) {
		__preinit_array_start[i]();
	}
}

int main(int argc, const char **argv);

int c_start() {
	const char *args[2] = {"", "rom.oxfs"};
	return main(2, args);
}

}

#pragma GCC diagnostic pop
