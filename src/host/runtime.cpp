#include <core/runtime.hh>
#include <iostream>
#include <fstream>

namespace wasmtm {

std::unique_ptr<Runtime> Runtime::open() {
    wasmtime::Config config;
    return Runtime::open(std::forward<wasmtime::Config>(config));
}

std::unique_ptr<Runtime> Runtime::open(wasmtime::Config&& config) {
    config.interruptable(true);
    std::unique_ptr<Runtime> runtime(
        new Runtime(std::forward<wasmtime::Config>(config)));
    return runtime;
}

Runtime::Runtime(wasmtime::Config&& config)
    : engine(std::forward<wasmtime::Config>(config)), store(engine), linker(engine)
{
}

void Runtime::import_func(std::string_view module, std::string_view name, std::function<void(std::span<wasm_wrap_t>)> f) {
    /**
     * @brief 实际导入的Host Function只接收两个参数，表示偏移和长度
     * 
     * @param off
     * @param n
     */
    this->linker.func_wrap(module, name, [this, f](wasm_size_t off, wasm_size_t n) {
        auto params = this->memory_data<wasm_wrap_t>(off, n);
        f(params);
    }).unwrap();
}

void Runtime::interrupt() {
    this->interrupt_handler->interrupt();
}

size_t Runtime::memory_pages() {
    return this->memory->size(this->store);
}

size_t Runtime::memory_size() {
    return this->memory_pages() * MEMORY_PAGE_SIZE;
}

wasm_size_t Runtime::malloc(wasm_size_t sz) {
    auto r = this->wasi_malloc->call(this->store, {u2i(sz)});
    if (r) {
        auto v = r.ok();
        if (!v.empty()) return v.front().i32();
    } else {
        std::cerr << "wasi_malloc:\n" << r.err().message() << std::endl;
    }
    return 0;
}

void Runtime::call_ctors() {
    auto r = this->wasm_ctors->call(this->store, {});
    if (!r) {
        std::cerr << "wasm_ctors:\n" << r.err().message() << std::endl;
    }
}

void Runtime::call_dtors() {
    auto r = this->wasm_dtors->call(this->store, {});
    if (!r) {
        std::cerr << "wasm_dtors:\n" << r.err().message() << std::endl;
    }
}

void Runtime::free(wasm_size_t ptr) {
    auto r = this->wasi_free->call(this->store, {u2i(ptr)});
    if (!r) {
        std::cerr << "wasi_free:\n" << r.err().message() << std::endl;
    }
}

bool Runtime::filter(nid_t nid) {
    auto r = this->wasm_filter->call(this->store, {u2i(nid)});
    if (r) {
        auto v = r.ok();
        if (!v.empty()) return v.front().i32() != 0;   
    } else {
        std::cerr << "export_filter:\n" << r.err().message() << std::endl;
    }
    return false;
}

void Runtime::finish() {
    auto r = this->wasm_finish->call(this->store, {});
    if (!r) {
        std::cerr << "export_flush:\n" << r.err().message() << std::endl;
    }
}

wasmtime::Result<std::monostate> Runtime::set_wasi(wasmtime::WasiConfig&& wasi_config) {
    auto r = this->store.context().set_wasi(std::forward<wasmtime::WasiConfig>(wasi_config));
    if (r) {
        return std::monostate();
    } else {
        return wasmtime::Error(r.err());
    }
}

wasmtime::Result<std::monostate> Runtime::instantiate(std::string_view filename, size_t pages) {
    std::ifstream wasm_file(filename.data(), std::ios::binary|std::ios::in);

    if (!wasm_file) return wasmtime::Error("can\'t open file");

    wasm_file.seekg(0, std::ios::end);
    size_t wasm_size = wasm_file.tellg();
    if (wasm_size == 0) return wasmtime::Error("file is empty");

    wasm_file.seekg(0, std::ios::beg);
    std::vector<uint8_t> wasm_bytes(wasm_size);
    wasm_file.read(reinterpret_cast<char*>(wasm_bytes.data()), wasm_size);

    return this->instantiate({wasm_bytes.data(), wasm_bytes.size()}, pages);
}

wasmtime::Result<std::monostate> Runtime::instantiate(std::span<uint8_t> wasm_bytes, size_t pages) {
    // import memory
    {
        wasmtime::MemoryType type(pages);
        auto r0 = wasmtime::Memory::create(this->store, type);
        if (!r0) {
            return r0.err();
        }

        this->memory = std::make_unique<wasmtime::Memory>(r0.ok());
        
        auto r1 = this->linker.define("env", "memory", *this->memory);
        if (!r1) {
            return r1.err();
        }
    }

    // module
    {
        auto r = wasmtime::Module::compile(this->engine, {wasm_bytes.data(), wasm_bytes.size()});
        if (!r) {
            return r.err();
        } else {
            this->module = std::make_unique<wasmtime::Module>(r.ok());
        }
    }
    
    // wasi
    {
        auto r = this->linker.define_wasi();
        if (!r) {
            return r.err();
        }
    }
    
    // instance
    {
        auto r = this->linker.instantiate(this->store, *this->module);
        if (!r) {
            return wasmtime::Error(r.err().message());
        } else {
            this->instance = std::make_unique<wasmtime::Instance>(r.ok());
        }
    }

    // interrupt handler
    {
        this->interrupt_handler = this->store.context().interrupt_handle();
    }

    // linker main
    {
        auto r = this->linker.define_instance(this->store, "main", *this->instance);
        if (!r) {
            return r.err();
        }
    }

    // export wasi_malloc
    {
        auto r = this->linker.get(this->store, "main", "malloc");
        if (!r) {
            return wasmtime::Error("malloc export is null");
        } else {
            this->wasi_malloc = std::make_unique<wasmtime::Func>(std::get<wasmtime::Func>(*r));
        }
    }

    // export wasi_free
    {
        auto r = this->linker.get(this->store, "main", "free");
        if (!r) {
            return wasmtime::Error("free export is null");
        } else {
            this->wasi_free = std::make_unique<wasmtime::Func>(std::get<wasmtime::Func>(*r));
        }
    }

    // export wasm_ctors
    {
        auto r = this->linker.get(this->store, "main", "__wasm_call_ctors");
        if (!r) {
            return wasmtime::Error("ctors export is null");
        } else {
            this->wasm_ctors = std::make_unique<wasmtime::Func>(std::get<wasmtime::Func>(*r));
        }
    }

    // export wasm_dtors
    {
        auto r = this->linker.get(this->store, "main", "__wasm_call_dtors");
        if (!r) {
            return wasmtime::Error("dtors export is null");
        } else {
            this->wasm_dtors = std::make_unique<wasmtime::Func>(std::get<wasmtime::Func>(*r));
        }
    }

    // export filter
    {
        auto r = this->linker.get(this->store, "main", "filter");
        if (!r) {
            return wasmtime::Error("filter export is null");
        } else {
            this->wasm_filter = std::make_unique<wasmtime::Func>(std::get<wasmtime::Func>(*r));
        }
    }
    
    // export flush
    {
        auto r = this->linker.get(this->store, "main", "finish");
        if (!r) {
            return wasmtime::Error("flush export is null");
        } else {
            this->wasm_finish = std::make_unique<wasmtime::Func>(std::get<wasmtime::Func>(*r));
        }
    }

    // initialize global variables
    this->call_ctors();
    
    return std::monostate();
}

}