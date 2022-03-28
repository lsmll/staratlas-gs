CC = clang++
AR = ar
CFLAGS := -O3 --std=c++17 -Wall -I../../include
CFLAGS += -I/home/hwj/.wasmtime/include
CFLAGS += -I/home/hwj/.local/include

SOURCES := $(shell find . -name "*.cpp")
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))

$(info * CFLAGS: $(CFLAGS))
$(info * SOURCES: $(SOURCES))

all: $(OBJECTS)
	$(AR) -rv ../../lib/libmrt.a $(OBJECTS)

$(OBJECTS):%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -rf $(OBJECTS)