CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

OBJS = bits.o status.o
TEST_OBJS = tests/test_bits.o
TEST_BIN = tests/test_bits

.PHONY: all test clean

all: $(OBJS) $(TEST_BIN)

test: all
	./$(TEST_BIN)

$(TEST_BIN): bits.o $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

bits.o: bits.h
status.o: status.h bits.h
tests/test_bits.o: bits.h

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TEST_BIN)
