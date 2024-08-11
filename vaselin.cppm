#ifndef LECO_TARGET_WASM
#error This only makes sense on WASM
#endif

module;
#include <wasi/api.h>

export module vaselin;

import hai;
import jute;

#define IMPORT(R, N) extern "C" [[clang::import_module("vaselin"), clang::import_name(#N)]] R N
#define VASI(N) extern "C" int __imported_wasi_snapshot_preview1_##N

export namespace vaselin {
  IMPORT(void, console_error)(const char *, int);
  IMPORT(void, console_log)(const char *, int);
  IMPORT(int, preopen_name_len)(int);
  IMPORT(int, preopen_name_copy)(int, uint8_t *, int);
  IMPORT(__wasi_timestamp_t, date_now)();
  IMPORT(void, request_animation_frame)(void (*)());
  IMPORT(void, set_timeout)(void (*)(), int);
} // namespace vaselin

static constexpr const auto read_rights = __WASI_RIGHTS_FD_READ;

static void err(jute::view msg) { vaselin::console_error(msg.begin(), msg.size()); }

VASI(clock_time_get)(__wasi_clockid_t id, __wasi_timestamp_t precision, __wasi_timestamp_t * ret) {
  if (id != 0) return __WASI_ERRNO_ACCES;
  *ret = vaselin::date_now() * 1000000;
  return __WASI_ERRNO_SUCCESS;
}

VASI(fd_seek)(int fd, __wasi_filedelta_t offset, __wasi_whence_t whence, __wasi_filesize_t * ret) {
  return __WASI_ERRNO_ACCES;
}

VASI(fd_fdstat_get)(int fd, __wasi_fdstat_t * stat) {
  switch (fd) {
    case 1: // stdout
    case 2: // stderr
      stat->fs_filetype = __WASI_FILETYPE_CHARACTER_DEVICE;
      stat->fs_flags = __WASI_FDFLAGS_APPEND;
      stat->fs_rights_base = __WASI_RIGHTS_FD_WRITE;
      stat->fs_rights_inheriting = __WASI_RIGHTS_FD_WRITE;
      return __WASI_ERRNO_SUCCESS;
    case 3: // "root dir" (prestat'd)
      stat->fs_filetype = __WASI_FILETYPE_DIRECTORY;
      stat->fs_flags = 0;
      stat->fs_rights_base = __WASI_RIGHTS_FD_READ;
      stat->fs_rights_inheriting = __WASI_RIGHTS_FD_READ;
      return __WASI_ERRNO_SUCCESS;
    default: err("fdstat_get: unknown FD"); return __WASI_ERRNO_BADF;
  }
}

VASI(fd_prestat_get)(int fd, __wasi_prestat_t * ret) {
  switch (fd) {
    case 3:
      ret->tag = 0;
      ret->u.dir.pr_name_len = 1;
      return __WASI_ERRNO_SUCCESS;
    case 4: // expected
      return __WASI_ERRNO_BADF;
    default: err("fd_prestat_get: unknown FD"); return __WASI_ERRNO_BADF;
  }
}
VASI(fd_prestat_dir_name)(int fd, uint8_t * path, unsigned path_len) {
  switch (fd) {
    case 3: path[0] = '/'; return __WASI_ERRNO_SUCCESS;
    default: err("fd_prestat_dir_name: unknown FD"); return __WASI_ERRNO_BADF;
  }
}

VASI(fd_write)(int fd, const __wasi_ciovec_t * iovs, size_t iovs_len, __wasi_size_t * written) {
  if (fd != 1 && fd != 2) return __WASI_ERRNO_BADF;

  auto acc = 0UL;
  for (auto i = 0; i < iovs_len; i++) {
    acc += iovs[i].buf_len;
  }

  hai::array<char> buf { acc };
  char * ptr = buf.begin();
  for (auto i = 0; i < iovs_len; i++) {
    auto * ip = iovs[i].buf;
    for (auto j = 0; j < iovs[i].buf_len; j++) {
      *ptr++ = *ip++;
    }
  }

  switch (fd) {
    case 1: vaselin::console_log(buf.begin(), buf.size()); break;
    case 2: vaselin::console_error(buf.begin(), buf.size()); break;
  }
  *written = acc;

  return __WASI_ERRNO_SUCCESS;
}

VASI(path_open)
(int dir_fd, int, const char * path, unsigned len, int, int64_t base, int64_t inh, int fdflags, int * ret) {
  if (base != read_rights || inh != read_rights) return __WASI_ERRNO_ACCES;
  if (fdflags != 0) return __WASI_ERRNO_ACCES;

  auto p = jute::view { path, len };
  err(p);
  return __WASI_ERRNO_ACCES;
}

int main();
static void run_main() { main(); }
namespace vaselin {
  struct init {
    init() { set_timeout(run_main, 0); }
  } i;
} // namespace vaselin
