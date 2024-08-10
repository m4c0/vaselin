#ifndef LECO_TARGET_WASM
#error This only makes sense on WASM
#endif

module;
#include <wasi/api.h>

export module vaselin;

import hai;

#define IMPORT(R, N)                                                        \
  extern "C" [[clang::import_module("vaselin"), clang::import_name(#N)]] R N
#define VASI(N) extern "C" int __imported_wasi_snapshot_preview1_##N

export namespace vaselin {
IMPORT(void, console_error)(const char *, int);
IMPORT(void, console_log)(const char *, int);
IMPORT(__wasi_timestamp_t, date_now)();
IMPORT(void, request_animation_frame)(void (*)());
IMPORT(void, set_timeout)(void (*)(), int);
} // namespace vaselin

VASI(clock_time_get)(
    __wasi_clockid_t id,
    __wasi_timestamp_t precision,
    __wasi_timestamp_t *ret
    ) {
  if (id != 0) return __WASI_ERRNO_ACCES;
  *ret = vaselin::date_now() * 1000000;
  return __WASI_ERRNO_SUCCESS;
}

VASI(fd_seek)
(int fd, __wasi_filedelta_t offset, __wasi_whence_t whence,
 __wasi_filesize_t *ret) {
  return __WASI_ERRNO_ACCES;
}

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
    vaselin::console_log(buf.begin(), buf.size());
    break;
  case 2:
    vaselin::console_error(buf.begin(), buf.size());
    break;
  }
  *written = acc;

  return __WASI_ERRNO_SUCCESS;
}

int main();
static void run_main() { main(); }
namespace vaselin {
struct init {
  init() { set_timeout(run_main, 0); }
} i;
} // namespace vaselin
