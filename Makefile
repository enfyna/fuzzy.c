default: build/tipper build/tipper-no-gui build/tipper-sugeno-no-gui build/network build/network-no-gui build/mobile-robot

CC=clang
CFLAGS=-Wall -Wextra -pedantic -I./src/include/ -g -lm

RAYLIB_DIR ?= ./src/third-party/raylib-5.5_linux_amd64
RAYLIB= -I$(RAYLIB_DIR)/include -L$(RAYLIB_DIR)/lib -l:libraylib.a -lm 

GRAPH_DIR  ?= ./src/third-party/graph.h
GRAPH= -I$(GRAPH_DIR)/include -L$(GRAPH_DIR)/lib -l:graph.a

LIBS=$(RAYLIB) $(GRAPH)

HEADERS=$(wildcard src/include/*.h)
SOURCES=$(wildcard src/*.c)

HEADERS_NOGUI=src/include/csv.h src/include/fuzzy.h 
SOURCES_NOGUI=src/csv.c src/fuzzy.c 

build/tipper: examples/tipper.c $(SOURCES) $(HEADERS)
	mkdir -p build
	$(CC) $(CFLAGS) -xc examples/tipper.c $(SOURCES) -o $@ $(LIBS) 

build/tipper-no-gui: examples/tipper-no-gui.c $(SOURCES_NOGUI) 
	mkdir -p build
	$(CC) $(CFLAGS) -xc examples/tipper-no-gui.c $(SOURCES_NOGUI) -o $@

build/tipper-sugeno-no-gui: examples/tipper-sugeno-no-gui.c $(SOURCES_NOGUI) 
	mkdir -p build
	$(CC) $(CFLAGS) -xc -g examples/tipper-sugeno-no-gui.c $(SOURCES_NOGUI) -o $@

build/network: examples/network.c $(SOURCES) $(HEADERS)
	mkdir -p build
	$(CC) $(CFLAGS) -xc examples/network.c $(SOURCES) -o $@ $(LIBS) 

build/network-no-gui: examples/network-no-gui.c $(SOURCES_NOGUI) 
	mkdir -p build
	$(CC) $(CFLAGS) -xc examples/network-no-gui.c $(SOURCES_NOGUI) -o $@

build/mobile-robot: examples/mobile-robot.c  $(SOURCES) $(HEADERS)
	mkdir -p build
	$(CC) $(CFLAGS) -xc examples/mobile-robot.c $(SOURCES) -o $@ $(LIBS) -DFZ_NO_LOGS

.PHONY: clean
clean:
	rm -fr build
