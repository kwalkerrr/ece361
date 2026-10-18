CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wshadow -fsanitize=address

OBJS = ptr.o
TEST_OBJS = tests/test_ptr.o
TEST_BIN = tests/test_ptr

.PHONY: test clean

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): ptr.o $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

ptr.o: ptr.h
tests/test_ptr.o: ptr.h

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TEST_BIN)
