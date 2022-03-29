/**
 * @file buffer.h
 * @author wenjie (wjie@zju.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <core/types.hh>

/**
 * @todo 通过宏开关WASM_ON来控制编译器自动补全和代码生成
 * 
 */
#define WASM_DECLARE(name, vars) \
    struct vars name;

#define WASM_FILTER \
    bool wasmtm::filter_impl(nid_t nid)

#define WASM_FINISH \
    void wasmtm::finish_impl()

#define WASM_BEGIN
#define WASM_END

namespace wasmtm {

/**
 * @brief 链接到UDF函数
 * 
 * @param nid 
 * @return true 
 * @return false 
 */
bool filter_impl(nid_t nid);

/**
 * @brief 连接到单次采样后续处理
 * 
 */
void finish_impl();

}