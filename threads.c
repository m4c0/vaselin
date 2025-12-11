// Required by thread-locals. We don't support thread, so we ignore this.
int __cxa_thread_atexit(void (*dtor)(void *), void * obj, void * dso) {
  // TODO: add a debug/warning about ignoring this
  return 0;
}

