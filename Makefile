CC = clang 
CFLAGS = -g -Wall
SRCS = attack.c aes-128_enc.c tests.c keyed_function.c
HDR = aes-128_enc.h attack.h keyed_function.h
TARGET = tests 

all: $(TARGET)

$(TARGET): $(SRCS) $(HDR)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
