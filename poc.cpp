#ifdef LECO_TARGET_WASM
#pragma leco app
#else
#error This only makes sense on WASM
#endif

#include <stdio.h>

import vaselin;

struct init {
  init();
} i;
init::init() { 
  fprintf(stdout, "stdout\n");
  fprintf(stderr, "stderr\n");
}
