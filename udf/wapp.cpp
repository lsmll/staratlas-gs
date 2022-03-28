#include <wasm/libwrt.hh>
#include <sstream>

// 数据读写案例
WASM_FILTER{
    auto node = wasm::get_node(nid); // 根据节点id获取属性和邻居

    std::stringstream ss;
    ss << "- nid: " << node->nid() << "\n"; // 节点id
    auto idata = node->idata(); // 入边
    auto odata = node->odata(); // 出边

    ss << "- idata: ";
    for (size_t i = 0; i < idata->size(); i++) {
        auto e = idata->Get(i);
        ss << e->dst(); // 邻居节点id
        auto attrs = e->attrs_flexbuffer_root().AsMap();  // 入边属性
        ss << "(p:" << attrs["p"].AsFloat() << ") ";
    }
    ss << "\n";

    ss << "- odata: ";
    for (size_t i = 0; i < odata->size(); i++) {
        auto e = odata->Get(i);
        ss << e->dst(); // 邻居节点id
        auto attrs = e->attrs_flexbuffer_root().AsMap(); // 出边属性
        ss << "(p:" << attrs["p"].AsFloat() << ") ";
    }
    
    wasm::console_log(ss.str()); // 向终端输出字符串
    return true;
}

WASM_FINISH{}