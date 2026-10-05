CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

TEST_BIN = tests/test_bits

.PHONY: test clean

test: $(TEST_BIN)

$(TEST_BIN): bits.c bits.h tests/test_bits.c
	$(CC) $(CFLAGS) -o $@ bits.c tests/test_bits.c

clean:
	rm -f $(TEST_BIN)
