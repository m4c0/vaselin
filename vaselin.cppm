#ifndef LECO_TARGET_WASM
#error This only makes sense on WASM
#endif

module;
#include <wasi/api.h>

export module vaselin;

import hai;

#define IMPORT(R, M, N)                                                        \
  extern "C" [[clang::import_module(#M), clang::import_name(#N)]] R N
#define VASI(N) extern "C" int __imported_wasi_snapshot_preview1_##N

IMPORT(void, leco, console_error)(const char *, int);
IMPORT(void, leco, console_log)(const char *, int);
IMPORT(void, leco, set_timeout)(void (*)(), int);

VASI(fd_close)(int fd) { return __WASI_ERRNO_SUCCESS; }

VASI(fd_seek)(int fd) { return __WASI_ERRNO_ACCES; }

VASI(fd_fdstat_get)(int fd, __wasi_fdstat_t *stat) {
  if (fd != 1 && fd != 2)
    return __WASI_ERRNO_BADF;

  stat->fs_filetype = __WASI_FILETYPE_CHARACTER_DEVICE;
  stat->fs_flags = __WASI_FDFLAGS_APPEND;
  stat->fs_rights_base = __WASI_RIGHTS_FD_WRITE;
  stat->fs_rights_inheriting = __WASI_RIGHTS_FD_WRITE;
  return __WASI_ERRNO_SUCCESS;
}

VASI(fd_write)
(int fd, const __wasi_ciovec_t *iovs, size_t iovs_len, __wasi_size_t *written) {
  if (fd != 1 && fd != 2)
    return __WASI_ERRNO_BADF;

  auto acc = 0UL;
  for (auto i = 0; i < iovs_len; i++) {
    acc += iovs[i].buf_len;
  }

  hai::array<char> buf{acc};
  char *ptr = buf.begin();
  for (auto i = 0; i < iovs_len; i++) {
    auto *ip = iovs[i].buf;
    for (auto j = 0; j < iovs[i].buf_len; j++) {
      *ptr++ = *ip++;
    }
  }

  switch (fd) {
  case 1:
    console_log(buf.begin(), buf.size());
    break;
  case 2:
    console_error(buf.begin(), buf.size());
    break;
  }
  *written = acc;

  return __WASI_ERRNO_SUCCESS;
}

int main();
static void run_main() { main(); }
struct vaselin {
  vaselin() {
    set_timeout(run_main, 0);
  }
} i;
