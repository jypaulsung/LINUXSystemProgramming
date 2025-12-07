CC = gcc
CFLAGS =
LDFLAGS = -lnsl -lpthread
# -lsocket caused error

.SUFFIXES : .c .o
.c.o :
	$(CC) -c $(CFLAGS) $<

ALL = chats chatc chats_singlethread chatc_t

all: $(ALL)

chats: chats.o 
	$(CC) -o $@ $< $(LDFLAGS)

chatc: chatc.o 
	$(CC) -o $@ $< $(LDFLAGS)

chats_singlethread: chats_singlethread.o 
	$(CC) -o $@ $< $(LDFLAGS)

chatc_t: chatc_t.o
	$(CC) -o $@ $< $(LDFLAGS)

clean :
	rm -rf *.o $(ALL)