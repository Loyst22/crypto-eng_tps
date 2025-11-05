CC = clang 
CFLAGS = -g -Wall
SRCS = second_preim_48_fillme.c tests.c
HDR = xoshiro.h 
TARGET = tests 

all: $(TARGET)

$(TARGET): $(SRCS) $(HDR)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
