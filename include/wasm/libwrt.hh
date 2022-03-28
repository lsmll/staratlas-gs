/**
 * @file libwrt.hh
 * @author wenjie (wjie@zju.edu.cn)
 * @brief WASM容器程序运行库
 * @version 0.1
 * @date 2022-03-18
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <string>
#include <memory>

#include <wasm/funcall.hh>
#include <relations_generated.h>

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

namespace wasmtm {

/**
 * @brief Host Functions
 * 
 */
WASM_IMPORT(env, console_log);
WASM_IMPORT(env, get_node);
WASM_IMPORT(env, select_node);
WASM_IMPORT(env, extend_node);

/**
 * @brief 链接到UDF函数
 * 
 * @param nid 
 * @return true 
 * @return false 
 */
bool filter_impl(nid_t nid);

/**
 * @brief 单次采样后续处理
 * 
 */
void finish_impl();

/**
 * @brief 容器内部的节点缓存类
 * 
 */
class DataBuffer {
    struct Deleter {
        void operator() (u8* t) {
            free(t);
        }
    };

    std::unique_ptr<u8, DataBuffer::Deleter> _data;
    const size_t _size;
    const Node* _node;
public:
    DataBuffer(u8* data, size_t size)
        : _data(data), _size(size), _node(nullptr)
    {
        if (this->_size != 0) {
            this->_node = GetNode(this->_data.get());
        }
    }

    inline const u8* data() const { return this->_data.get(); }
    inline size_t size() const { return this->_size; }
    
    const Node* operator -> () const { return this->_node; }
    
    operator bool() const {
        return this->_node != nullptr;
    }
};

/**
 * @brief 向终端输出日志，用于调试
 * 
 * @param s 
 * 
 * @todo 完整的分级日志功能
 */
void console_log(const std::string_view s);

/**
 * @brief 从宿主机下载节点
 * 
 * @param nid 
 * @return DataBuffer 
 */
DataBuffer get_node(nid_t nid);

/**
 * @brief 选中节点，作为结果返回
 * 
 * @param nid 
 */
void select_node(nid_t nid);
void select_node(std::span<nid_t> nids);

/**
 * @brief 将节点添加到候选队列里
 * 
 * @param nid 
 */
void extend_node(nid_t nid);
void extend_node(std::span<nid_t> nids);

}
/**
 * @brief 名字更友好一点
 * 
 */
namespace wasm = wasmtm;
