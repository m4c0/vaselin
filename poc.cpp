#ifdef LECO_TARGET_WASM
#pragma leco app
#else
#error This only makes sense on WASM
#endif

#include <stdio.h>
#include <wasi/api.h>

import hai;

extern "C"
    [[clang::import_module("leco"), clang::import_name("console_log")]] void
    console_log(const char *, int);
extern "C"
    [[clang::import_module("leco"), clang::import_name("console_error")]] void
    console_error(const char *, int);

extern "C" int
__imported_wasi_snapshot_preview1_fd_fdstat_get(int fd, __wasi_fdstat_t *stat) {
  if (fd != 1 && fd != 2)
    return __WASI_ERRNO_BADF;

  stat->fs_filetype = __WASI_FILETYPE_CHARACTER_DEVICE;
  stat->fs_flags = __WASI_FDFLAGS_APPEND;
  stat->fs_rights_base = __WASI_RIGHTS_FD_WRITE;
  stat->fs_rights_inheriting = __WASI_RIGHTS_FD_WRITE;
  return __WASI_ERRNO_SUCCESS;
}

extern "C" int
__imported_wasi_snapshot_preview1_fd_write(int fd, const __wasi_ciovec_t *iovs,
                                           size_t iovs_len,
                                           __wasi_size_t *written) {
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

struct init {
  init();
} i;
init::init() { 
  fprintf(stdout, "stdout\n");
  fprintf(stderr, "stderr\n");
}
