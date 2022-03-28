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
        this->_Q->push(nid, p);
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
    pages = std::min<size_t>(std::max<size_t>(pages, 8), 65536);

    auto runtime = Runtime::open();
    auto raw_rt = runtime.get();

    runtime->import_func("env", "console_log", [raw_rt](wasm_params_t params) {
        auto [off, n] = wasm_funcall_pull<wasm_size_t, wasm_size_t>(params);
        auto buf = raw_rt->memory_data<char>(off, n);

        std::string_view s(buf.data(), buf.size());
        std::cerr << s << std::endl;
    });

    runtime->import_func("env", "get_node", [raw_rt, this](wasm_params_t params) {
        auto [nid] = wasm_funcall_pull<nid_t>(params);

        auto buffer = this->_storage->get(nid);
        if (buffer.empty()) {
            wasm_funcall_push<wasm_size_t, wasm_size_t>(params, 0, 0);
        } else {
            wasm_size_t offset = raw_rt->malloc(buffer.size());
            if (offset == 0) {
                wasm_funcall_push<wasm_size_t, wasm_size_t>(params, 0, 0);
            } else {
                auto wasm_buffer = raw_rt->memory_data(offset, buffer.size());
                memcpy(wasm_buffer.data(), buffer.data(), buffer.size());
                wasm_funcall_push<wasm_size_t, wasm_size_t>(params, offset, buffer.size());
            }
        }
    });

    auto raw_selected = this->_selected.get();
    runtime->import_func("env", "select_node", [raw_rt, raw_selected](wasm_params_t params) {
        auto [off, n] = wasm_funcall_pull<wasm_size_t, wasm_size_t>(params);
        auto nids = raw_rt->memory_data<nid_t>(off, n);

        for (auto it = nids.begin(); it != nids.end(); ++it) {
            raw_selected->insert(*it);
        }
    });

    auto raw_distinct = this->_distinct.get();
    auto raw_Q = this->_Q.get();
    runtime->import_func("env", "extend_node", [raw_rt, raw_Q, raw_distinct](wasm_params_t params) {
        auto [off, n] = wasm_funcall_pull<wasm_size_t, wasm_size_t>(params);
        auto nids = raw_rt->memory_data<nid_t>(off, n);

        for (auto it = nids.begin(); it != nids.end(); ++it) {
            nid_t nid = *it;
            if (raw_distinct) {
                if (raw_distinct->find(nid) == raw_distinct->end()) {
                    raw_distinct->insert(nid);
                    raw_Q->push(nid);
                }
            } else {
                raw_Q->push(nid);
            }
        }
    });

    auto start = std::chrono::high_resolution_clock::now();
    runtime->import_func("env", "clk_m", [&start](wasm_params_t params) {
        auto end = std::chrono::high_resolution_clock::now();
        u64 r = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        wasm_funcall_push(params, r);
    });

    auto instance = runtime->instantiate(wasm_bytes, pages);
    if (instance) {
        this->_wasm = std::move(runtime);
        return true;
    } else {
        std::cerr << instance.err().message() << std::endl;
        return false;
    }
}

}