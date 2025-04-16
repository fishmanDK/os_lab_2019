CC = gcc
CFLAGS = -pthread
SERVER = server
CLIENT = client

K ?= 1000
MOD ?= 5
SERVERS_FILE ?= servers.txt

all: $(SERVER) $(CLIENT)

$(SERVER): server.o multmodulo.o
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT): client.o multmodulo.o 
	$(CC) -o $@ $^

%.o: %.c multmodulo.h
	$(CC) -c $<

clean:
	rm -f $(SERVER) $(CLIENT) *.o

run-server1:
	./server --port 20001 --tnum 4

run-server2:
	./server --port 20002 --tnum 4

run-client:
	./client --k $(K) --mod $(MOD) --servers $(SERVERS_FILE)

.PHONY: all clean run-server1 run-server2 run-client