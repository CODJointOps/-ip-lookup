CC=gcc
CFLAGS=-lcurl -lcjson
SRCDIR=src
OBJDIR=bin

# Use variables for source and object files
ip-lookup: $(SRCDIR)/main.c
	@mkdir -p $(OBJDIR)
	$(CC) -o $(OBJDIR)/ip-lookup $(SRCDIR)/main.c $(CFLAGS)
