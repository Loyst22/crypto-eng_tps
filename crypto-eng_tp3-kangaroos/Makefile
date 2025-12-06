CC = clang 
CFLAGS = -g -O3 -Wall -Wextra -march=native -ffast-math
LDFLAGS = -lm
SRCS = kangaroos.c tests.c
HDR = mul11585.h kangaroos.h hasht.h

all: tests perf_tests

tests: tests.c kangaroos.c $(HDR)
	$(CC) $(CFLAGS) tests.c kangaroos.c $(LDFLAGS) -o tests

perf_tests: perf_tests.c kangaroos.c $(HDR)
	$(CC) $(CFLAGS) perf_tests.c kangaroos.c $(LDFLAGS) -o perf_tests

clean:
	rm -f $(TARGET) attack

.PHONY: all clean
