CC = g++ --std=c++17

CFLAGS = -Ofast -march=native -funroll-loops -Wall -Wextra -Wpedantic -Wdouble-promotion
# CFLAGS+= -fsanitize=undefined -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow

LDFLAGS = -lSDL2 -lpthread

DEPS = softshader.hh Makefile

all: ../tunnel ../creation ../seascape

../tunnel:   tunnel.cc   $(DEPS); $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@
../creation: creation.cc $(DEPS); $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@
../seascape: seascape.cc $(DEPS); $(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm ../tunnel
	rm ../creation
	rm ../seascape
