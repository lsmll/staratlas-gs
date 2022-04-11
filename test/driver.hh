#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

#include <rand_graph.hpp>
#include <core/scheduler.hh>
#include <core/storage.hh>

bool load_wasm(const char* fn, std::vector<u8>& wasm_bytes) {
    wasm_bytes.clear();

    std::ifstream wasm_file(fn, std::ios::binary|std::ios::in);
    if (!wasm_file) {
        std::cerr << "@ can\'t open file: " << fn << std::endl;
        return false;
    }

    wasm_file.seekg(0, std::ios::end);
    size_t wasm_size = wasm_file.tellg();
    if (wasm_size == 0) {
        std::cerr << "@ file is empty: " << fn << std::endl;
        return false;
    }

    wasm_bytes.resize(wasm_size);

    std::cout << "@ reading file: " << fn << "." << std::endl;
    wasm_file.seekg(0, std::ios::beg);
    wasm_file.read(reinterpret_cast<char*>(wasm_bytes.data()), wasm_size);

    return true;
}