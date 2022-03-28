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
#include <relations_generated.h>

namespace wasmtm {

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

}