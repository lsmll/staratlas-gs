#include <wasm/libwrt.hh>
#include <sstream>

WASM_DECLARE(g, {
    std::string name = "name";
})

WASM_FILTER{
    auto node = wasm::get_node(nid);

    std::stringstream ss;
    ss << "> nid: " << node->nid() << "[" << nid << "]" << "\n";
    auto idata = node->idata();
    auto odata = node->odata();

    ss << "> idata: ";
    for (size_t i = 0; i < idata->size(); i++) {
        auto e = idata->Get(i);
        ss << e->dst();
        auto attrs = e->attrs_flexbuffer_root().AsMap();
        ss << "(p:" << attrs["p"].AsFloat() << ") ";
    }
    ss << "\n";

    ss << "> odata: ";
    for (size_t i = 0; i < odata->size(); i++) {
        auto e = odata->Get(i);
        ss << e->dst();
        auto attrs = e->attrs_flexbuffer_root().AsMap();
        ss << "(p:" << attrs["p"].AsFloat() << ") ";
    }
    
    wasm::console_log(ss.str());
    // wasm::console_log(g.name);
    wasm::extend_node(12);
    return true;
}

WASM_FINISH{}