#include <core/scheduler.hh>

#include <chrono>
#include <algorithm>

namespace wasmtm {

SchedulerBlock::SchedulerBlock(SchedulerQueue::Type type, std::shared_ptr<Storage> storage, bool distinct) {
    this->_Q = std::make_unique<SchedulerQueue>(type);
    this->_selected = std::make_unique<std::unordered_set<nid_t>>();
    this->_storage = storage;
    if (distinct) {
        this->_distinct = std::make_unique<std::unordered_set<nid_t>>();
    }
}

void SchedulerBlock::push(nid_t nid, f64 p) {
    if (this->_Q) {
        if (this->_distinct) {
            auto it = this->_distinct->find(nid);
            if (it == this->_distinct->end()) {
                this->_Q->push(nid, p);
                this->_distinct->insert(nid);
            }
        } else {
            this->_Q->push(nid, p);
        }
    }
}

void SchedulerBlock::clear() {
    if (this->_Q) this->_Q->clear();
    if (this->_distinct) this->_distinct->clear();
    if (this->_selected) this->_selected->clear();
}

std::vector<nid_t> SchedulerBlock::selected() {
    std::vector<nid_t> nids;
    if (this->_selected) {
        nids.reserve(this->_selected->size());
        std::copy(this->_selected->begin(), this->_selected->end(), std::back_inserter(nids));
    }
    return nids;
}

bool SchedulerBlock::poll() {
    if (!this->_wasm) {
        return false;
    }

    if (this->_Q && !this->_Q->empty()) {
        nid_t nid = this->_Q->pop();
        bool ok = this->_wasm->filter(nid);
        if (ok && this->_selected) {
            this->_selected->insert(nid);
        }
        return !this->_Q->empty();
    }
    return false;
}

void SchedulerBlock::finish() {
    if (this->_wasm) this->_wasm->finish();
}

bool SchedulerBlock::initialize(std::span<u8> wasm_bytes, size_t pages) {
    
    // 限制pages范围
    pages = std::min<size_t>(std::max<size_t>(pages, 8), 65536);

    this->_wasm = Runtime::open();

    auto& wasm = this->_wasm;
    // auto& distinct = this->_distinct;
    // wasm->import_func("env", "distinct", [&](wasm_params_t params) {
    //     auto [flag] = wasm_funcall_pull<bool>(params);
    //     if (flag && !distinct) {
    //         distinct = std::make_unique<std::unordered_set<nid_t>>();
    //     } else if (!flag && distinct) {
    //         distinct.release();
    //     }
    // });

    wasm->import_func("env", "console_log", [&](wasm_params_t params) {
        auto [off, n] = wasm_funcall_pull<wasm_size_t, wasm_size_t>(params);
        auto buf = wasm->memory_data<char>(off, n);

        std::string_view s(buf.data(), buf.size());
        std::cerr << s << std::endl;
    });

    wasm->import_func("env", "get_node", [&](wasm_params_t params) {
        auto [nid] = wasm_funcall_pull<nid_t>(params);

        auto buffer = this->_storage->get(nid);
        if (buffer.empty()) {
            wasm_funcall_push<wasm_size_t, wasm_size_t>(params, 0, 0);
        } else {
            wasm_size_t offset = wasm->malloc(buffer.size());
            if (offset == 0) {
                wasm_funcall_push<wasm_size_t, wasm_size_t>(params, 0, 0);
            } else {
                auto wasm_buffer = wasm->memory_data(offset, buffer.size());
                memcpy(wasm_buffer.data(), buffer.data(), buffer.size());
                wasm_funcall_push<wasm_size_t, wasm_size_t>(params, offset, buffer.size());
            }
        }
    });

    auto& selected = this->_selected;
    wasm->import_func("env", "select_node", [&](wasm_params_t params) {
        auto [off, n] = wasm_funcall_pull<wasm_size_t, wasm_size_t>(params);
        auto nids = wasm->memory_data<nid_t>(off, n);

        for (auto it = nids.begin(); it != nids.end(); ++it) {
            selected->insert(*it);
        }
    });

    wasm->import_func("env", "extend_node", [&](wasm_params_t params) {
        if (params.size() == 2) {
            // 默认优先级
            auto [off, n] = wasm_funcall_pull<wasm_size_t, wasm_size_t>(params);
            auto nids = wasm->memory_data<nid_t>(off, n);

            for (auto it = nids.begin(); it != nids.end(); ++it) {
                this->push(*it);
            }
        } else if (params.size() == 4) {
            auto [noff, nn, poff, pn] = wasm_funcall_pull<wasm_size_t, wasm_size_t, wasm_size_t, wasm_size_t>(params);
            auto n = std::min(nn, pn);
            auto nids = wasm->memory_data<nid_t>(noff, n);
            auto ps = wasm->memory_data<f64>(poff, n);

            for (size_t i = 0; i < n; i++) {
                this->push(nids[i], ps[i]);
            }
        }
    });

    auto start = std::chrono::high_resolution_clock::now();
    wasm->import_func("env", "clk_m", [&start](wasm_params_t params) {
        auto end = std::chrono::high_resolution_clock::now();
        u64 r = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        wasm_funcall_push(params, r);
    });

    auto instance = wasm->instantiate(wasm_bytes, pages);
    if (instance) {
        return true;
    } else {
        this->_wasm.release();
        std::cerr << instance.err().message() << std::endl;
        return false;
    }
}

}