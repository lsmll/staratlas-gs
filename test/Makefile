CC = clang++
CFLAGS := -O3 --std=c++17 -Wall -I../include
CFLAGS += -I/home/hwj/.wasmtime/include
CFLAGS += -I/home/hwj/.local/include

LDFLAGS := -ldl -lm -lpthread
LDFLAGS += -L/home/hwj/.wasmtime/lib -lwasmtime
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