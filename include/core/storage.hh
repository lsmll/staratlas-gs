/**
 * @file storage.hh
 * @author wenjie (wjie@zju.edu.cn)
 * @brief 后端存储访问器
 * @version 0.1
 * @date 2022-03-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <functional>

#include <core/buffer.hh>
#include <core/types.hh>
#include <caches/cache.hpp>
#include <caches/lru_cache_policy.hpp>

namespace wasmtm {

template<typename K, typename V>
using LRUCache = typename caches::fixed_sized_cache<K, V, caches::LRUCachePolicy>;

/**
 * @brief 用于访问存储后端
 * 
 * @note 先到缓存中查询数据，如果为空则调用fetch函数从后端获取。
 * @note 缓存对象和回调对象都需要保证线程安全！
 * 
 * @todo 带失效时间的LRU缓存
 */
class Storage {
    LRUCache<nid_t, NodeBuffer> _cache;
    std::function<NodeBuffer(nid_t)> _fetch;
public:
    Storage(std::function<NodeBuffer(nid_t)> fetch, size_t cache_size = 1024);
    NodeBuffer get(nid_t nid);
};

}
