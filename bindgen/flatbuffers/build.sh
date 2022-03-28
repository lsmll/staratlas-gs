#!/bin/bash

VER="2.0.6"

cd `dirname $0`
LIB="../../lib"
rm -rf $LIB && mkdir $LIB

CFLAGS="-O3 --std=c++17 -Wall -I$HOME/.local/include"

function build_host() {
    CC=clang++-10
    AR=llvm-ar-10

    $CC -c reflection.cpp $CFLAGS
    $CC -c util.cpp $CFLAGS
    
    $AR -rv $LIB/libmrt.a reflection.o util.o
    rm -rf *.o
}

function build_wasm() {
    WASI_SDK=$HOME/.opt/wasi-sdk-14.0
    CC=$WASI_SDK/bin/clang++
    AR=$WASI_SDK/bin/ar

    $CC -c reflection.cpp $CFLAGS
    $CC -c util.cpp $CFLAGS -DFLATBUFFERS_NO_ABSOLUTE_PATH_RESOLUTION
    
    $AR -rv $LIB/libwrt.a reflection.o util.o
    rm -rf *.o
}

build_host && build_wasm