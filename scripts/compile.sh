#!/bin/bash

ROOT=$(cd `dirname $0` && pwd)/..

WASI_SDK=$HOME/.opt/wasi-sdk-14.0
CC=$WASI_SDK/bin/clang++
AR=$WASI_SDK/bin/ar

CFLAGS="-O3 --std=c++17 \
-fno-exceptions -Wall \
-I$ROOT/include \
-I$HOME/.local/include \
"

LDFLAGS="-lm \
-L$ROOT/lib -lwrt \
-Wl,--no-entry \
-Wl,--strip-all \
-Wl,--allow-undefined \
-Wl,--export=__wasm_call_ctors \
-Wl,--export=__wasm_call_dtors \
-Wl,--export=malloc \
-Wl,--export=free \
-Wl,--export=filter \
-Wl,--export=finish \
"

SRC=$1
OBJ=${SRC%.cpp}.wasm

$CC $SRC $CFLAGS $LDFLAGS -o $OBJ
