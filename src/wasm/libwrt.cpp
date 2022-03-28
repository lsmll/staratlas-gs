#include <wasm/libwrt.hh>

#include <wasm/funcall.hh>

namespace wasmtm {

/**
 * @brief Host Functions
 * 
 */
// WASM_IMPORT(env, distinct);
WASM_IMPORT(env, console_log);
WASM_IMPORT(env, get_node);
WASM_IMPORT(env, select_node);
WASM_IMPORT(env, extend_node);
WASM_IMPORT(env, clk_m);

// DISTINCT::DISTINCT(bool flag) {
//     static bool DISTINCT_INVOKED = false;
//     if (!DISTINCT_INVOKED) {
//         std::array<wasm_wrap_t, 1> params;
//         wasm_funcall_push(params, flag);
//         WASM_IMPORT_CALL(distinct, params);
//         DISTINCT_INVOKED = true;
//     }
// }

void console_log(const std::string_view s) {
    std::array<wasm_wrap_t, 2> params;
    wasm_funcall_push(params,
        s.data(),
        s.length()
    );
    WASM_IMPORT_CALL(console_log, params);
}

DataBuffer get_node(nid_t nid) {
    std::array<wasm_wrap_t, 2> params;
    wasm_funcall_push<nid_t>(params, nid);

    WASM_IMPORT_CALL(get_node, params);

    auto [s, n] = wasm_funcall_pull<u8*, size_t>(params);
    return {s, n};
}

void select_node(nid_t nid) {
    std::array<wasm_wrap_t, 2> params;
    wasm_funcall_push<nid_t*, wasm_size_t>(params, &nid, 1);
    WASM_IMPORT_CALL(select_node, params);
}

void select_node(std::span<nid_t> nids) {
    std::array<wasm_wrap_t, 2> params;
    wasm_funcall_push(params, nids.data(), nids.size());
    WASM_IMPORT_CALL(select_node, params);
}

void extend_node(nid_t nid) {
    std::array<wasm_wrap_t, 2> params;
    wasm_funcall_push<nid_t*, wasm_size_t>(params, &nid, 1);
    WASM_IMPORT_CALL(extend_node, params);
}

void extend_node(nid_t nid, f64 p) {
    std::array<wasm_wrap_t, 4> params;
    wasm_funcall_push<nid_t*, wasm_size_t, f64*, wasm_size_t>(params, &nid, 1, &p, 1);
    WASM_IMPORT_CALL(extend_node, params);
}

void extend_node(std::span<nid_t> nids) {
    std::array<wasm_wrap_t, 2> params;
    wasm_funcall_push(params, nids.data(), nids.size());
    WASM_IMPORT_CALL(extend_node, params);
}

void extend_node(std::span<nid_t> nids, std::span<f64> ps) {
    std::array<wasm_wrap_t, 4> params;
    wasm_funcall_push(params, nids.data(), nids.size(), ps.data(), ps.size());
    WASM_IMPORT_CALL(extend_node, params);
}

u64 clk_m() {
    std::array<wasm_wrap_t, 1> params;
    WASM_IMPORT_CALL(clk_m, params);
    auto [r] = wasm_funcall_pull<u64>(params);
    return r;
}

}

extern "C" {
    bool filter(nid_t nid) {
        return wasmtm::filter_impl(nid);
    }

    void finish() {
        wasmtm::finish_impl();
    }
}

int main() {
    wasmtm::console_log("calling main();");
    return 0;
}
