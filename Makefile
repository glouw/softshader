CC = g++ --std=c++17

CFLAGS = -flto -Ofast -march=native -Wall -Wextra -Wpedantic -Wdouble-promotion

LIBS = -lSDL2

tunnel: tunnel.cc softshader.hh Makefile
	$(CC) $(CFLAGS) $(LIBS) $< -o $@
