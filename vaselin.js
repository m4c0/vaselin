function vaselin_toarr(ptr, size) {
  return new Uint8Array(leco_exports.memory.buffer, ptr, size);
}
function vaselin_tostr(ptr, size) {
  return new TextDecoder().decode(vaselin_toarr(ptr, size));
}

(function() {
  function ifn(fn) { return leco_exports.__indirect_function_table.get(fn); }

  var open_files = [];

  function open_file(ptr, sz) {
    const res = open_files.push(null);
    fetch(vaselin_tostr(ptr, sz))
      .then(r => {
        const rdr = r.body.getReader();
        const buffer = new Uint8Array(r.headers.get('Content-Length'));
        var acc = 0;
        rdr.read().then(function it({ done, value }) {
          if (done) {
            open_files[res] = buffer;
            return;
          }
          buffer.set(value, acc);
          acc += value.length;
          return rdr.read().then(it);
        });
      });
    return res - 1;
  }
  function read_block(fd, ptr, sz) {
    return 1;
  }

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
    open_file,
    preopen_name_len : (idx) => idx >= vaselin_preopens.length ? 0 : vaselin_preopens[idx].length,
    preopen_name_copy : (idx, ptr, sz) => vaselin_toarr(ptr, sz).set(vaselin_preopens[idx]),
    read_block,
    request_animation_frame : (fn) => window.requestAnimationFrame(ifn(fn)),
    set_timeout : (fn, timeout) => setTimeout(ifn(fn), timeout),
  };
})();
