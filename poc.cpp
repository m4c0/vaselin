#ifdef LECO_TARGET_WASM
#pragma leco app
#else
#error This only makes sense on WASM
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>

import vaselin;

int main() {
  fprintf(stdout, "stdout\n");
  fprintf(stderr, "stderr\n");

  auto f = fopen("poc.html", "rb");
  if (!f) {
    fprintf(stderr, "%s\n", strerror(errno));
  } else {
    fgetc(f);
    fclose(f);
  }
}
