CC = gcc

SRCDIR=src
INCLDIR=include
	
server: $(SRCDIR)/server.c $(SRCDIR)/utils.o
	$(CC) -pthread -I $(INCLDIR) $(SRCDIR)/utils.o $(SRCDIR)/server.c -o server

$(SRCDIR)/utils.o: $(SRCDIR)/utils.c
	$(CC) -pthread -I $(INCLDIR) -c $(SRCDIR)/utils.c -o $(SRCDIR)/utils.o

clean: 
	rm server $(SRCDIR)/utils.o
