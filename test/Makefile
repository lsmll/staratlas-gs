CC = clang++
CFLAGS := -O3 --std=c++17 -Wall -I../include
CFLAGS += -I$(STARATLAS_SDK)/wasmtime/include
CFLAGS += -I$(STARATLAS_SDK)/local/include

LDFLAGS := -ldl -lm -lpthread
LDFLAGS += -L$(STARATLAS_SDK)/wasmtime/lib -lwasmtime
LDFLAGS += -L../lib -lmrt

SOURCES := $(shell find . -name "*.cpp")
OBJECTS := $(patsubst %.cpp, %.bin, $(SOURCES))

$(info * CFLAGS: $(CFLAGS))
$(info * LDFLAGS: $(LDFLAGS))
$(info * SOURCES: $(SOURCES))

all: $(OBJECTS)

$(OBJECTS):%.bin: %.cpp
	$(CC) $< -o $@ $(CFLAGS) $(LDFLAGS)

.PHONY:clean
clean:
	rm -rf $(OBJECTS)