#include <wasm/libwrt.hh>

#include <random>
#include <unordered_map>

WASM_FILTER {

    std::default_random_engine e(nid);

    auto node = wasm::get_node(nid);
    auto idata = node->idata();

    if (idata->size() != 0) {
        std::vector<nid_t> nids;
        nids.reserve(idata->size());
        for (auto it = idata->begin(); it != idata->end(); ++it) {
            nids.push_back(it->dst());
        }
        std::shuffle(nids.begin(), nids.end(), e);
        wasm::extend_node({nids.data(), 5});
    }
    return true;
}

WASM_FINISH{}
