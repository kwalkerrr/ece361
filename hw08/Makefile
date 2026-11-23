CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wshadow
# Only used by memcheck. no-recover makes undefined behavior stop the program
# with a failing exit status instead of printing a warning and carrying on.
SANITIZE = -fsanitize=address,undefined -fno-sanitize-recover=all

OBJS = bst.o treestats.o
BIN = treestats
TEST_OBJS = tests/test_bst.o
TEST_BIN = tests/test_bst

.PHONY: all test memcheck clean

all: $(BIN) $(TEST_BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST_BIN)
	./$(TEST_BIN)

# Starts from clean so every object is rebuilt with the sanitizers, then runs
# the tests. Leaves sanitized objects behind; run make clean before a plain
# build.
memcheck:
	$(MAKE) clean
	$(MAKE) test CFLAGS="$(CFLAGS) $(SANITIZE)"

$(TEST_BIN): bst.o $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

bst.o: bst.h
treestats.o: bst.h
tests/test_bst.o: bst.h

clean:
	rm -f $(OBJS) $(BIN) $(TEST_OBJS) $(TEST_BIN)
