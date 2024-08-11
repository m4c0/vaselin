#ifdef LECO_TARGET_WASM
#pragma leco app
#else
#error This only makes sense on WASM
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>

import vaselin;

static FILE * f;

// Due to the event-driven nature of JS, we can't do a while loop for reading the file.
// So we need to treat all IO as "async"
void try_read() {
  // TODO: the final </html> is not being printed
  if (feof(f)) {
    fclose(f);
    return;
  }

  char buf[10]{};
  
  // No overflow check, I just want to read a 128-bytes file in 10-bytes chunks
  int n = fread(buf, 1, sizeof(buf), f);
  if (n > 0) fwrite(buf, n, 1, stdout);

  vaselin::set_timeout(try_read, 0);
}

int main() {
  fprintf(stdout, "stdout\n");
  fprintf(stderr, "stderr\n");

  f = fopen("poc.html", "rb");
  if (!f) {
    fprintf(stderr, "%s\n", strerror(errno));
  } else {
    vaselin::set_timeout(try_read, 0);
  }
}
