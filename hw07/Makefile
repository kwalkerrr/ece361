CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wshadow
# Only used by memcheck. no-recover makes undefined behavior stop the program
# with a failing exit status instead of printing a warning and carrying on.
SANITIZE = -fsanitize=address,undefined -fno-sanitize-recover=all

OBJS = istack.o balance.o
BIN = balance
TEST_OBJS = tests/test_istack.o
TEST_BIN = tests/test_istack

.PHONY: all test memcheck clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test: $(BIN) $(TEST_BIN)
	./$(TEST_BIN)
	sh regress.sh

# Starts from clean so every object is rebuilt with the sanitizers, then runs
# the tests. Leaves sanitized objects behind; run make clean before a plain
# build.
memcheck:
	$(MAKE) clean
	$(MAKE) test CFLAGS="$(CFLAGS) $(SANITIZE)"

$(TEST_BIN): istack.o $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

istack.o: istack.h
balance.o: istack.h
tests/test_istack.o: istack.h

clean:
	rm -f $(OBJS) $(BIN) $(TEST_OBJS) $(TEST_BIN)
