#include <driver.hh>

using namespace wasmtm;

int main(int argc, char* argv[]) {

    std::vector<u8> wasm_bytes;
    if (argc == 2) {
        if (!load_wasm(argv[1], wasm_bytes)) {
            exit(1);
        }
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