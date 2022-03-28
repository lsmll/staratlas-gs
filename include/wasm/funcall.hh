/**
 * @file funcall.hh
 * @author wenjie (wjie@zju.edu.cn)
 * @brief WASM定义+调用宿主机函数（Host Function）的辅助库
 * @version 0.1
 * @date 2022-02-21
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include <core/types.hh>
#include <cassert>
#include <array>
#include <type_traits>

namespace wasmtm {

/**
 * @brief 定义WASM容器导入函数的实际函数名
 * 
 */
#define WASM_FUNCALL_IMPORT_NAME(name) \
    __wasm_funcall_##name##_port

/**
 * @brief 定义WASM容器导入函数的环境和名称
 * 
 */
#define WASM_FUNCALL_IMPORT_ATTRIBUTE(module, name) \
    __attribute__((import_module(#module))) \
	__attribute__((import_name(#name)))

/**
 * @brief 定义WASM容器的导入函数，和Host Function匹配
 * 
 */
#define WASM_IMPORT(module, name) void \
	WASM_FUNCALL_IMPORT_ATTRIBUTE(module, name) \
	WASM_FUNCALL_IMPORT_NAME(name)(wasm_size_t,wasm_size_t)

/**
 * @brief WASM容器内部调用调入函数的辅助宏
 * 
 */
#define WASM_IMPORT_CALL(name, params) \
    WASM_FUNCALL_IMPORT_NAME(name)( \
        reinterpret_cast<wasm_size_t>(&params[0]), \
        params.size())

/**
 * @brief 封包WASM导入函数参数
 * 
 * @tparam T 
 * @param x 
 * @return wasm_wrap_t 
 */
template<typename T>
inline wasm_wrap_t wasm_funcall_wrap(T x) {
    static_assert(std::is_pod<T>::value);
    static_assert(sizeof(T) <= sizeof(wasm_wrap_t));
    wasm_wrap_t r = 0;
    *reinterpret_cast<T*>(&r) = x;
    return r;
}

/**
 * @brief 解包WASM导入函数参数
 * 
 * @tparam T 
 * @param x 
 * @return T 
 */
template<typename T>
inline T wasm_funcall_unwrap(wasm_wrap_t x) {
    static_assert(std::is_pod<T>::value);
    static_assert(sizeof(T) <= sizeof(wasm_wrap_t));
    return *reinterpret_cast<const T*>(&x);
}


template<size_t s, typename ... Args> struct __wasm_funcall_utils {
    static void pull_tuple(std::span<wasm_wrap_t> buf, std::tuple<Args...>& t) {
        static_assert(s > 1);
        const size_t i = sizeof...(Args) - s;
        using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
        std::get<i>(t) = wasm_funcall_unwrap<type>(buf[i]);
        __wasm_funcall_utils<s-1, Args...>::pull_tuple(buf, t);
    }
};
template<typename ... Args> struct __wasm_funcall_utils<1, Args...> {
    static void pull_tuple(std::span<wasm_wrap_t> buf, std::tuple<Args...>& t) {
        const size_t i = sizeof...(Args) - 1;
        using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
        std::get<i>(t) = wasm_funcall_unwrap<type>(buf[i]);
    }
};

/**
 * @brief 用于批量封包WASM外部函数参数
 * 
 * @tparam Args 
 * @param buf 
 * @param args 
 */
template<typename ... Args>
inline void wasm_funcall_push(std::span<wasm_wrap_t> buf, Args ... args) {
    const size_t nargs = sizeof...(Args);
    assert(buf.size() >= nargs);

    std::array<wasm_wrap_t, sizeof...(Args)> tmp = {
        wasm_funcall_wrap<Args>(args)...};
    std::copy(tmp.begin(), tmp.end(), buf.begin());
}

// template<typename T>
// inline void wasm_funcall_pull(std::span<wasm_wrap_t> buf, T& t) {
//     assert(!buf.empty());
//     t = wasm_funcall_unwrap<T>(buf.front());
// }

// template<typename T, typename ... Args>
// inline void wasm_funcall_pull(std::span<wasm_wrap_t> buf, T& t, Args& ... args) {
//     assert(!buf.empty());
//     t = wasm_funcall_unwrap<T>(buf.front());
//     wasm_funcall_pull({buf.data()+1, buf.size()-1}, args...);
// }

/**
 * @brief 用于批量解包WASM外部函数参数
 * 
 * @tparam Args 
 * @param buf 
 * @return std::tuple<Args...> 
 */
template<typename ... Args>
inline std::tuple<Args...> wasm_funcall_pull(std::span<wasm_wrap_t> buf) {
    const size_t nargs = sizeof...(Args);
    assert(nargs <= buf.size());

    std::tuple<Args...> t;
    __wasm_funcall_utils<nargs, Args...>::pull_tuple(buf, t);
    return t;
}

}