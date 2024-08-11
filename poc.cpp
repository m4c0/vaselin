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
    char buf[1024]{};
    char *ptr = buf;
    while (!feof(f)) {
      // No overflow check, I just want to read a 128-bytes file in 10-bytes chunks
      int n = fread(ptr, 1, 1, f);
      printf("Got %d bytes", n);
      ptr += n;
    }
    fclose(f);
    printf("%s\n", buf);
  }
}
