CC = g++ --std=c++17

CFLAGS = -Ofast -march=native -Wall -Wextra -Wpedantic -Wdouble-promotion

LIBS = -lSDL2

tunnel: tunnel.cc softshader.hh Makefile
	$(CC) $(CFLAGS) $(LIBS) $< -o $@
