#include <core/storage.hh>

namespace wasmtm {

Storage::Storage(std::function<NodeBuffer(nid_t)> fetch, size_t cache_size)
    : _cache(cache_size), _fetch(fetch)
{

}

NodeBuffer Storage::get(nid_t nid) {
    auto elem = this->_cache.TryGet(nid);
    if (elem.second) {
        return elem.first->second;
    } else {
        NodeBuffer buf = this->_fetch(nid);
        this->_cache.Put(nid, buf);
        return buf;
    }
}

}