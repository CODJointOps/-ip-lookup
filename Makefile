CC=gcc
CFLAGS=-lcurl -lcjson
SRCDIR=src
OBJDIR=bin

# Use variables for source and object files
ip-lookup: $(SRCDIR)/main.c
	$(CC) -o $(OBJDIR)/ip-lookup $(SRCDIR)/main.c $(CFLAGS)

