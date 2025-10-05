[[clang::import_module("vaselin"), clang::import_name("console_error")]]
void vaselin_console_error(const char * msg, int sz);

[[noreturn]] void abort();
[[noreturn]] static inline void fail() {
  vaselin_console_error("exception\n", 10);
  abort();
}

// Stubbing these until exceptions are properly supported
unsigned __cxa_allocate_exception(unsigned a) { fail(); }
void __cxa_throw(unsigned a, unsigned b, unsigned c) { fail(); }
unsigned __cxa_begin_catch(unsigned a) { fail(); }
void __cxa_end_catch() { fail(); }
