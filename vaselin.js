function vaselin_toarr(ptr, size) {
  return new Uint8Array(leco_exports.memory.buffer, ptr, size);
}
function vaselin_tostr(ptr, size) {
  return new TextDecoder().decode(vaselin_toarr(ptr, size));
}

(function() {
  function ifn(fn, ptr) {
    return () => leco_exports.__indirect_function_table.get(fn)(ptr);
  }

  var open_files = [];

  function open_file(ptr, sz) {
    const res = open_files.push(null) - 1;
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
    return res;
  }
  function read_block(fd, ofs, ptr, sz) {
    const buf = open_files[fd];
    if (!buf) return 0;

    if (ofs + sz > buf.length) sz = buf.length - ofs;
    if (sz == 0) return -1;

    vaselin_toarr(ptr, sz).set(buf.subarray(ofs, sz));
    return sz;
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
    close_file : (fd) => open_files[fd] = null,
    console_error : (ptr, size) => console.error(vaselin_tostr(ptr, size)),
    console_log : (ptr, size) => console.log(vaselin_tostr(ptr, size)),
    date_now : () => BigInt(Date.now()),
    open_file,
    preopen_name_len : (idx) => idx >= vaselin_preopens.length ? 0 : vaselin_preopens[idx].length,
    preopen_name_copy : (idx, ptr, sz) => vaselin_toarr(ptr, sz).set(vaselin_preopens[idx]),
    read_block,
    request_animation_frame : (fn, ptr) => window.requestAnimationFrame(ifn(fn, ptr)),
    set_timeout : (fn, ptr, timeout) => setTimeout(ifn(fn, ptr), timeout),
  };
})();
