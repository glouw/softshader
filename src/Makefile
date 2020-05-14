CC = g++ --std=c++17

CFLAGS = -Ofast -march=native -Wall -Wextra -Wpedantic -Wdouble-promotion -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow

LDFLAGS = -lSDL2 -lpthread

DEPS = softshader.hh Makefile

all: ../tunnel ../creation

../tunnel:   tunnel.cc   $(DEPS); $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@
../creation: creation.cc $(DEPS); $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm ../tunnel
	rm ../creation
