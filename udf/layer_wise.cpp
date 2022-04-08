#include <wasm/libwrt.hh>

WASM_BEGIN

#include <memory>
#include <random>
#include <unordered_map>


std::unique_ptr<std::default_random_engine> random_engine;
std::unordered_map<nid_t, size_t> vis;

const size_t max_depth = 3;
const size_t K = 5;

size_t layer_count[max_depth + 1];

WASM_FILTER {
    // 初始化随机数发生器
    if (!random_engine) {
        random_engine = std::make_unique<std::default_random_engine>(wasm::clk_m());
    }

    /// 以下为采样代码，注意使用此采样算法需要调度队列参数distinct = true
    
    if (vis.empty()) {
        // vis为空，表示nid为起始节点
        vis[nid] = 0;
        memset(layer_count, 0, sizeof(layer_count));
    }

    // 获取节点入边
    auto node = wasm::get_node(nid);
    auto idata = node->idata();
    
    // 每层最多保留K个点
    size_t cur_d = vis[nid];
    if ((++layer_count[cur_d]) > K) {
        return false;
    }

    // 选择邻居
    if (idata->size() != 0) {
        // 获取入边邻居的ids
        std::vector<nid_t> nids;
        nids.reserve(idata->size());
        for (auto it = idata->begin(); it != idata->end(); ++it) {
            nid_t v = it->dst();
            size_t dst_d = cur_d + 1;
            
            if (vis.find(v) != vis.end()) {
                dst_d = vis[v];
            }

            if (dst_d <= max_depth) {
                vis[v] = dst_d;
                nids.push_back(it->dst());
            }
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
    vis.clear();
}

WASM_END