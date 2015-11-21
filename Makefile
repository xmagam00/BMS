CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g
LIBS=-lm

all: bms1A 


bms1A: bms1A.c
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

#bms1B: bms1B.c
#	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

clean:
	rm -f *.o bms1A bms1B
