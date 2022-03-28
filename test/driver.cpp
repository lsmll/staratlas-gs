#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <rand_graph.hpp>
#include <core/scheduler.hh>
#include <core/storage.hh>

using namespace wasmtm;

/**
 * @brief 测试用driver
 * 
 * @return int 
 */
int main(int argc, char* argv[]) {

    std::vector<u8> wasm_bytes;
    if (argc == 2) {
        const char* fn = argv[1];

        std::ifstream wasm_file(fn, std::ios::binary|std::ios::in);
        if (!wasm_file) {
            std::cerr << "can\'t open file: " << fn << std::endl;
            exit(1);
        }

        wasm_file.seekg(0, std::ios::end);
        size_t wasm_size = wasm_file.tellg();
        if (wasm_size == 0) {
            std::cerr << "file is empty: " << fn << std::endl;
            exit(1);
        }

        wasm_bytes.resize(wasm_size);

        std::cout << "reading file: " << fn << "." << std::endl;
        wasm_file.seekg(0, std::ios::beg);
        wasm_file.read(reinterpret_cast<char*>(wasm_bytes.data()), wasm_size);
    } else {
        std::cerr << "usage: driver $.wasm" << std::endl;
        exit(1);
    }

    std::cout << "generating random graph for testing." << std::endl;
    auto graph = gen_random_graph(1000, 20);

    std::cout << "creating storage." << std::endl;
    auto store = std::make_shared<Storage>([&](nid_t nid) {
        auto it = graph.find(nid);
        if (it != graph.end()) {
            return it->second;
        } else {
            return NodeBuffer();
        }
    });

    std::cout << "creating scheduler." << std::endl;
    SchedulerBlock a(SchedulerQueue::FIFO, store, true);

    if (!a.initialize({wasm_bytes.data(), wasm_bytes.size()})) {
        std::cerr << "failed!" << std::endl;
        exit(1);
    }

    nid_t src = 200;
    std::cout << "starting from node(" << src << ")" << std::endl;
    a.push(src);

    while(a.poll()) {
        std::cout << "scheduler queue is not empty." << std::endl;
    }

    a.finish();
    std::cout << "flushed." << std::endl;

    std::cout << "finished." << std::endl;

    auto selected = a.selected();
    std::cout << "selected nodes: ";
    for (size_t i=0; i<selected.size(); i++) {
        std::cout << selected[i] << " ";
    }
    std::cout << std::endl;

    a.clear();
    std::cout << "cleaned." << std::endl;

    return 0;
}