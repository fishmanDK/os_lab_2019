CC = gcc
CFLAGS = -I. -Wall -Wextra -pthread

all: parallel_min_max process_memory parallel_sum

parallel_min_max: parallel_min_max.o find_min_max.o utils.o
	$(CC) -o parallel_min_max parallel_min_max.o find_min_max.o utils.o $(CFLAGS)

process_memory: process_memory.o
	$(CC) -o process_memory process_memory.o $(CFLAGS)

parallel_sum: parallel_sum.o
	$(CC) -o parallel_sum parallel_sum.o $(CFLAGS)

parallel_min_max.o: parallel_min_max.c find_min_max.h utils.h
	$(CC) -c parallel_min_max.c $(CFLAGS)

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -c find_min_max.c $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) -c utils.c $(CFLAGS)

process_memory.o: process_memory.c
	$(CC) -c process_memory.c $(CFLAGS)

parallel_sum.o: parallel_sum.c
	$(CC) -c parallel_sum.c $(CFLAGS)

clean:
	rm -f *.o parallel_min_max process_memory parallel_sum