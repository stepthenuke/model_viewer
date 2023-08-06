CC=clang

CFLAGS=-std=c11 -Wall -I./src/include -I./src/include/glad/include -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw

CFILES=src/main.c src/glad.c
HFILES=src/common.h src/shader.h src/camera.h

build/main: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) -O2 $(CFILES) -o build/main

build/debug: $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) $(CFILES) -g -fsanitize=address -fsanitize=undefined -o build/debug

run:
	build/debug

clean:
	rm -f build/main build/debug
	rm -r build/*.dSYM
