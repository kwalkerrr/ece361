CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wshadow -fsanitize=address
LDLIBS = -lm

LIB_OBJS = iom361_r4.o panel.o scenario.o sensor.o
OBJS = $(LIB_OBJS) main.o
BIN = replay
TEST_OBJS = tests/test_hw06.o
TEST_BIN = tests/test_hw06

.PHONY: all test clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

test: $(BIN) $(TEST_BIN)
	./$(TEST_BIN)
	sh regress.sh

$(TEST_BIN): $(LIB_OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

iom361_r4.o: iom361_r4.h
main.o: iom361_r4.h panel.h scenario.h sensor.h
panel.o: panel.h iom361_r4.h
scenario.o: scenario.h
sensor.o: sensor.h iom361_r4.h
tests/test_hw06.o: iom361_r4.h panel.h scenario.h sensor.h

clean:
	rm -f $(OBJS) $(BIN) $(TEST_OBJS) $(TEST_BIN)
