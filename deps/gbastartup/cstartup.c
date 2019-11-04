
extern void (*__preinit_array_start[]) (void);
extern void (*__preinit_array_end[]) (void);
extern void (*__init_array_start[]) (void);
extern void (*__init_array_end[]) (void);

extern "C" void __libc_init_array() {
	auto count = __preinit_array_end - __preinit_array_start;
}
