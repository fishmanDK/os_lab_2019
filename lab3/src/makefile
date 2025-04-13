CC = gcc
CFLAGS = -Wall -Wextra -I.

TARGETS = sequential_min_max parallel_min_max runner
OBJS = utils.o find_min_max.o

.PHONY: all clean

all:
	$(TARGETS)

sequential_min_max: sequential_min_max.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

parallel_min_max: parallel_min_max.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

runner: runner.c
	$(CC) $(CFLAGS) -o $@ runner.c

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c $< -o $@

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) $(OBJS) tmp_min_max_*.txt