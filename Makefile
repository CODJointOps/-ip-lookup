CC=gcc
CFLAGS=-lcurl -lcjson

ip-lookup: main.c
	$(CC) -o ip-lookup main.c $(CFLAGS)
