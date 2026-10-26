CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wshadow -fsanitize=address

OBJS = tally.o events.o
BIN = events
TEST_OBJS = tests/test_tally.o
TEST_BIN = tests/test_tally

.PHONY: all test clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): tally.o $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

tally.o: tally.h
events.o: tally.h
tests/test_tally.o: tally.h

clean:
	rm -f $(OBJS) $(BIN) $(TEST_OBJS) $(TEST_BIN)
