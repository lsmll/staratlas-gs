/**
 * @file buffer.hh
 * @author wenjie (wjie@zju.edu.cn)
 * @brief 宿主机缓存类
 * @version 0.1
 * @date 2022-03-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <cstring>
#include <memory>
#include <core/types.hh>

namespace wasmtm {

/**
 * @brief 用于LRUCache的节点缓存
 * 
 * @note 线程间安全拷贝
 */
class NodeBuffer {
    std::shared_ptr<u8> _data;
    size_t _size;
public:
    NodeBuffer(): _size(0) {}
    NodeBuffer(std::span<u8> buf) {
        this->_size = buf.size();
        this->_data.reset(new u8[this->_size]);
        memcpy(this->_data.get(), buf.data(), this->_size);
    }
    
    inline const u8* data() const {
        return this->_data.get();
    }

    inline size_t size() const {
        return this->_size;
    }

    inline bool empty() const {
        return this->size() == 0;
    }

    inline std::span<const u8> span() const {
        return {this->data(), this->size()};
    }
};

}