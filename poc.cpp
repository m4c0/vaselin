#ifdef LECO_TARGET_WASM
#pragma leco app
#else
#error This only makes sense on WASM
#endif

import silog;

struct init { init(); } i;
init::init() {
  silog::log(silog::debug, "ok");
}


