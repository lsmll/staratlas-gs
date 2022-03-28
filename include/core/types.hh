/**
 * @file types.hh
 * @author wenjie (wjie@zju.edu.cn)
 * @brief 基本类型定义
 * @version 0.1
 * @date 2022-02-2
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include <cstdint>

#define TCB_SPAN_NAMESPACE_NAME std
#include <tcb/span.hpp>

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using wasm_size_t = u32;
using wasm_isize_t = i32;

using wasm_wrap_t = u64;
using wasm_params_t = std::span<wasm_wrap_t>;

using nid_t = u64;

inline i8 u2i(u8 x) { return static_cast<i8>(x); }
inline i16 u2i(u16 x) { return static_cast<i16>(x); }
inline i32 u2i(u32 x) { return static_cast<i32>(x); }
inline i64 u2i(u64 x) { return static_cast<i64>(x); }

inline u8 i2u(i8 x) { return static_cast<u8>(x); }
inline u16 i2u(i16 x) { return static_cast<u16>(x); }
inline u32 i2u(i32 x) { return static_cast<u32>(x); }
inline u64 i2u(i64 x) { return static_cast<u64>(x); }
