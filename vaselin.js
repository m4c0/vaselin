(function() {
  function ifn(fn) { return leco_exports.__indirect_function_table.get(fn); }
  function arr(ptr, size) { return new Uint8Array(leco_exports.memory.buffer, ptr, size); }

  leco_imports.vaselin = {
    console_error : (ptr, size) => console.error(new TextDecoder().decode(arr(ptr, size))),
    console_log : (ptr, size) => console.log(new TextDecoder().decode(arr(ptr, size))),
    request_animation_frame : (fn) => window.requestAnimationFrame(ifn(fn)),
    set_timeout : (fn, timeout) => setTimeout(ifn(fn), timeout),
  };
})();
