CC = clang 
CFLAGS = -g -O3 -Wall -Wextra -march=native
SRCS = kangaroos.c tests.c
HDR = mul11585.h kangaroos.h
TARGET = tests 

all: $(TARGET)

$(TARGET): $(SRCS) $(HDR)
	$(CC) $(CFLAGS) $(SRCS) $(SRCS_TEST) -o $(TARGET)

clean:
	rm -f $(TARGET) attack

.PHONY: all clean
