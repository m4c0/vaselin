function vaselin_toarr(ptr, size) {
  return new Uint8Array(leco_exports.memory.buffer, ptr, size);
}
function vaselin_tostr(ptr, size) {
  return new TextDecoder().decode(vaselin_toarr(ptr, size));
}
var vaselin_preopens = [];

(function() {
  function ifn(fn) { return leco_exports.__indirect_function_table.get(fn); }

  leco_imports.wasi_snapshot_preview1 = new Proxy({
    proc_exit : code => { throw `process exit with code ${code}` },
  }, {
    get(obj, prop) {
      return prop in obj ? obj[prop] : (... args) => {
        console.log(prop, ... args);
        throw prop + " is not defined";
      };
    },
  });
  leco_imports.vaselin = {
    console_error : (ptr, size) => console.error(vaselin_tostr(ptr, size)),
    console_log : (ptr, size) => console.log(vaselin_tostr(ptr, size)),
    date_now : () => BigInt(Date.now()),
    preopen_name_len : (idx) => idx >= vaselin_preopens.length ? 0 : vaselin_preopens[idx].length,
    preopen_name_copy : (idx, ptr, sz) => vaselin_toarr(ptr, sz).set(vaselin_preopens[idx]),
    request_animation_frame : (fn) => window.requestAnimationFrame(ifn(fn)),
    set_timeout : (fn, timeout) => setTimeout(ifn(fn), timeout),
  };
})();
