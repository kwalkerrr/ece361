CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wshadow -fsanitize=address

OBJS = main.o reading.o stats.o histogram.o
TEST_OBJS = tests/test_stats.o
TEST_BIN = tests/test_stats

.PHONY: all test clean

all: readings

test: readings $(TEST_BIN)
	./$(TEST_BIN)
	sh regress.sh

readings: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_BIN): stats.o $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: reading.h stats.h histogram.h
reading.o: reading.h
stats.o: stats.h
histogram.o: histogram.h
tests/test_stats.o: stats.h

clean:
	rm -f readings $(OBJS) $(TEST_OBJS) $(TEST_BIN)
