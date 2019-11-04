
extern void (*__preinit_array_start[]) (void);
extern void (*__preinit_array_end[]) (void);
extern void (*__init_array_start[]) (void);
extern void (*__init_array_end[]) (void);

extern "C" void __libc_init_array() {
	auto preInits = __preinit_array_end - __preinit_array_start;
	for (decltype(preInits) i = 0; i < preInits; i++) {
		__preinit_array_start[i]();
	}
	auto inits = __init_array_end - __init_array_start;
	for (decltype(inits) i = 0; i < inits; i++) {
		__preinit_array_start[i]();
	}
}
