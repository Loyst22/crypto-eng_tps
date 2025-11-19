CC = clang 
CFLAGS = -g -Wall -O3 -march=native
SRCS = second_preim_48_fillme.c
SRCS_TEST = tests.c
SRCS_ATTACK = attack.c
HDR = xoshiro.h second_preim.h hasht.h
TARGET = tests 

all: $(TARGET)

$(TARGET): $(SRCS) $(HDR)
	$(CC) $(CFLAGS) $(SRCS) $(SRCS_TEST) -o $(TARGET)

attack: $(SRCS) $(HDR)
	$(CC) $(CFLAGS) $(SRCS) $(SRCS_ATTACK) -o attack

clean:
	rm -f $(TARGET) attack

.PHONY: all clean
