/**
 * @file runtime.hh
 * @author wenjie (wjie@zju.edu.cn)
 * @brief WASM容器运行库封装（C++接口）
 * @version 0.1
 * @date 2022-02-25
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <iostream>
#include <wasmtime.hh>
#include <functional>
#include <wasm/funcall.hh>

namespace wasmtm {

class Runtime {
    wasmtime::Engine engine;
    wasmtime::Store store;
    wasmtime::Linker linker;
    std::unique_ptr<wasmtime::Module> module;
    std::unique_ptr<wasmtime::Instance> instance;
    std::unique_ptr<wasmtime::Memory> memory;
    
    std::unique_ptr<wasmtime::Func> wasi_malloc;
    std::unique_ptr<wasmtime::Func> wasi_free;
    std::unique_ptr<wasmtime::Func> wasm_ctors;
    std::unique_ptr<wasmtime::Func> wasm_dtors;
    std::unique_ptr<wasmtime::Func> wasm_filter;
    std::unique_ptr<wasmtime::Func> wasm_finish;

    std::optional<wasmtime::InterruptHandle> interrupt_handler;
public:
    /**
     * @brief 创建一个默认参数的WASM容器
     * 
     * @return std::unique_ptr<Runtime> 
     */
    static std::unique_ptr<Runtime> open();

    /**
     * @brief 按照配置创建WASM容器
     * 
     * @param config 
     * @return std::unique_ptr<Runtime> 
     * 
     * @note 会强制开启interruptable功能
     */
    static std::unique_ptr<Runtime> open(wasmtime::Config&& config);

    /**
     * @brief 导入Host Functions
     * 
     * @param module 模块名，例如env
     * @param name 函数名
     * @param f 函数对象
     */
    void import_func(std::string_view module, std::string_view name, std::function<void(std::span<wasm_wrap_t>)> f);

    /**
     * @brief 打断WASM容器执行，不可恢复
     * 
     * @note 线程安全函数
     */
    void interrupt();

    /**
     * @brief 获取WASM容器memory区域，以T*格式返回
     * 
     * @tparam T 返回数组类型，默认uint8_t
     * @param offset 内存段在WASM里的偏移
     * @param len 内存段长度
     * @return std::span<T> 
     */
    template<typename T = uint8_t>
    std::span<T> memory_data(size_t offset = 0, size_t len = 0) {
        auto data = this->memory->data(this->store);
        auto ptr = reinterpret_cast<T*>(&data[offset]);
        return {ptr, len};
    }

    /**
     * @brief 返回内存页数
     * 
     * @return size_t 
     * 
     * @note 页大小64KB
     */
    size_t memory_pages();

    /**
     * @brief 返回实际内存大小
     * 
     * @return size_t 
     * 
     * @note pages * 64KB，不是精确值
     */
    size_t memory_size();

    /**
     * @brief 导出WASM容器内libc库的memory函数
     * 
     * @param sz 
     * @return wasm_size_t 
     * 
     * @note 返回的是memory偏移，需要用memory_data获取真实地址
     */
    wasm_size_t malloc(wasm_size_t sz);

    /**
     * @brief 导出WASM容器内libc库的free函数
     * 
     * @param ptr 
     * 
     * @note 传入WASM容器内的偏移地址，而不是进程虚拟地址
     */
    void free(wasm_size_t ptr); 

    /**
     * @brief 判断点是否被保留
     * 
     * @param nid 
     * @return true 保留
     * @return false 不保留
     */
    bool filter(nid_t nid);

    /**
     * @brief 单次采样完成之后调用，用于重置容器状态，返回内部信息等
     * 
     */
    void finish();

    /**
     * @brief 初始化全局对象
     * 
     */
    void call_ctors();

    /**
     * @brief 析构全局对象
     * 
     */
    void call_dtors();

    [[nodiscard]] wasmtime::Result<std::monostate> set_wasi(wasmtime::WasiConfig&& wasi_config);
    
    /**
     * @brief 从文件实例化WASM容器
     * 
     * @param filename 
     * @param pages 
     * @return wasmtime::Result<std::monostate>
     * 
     * @note 内部调用重载函数 
     */
    [[nodiscard]] wasmtime::Result<std::monostate> instantiate(std::string_view filename, size_t pages);

    /**
     * @brief 实例化WASM容器
     * 
     * @param wasm_bytes WASM字节码
     * @param pages 内存页数
     * @return wasmtime::Result<std::monostate> 
     * 
     * @note 1. 创建内存块 2. 编译 3. 链接模块 4. 导出内部函数 5. 初始化全局变量
     */
    [[nodiscard]] wasmtime::Result<std::monostate> instantiate(std::span<uint8_t> wasm_bytes, size_t pages);
private:
    Runtime(wasmtime::Config&& config);
};

}