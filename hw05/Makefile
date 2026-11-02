CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wshadow -fsanitize=address

OBJS = lock.o main.o
BIN = lock
TEST_OBJS = tests/test_lock.o
TEST_BIN = tests/test_lock

.PHONY: all test clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): lock.o $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

lock.o: lock.h
main.o: lock.h
tests/test_lock.o: lock.h

clean:
	rm -f $(OBJS) $(BIN) $(TEST_OBJS) $(TEST_BIN)
