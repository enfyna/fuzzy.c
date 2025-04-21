default: build/main

CC=clang
CFLAGS=-Wall -Wextra -pedantic -I./src/include/

RAYLIB_H=./raylib-5.5_linux_amd64/include
RAYLIB=-I$(RAYLIB_H) -L./raylib-5.5_linux_amd64/lib -l:libraylib.a -lm 

HEADERS=$(wildcard src/include/*.h)
SOURCES=$(wildcard src/*.c)

build/main: $(SOURCES) $(HEADERS)
	mkdir -p build
	$(CC) $(CFLAGS) -xc $(SOURCES) -o $@ $(RAYLIB) 


.PHONY: clean
clean:
	rm -fr build
