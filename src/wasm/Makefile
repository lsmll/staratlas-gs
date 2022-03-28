CC = /home/hwj/.opt/wasi-sdk-14.0/bin/clang++
AR = /home/hwj/.opt/wasi-sdk-14.0/bin/ar
CFLAGS := -O3 --std=c++17 -fno-exceptions -Wall -I../../include
CFLAGS += -I/home/hwj/.local/include

SOURCES := $(shell find . -name "*.cpp")
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))

$(info * CFLAGS: $(CFLAGS))
$(info * SOURCES: $(SOURCES))

all: $(OBJECTS)
	$(AR) -rv ../../lib/libwrt.a $(OBJECTS)

$(OBJECTS):%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:clean
clean:
	rm -rf $(OBJECTS)
	