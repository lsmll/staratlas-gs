/**
 * @file rand_graph.hpp
 * @author wenjie (wjie@zju.edu.cn)
 * @brief 随机图生成器（用于调试）
 * @version 0.1
 * @date 2022-03-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <core/types.hh>
#include <unordered_map>
#include <vector>
#include <memory>
#include <random>

#include <core/buffer.hh>
#include <relations_generated.h>

/**
 * @brief 生成一个random graph
 * 
 * @param n 
 * @param m 
 * @return std::unordered_map<u64, GVal> 
 */
std::unordered_map<u64, wasmtm::NodeBuffer> gen_random_graph(u64 n, u64 m) {
    using namespace wasmtm;

    std::default_random_engine e(time(NULL));
    std::uniform_int_distribution<u64> u(1, n);
    std::uniform_real_distribution<f32> r(0.0, 1.0);

    std::vector<flatbuffers::FlatBufferBuilder> fbbs(n+1);
    std::vector<std::vector<flatbuffers::Offset<Edge>>> idatas(n+1), odatas(n+1);
    for (u64 i=1; i<=n; i++) {
        const u64 nid = i;
        for (u64 j=0; j<m; j++) {
            const u64 dst = u(e);
            flexbuffers::Builder attrs;
            attrs.Map([&](){
                attrs.Float("p", r(e));
            });
            attrs.Finish();
            auto onid = CreateEdgeDirect(fbbs[nid], dst, &attrs.GetBuffer());
            odatas[nid].push_back(onid);

            auto idst = CreateEdgeDirect(fbbs[dst], nid, &attrs.GetBuffer());
            idatas[dst].push_back(idst);
        }
    }

    std::unordered_map<u64, NodeBuffer> g;
    for (u64 i=1; i<=n; i++) {
        const u64 nid = i;
        auto& fbb = fbbs[nid];

        auto idata = fbb.CreateVector(idatas[nid]);
        auto odata = fbb.CreateVector(odatas[nid]);
        auto root = CreateNode(fbb, nid, 0, idata, odata);
        fbb.Finish(root);

        g.insert({nid, NodeBuffer({fbb.GetBufferPointer(), fbb.GetSize()})});
    }

    return g;
}
