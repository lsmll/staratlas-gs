#include <wasm/libwrt.hh>

WASM_BEGIN

#include <memory>
#include <random>
#include <unordered_map>


std::unique_ptr<std::default_random_engine> random_engine;
size_t node_count = 0;

const size_t K = 5 * 3;

WASM_FILTER {
    // 初始化随机数发生器
    if (!random_engine) {
        random_engine = std::make_unique<std::default_random_engine>(wasm::clk_m());
    }

    /// 以下为采样代码，注意使用此采样算法需要调度队列参数distinct = true

    // 获取节点入边
    auto node = wasm::get_node(nid);
    auto idata = node->idata();
    
    if ((++node_count) > K) {
        return false;
    }

    // 选择邻居
    if (idata->size() != 0) {
        // 获取入边邻居的ids
        std::vector<nid_t> nids;
        nids.reserve(idata->size());
        for (auto it = idata->begin(); it != idata->end(); ++it) {
            nids.push_back(it->dst());
        }

        // 优先级选择
        std::vector<f64> ps(nids.size());
        std::uniform_real_distribution<f64> u(0, 1);
        for (size_t i = 0; i < ps.size(); i++) {
            ps[i] = u(*random_engine);
        }

        wasm::extend_node(nids, ps);
    }
    
    return true;
}

WASM_FINISH{
    // 重置全局变量，实现复用
    node_count = 0;
}

WASM_END