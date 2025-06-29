default: build/tipper build/tipper-no-gui build/network build/network-no-gui 

CC=clang
CFLAGS=-Wall -Wextra -pedantic -I./src/include/ -g -lm

RAYLIB_DIR ?= ./src/third-party/raylib-5.5_linux_amd64
RAYLIB= -I$(RAYLIB_DIR)/include -L$(RAYLIB_DIR)/lib -l:libraylib.a -lm 

GRAPH_DIR  ?= ./src/third-party/graph.h
GRAPH= -I$(GRAPH_DIR)/include -L$(GRAPH_DIR)/lib -l:graph.a

LIBS=$(RAYLIB) $(GRAPH)

HEADERS=$(wildcard src/include/*.h)
SOURCES=$(wildcard src/*.c)

HEADERS_NOGUI=src/include/csv.h src/include/fuzzy.h src/include/helper.h
SOURCES_NOGUI=src/csv.c src/fuzzy.c src/helper.c

build/tipper: examples/tipper.c $(SOURCES) $(HEADERS)
	mkdir -p build
	$(CC) $(CFLAGS) -xc examples/tipper.c $(SOURCES) -o $@ $(LIBS) 

build/tipper-no-gui: examples/tipper-no-gui.c 
	mkdir -p build
	$(CC) $(CFLAGS) -xc examples/tipper-no-gui.c $(SOURCES_NOGUI) -o $@

build/network: examples/network.c $(SOURCES) $(HEADERS)
	mkdir -p build
	$(CC) $(CFLAGS) -xc examples/network.c $(SOURCES) -o $@ $(LIBS) 

build/network-no-gui: examples/network-no-gui.c 
	mkdir -p build
	$(CC) $(CFLAGS) -xc examples/network-no-gui.c $(SOURCES_NOGUI) -o $@

.PHONY: clean
clean:
	rm -fr build
