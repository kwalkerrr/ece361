CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

OBJS = bits.o status.o
TEST_OBJS = tests/test_bits.o tests/test_status.o
TEST_BINS = tests/test_bits tests/test_status

.PHONY: all test clean

all: $(OBJS) $(TEST_BINS)

test: all
	./tests/test_bits
	./tests/test_status

tests/test_bits: bits.o tests/test_bits.o
	$(CC) $(CFLAGS) -o $@ $^

tests/test_status: bits.o status.o tests/test_status.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

bits.o: bits.h
status.o: status.h bits.h
tests/test_bits.o: bits.h
tests/test_status.o: status.h

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TEST_BINS)
