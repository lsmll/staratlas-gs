#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

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
            std::cerr << "@ can\'t open file: " << fn << std::endl;
            exit(1);
        }

        wasm_file.seekg(0, std::ios::end);
        size_t wasm_size = wasm_file.tellg();
        if (wasm_size == 0) {
            std::cerr << "@ file is empty: " << fn << std::endl;
            exit(1);
        }

        wasm_bytes.resize(wasm_size);

        std::cout << "@ reading file: " << fn << "." << std::endl;
        wasm_file.seekg(0, std::ios::beg);
        wasm_file.read(reinterpret_cast<char*>(wasm_bytes.data()), wasm_size);
    } else {
        std::cerr << "usage: driver $.wasm" << std::endl;
        exit(1);
    }

    std::cout << "@ generating random graph for testing" << std::endl;
    auto graph = gen_random_graph(1000, 20);

    std::cout << "@ creating storage" << std::endl;
    auto store = std::make_shared<Storage>([&](nid_t nid) {
        auto it = graph.find(nid);
        if (it != graph.end()) {
            return it->second;
        } else {
            return NodeBuffer();
        }
    });

    std::cout << "@ creating scheduler" << std::endl;
    SchedulerBlock a(SchedulerQueue::FIFO, store, true);

    if (!a.initialize({wasm_bytes.data(), wasm_bytes.size()})) {
        std::cerr << "@ failed!" << std::endl;
        exit(1);
    }

    // for (nid_t s = 1; s <= 3; s ++) {
    //     std::cout << "@ starting from node(" << s << ")" << std::endl;
    //     a.push(s);

    //     while(a.poll()) {
    //         std::cout << "@ scheduler queue is not empty." << std::endl;
    //     }

    //     a.finish();
    //     std::cout << "@ finished" << std::endl;

    //     auto selected = a.selected();
    //     std::cout << "> ";
    //     for (size_t i=0; i<selected.size(); i++) {
    //         std::cout << selected[i] << " ";
    //     }
    //     std::cout << std::endl;

    //     std::cout << "@ cleaned" << std::endl;
    //     a.clear();
    // }

    for (nid_t s = 1; s <= 3; s ++) {
        auto selected = a.sample(s);
        std::sort(selected.begin(), selected.end());
        
        std::cout << s << "> ";
        for (size_t i=0; i<selected.size(); i++) {
            std::cout << selected[i] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}