CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g -Werror -Wextra -W
LIBS=-lm

all: bms1A bms1B


bms1A: bms1A.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

bms1B: bms1B.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

clean:
	rm -f *.o bms1A bms1B
