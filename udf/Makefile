CC = $(STARATLAS_SDK)/wasi-sdk/bin/clang++
CFLAGS := -O3 --std=c++17 -fno-exceptions -Wall -I../include
CFLAGS += -I$(STARATLAS_SDK)/local/include

LDFLAGS := -lm
LDFLAGS += -L../lib -lwrt
LDFLAGS += -Wl,--no-entry
# LDFLAGS += -Wl,--import-memory
# LDFLAGS += -Wl,--strip-all
LDFLAGS += -Wl,--allow-undefined
LDFLAGS += -Wl,--export=__wasm_call_ctors
LDFLAGS += -Wl,--export=__wasm_call_dtors
LDFLAGS += -Wl,--export=malloc
LDFLAGS += -Wl,--export=free
LDFLAGS += -Wl,--export=filter
LDFLAGS += -Wl,--export=finish

SOURCES := $(shell find . -name "*.cpp")
OBJECTS := $(patsubst %.cpp, %.wasm, $(SOURCES))

$(info * CFLAGS: $(CFLAGS))
$(info * LDFLAGS: $(LDFLAGS))
$(info * SOURCES: $(SOURCES))

all: $(OBJECTS)

$(OBJECTS):%.wasm: %.cpp
	$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)

.PHONY:clean
clean:
	rm -rf $(OBJECTS)
	