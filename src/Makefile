CC = g++ --std=c++17

CFLAGS = -Ofast -march=native -Wall -Wextra -Wpedantic -Wdouble-promotion

LDFLAGS = -lSDL2 -lpthread

DEPS = softshader.hh Makefile

../tunnel: tunnel.cc $(DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm ../tunnel
