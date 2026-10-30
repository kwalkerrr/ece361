CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wshadow -fsanitize=address

OBJS = aux.o fsm.o iom361_r4.o main.o report.o sensor.o store.o
BIN = alarm_panel

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

aux.o: aux.h
fsm.o: fsm.h
iom361_r4.o: iom361_r4.h
main.o: iom361_r4.h
report.o: report.h
sensor.o: sensor.h
store.o: store.h

clean:
	rm -f $(OBJS) $(BIN)
