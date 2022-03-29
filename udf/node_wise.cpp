#include <wasm/libwrt.hh>

WASM_BEGIN

#include <memory>
#include <random>
#include <unordered_map>

// wasm::DISTINCT _(false);

struct A {
    size_t sst; // 到起点最短路
    size_t cnt; // 被访问次数
    A(): sst(0), cnt(0) {}
};

std::unique_ptr<std::default_random_engine> random_engine;
std::unordered_map<nid_t, A> vis;

const size_t max_depth = 3;
const size_t K = 5;

WASM_FILTER {
    // 初始化随机数发生器
    if (!random_engine) {
        random_engine = std::make_unique<std::default_random_engine>(wasm::clk_m());
    }

    /// 以下为采样代码，注意使用此采样算法需要调度队列参数distinct = false
    
    if (vis.empty()) {
        // vis为空，表示nid为起始节点
        vis[nid].sst = 0;
    }

    auto& vis_nid = vis[nid];
    vis_nid.cnt += 1;

    if (vis_nid.cnt + vis_nid.sst >= max_depth) {
        // 已到达最大访问次数，不再向外拓展
        return true;
    }

    // 获取节点入边
    auto node = wasm::get_node(nid);
    auto idata = node->idata();

    // 选择邻居
    if (idata->size() != 0) {
        // 获取入边邻居的ids
        std::vector<nid_t> nids;
        nids.reserve(idata->size());
        for (auto it = idata->begin(); it != idata->end(); ++it) {
            nids.push_back(it->dst());
        }

        // 随机选择K个邻居
        std::shuffle(nids.begin(), nids.end(), *random_engine);
        std::span sampled = {nids.data(), std::min(nids.size(), K)};

        // 设置邻居属性
        for (auto it = sampled.begin(); it != sampled.end(); ++it) {
            auto vis_it = vis.find(*it);
            if (vis_it == vis.end()) {
                vis[*it].sst = vis_nid.sst + 1;
            }
        }
        wasm::extend_node(sampled);
    }
    
    // 返回true表示nid被采样
    return true;
}

WASM_FINISH{
    // 重置全局变量，实现复用
    vis.clear();
}

WASM_END