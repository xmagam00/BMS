CC=g++
CFLAGS=-std=c++11 -Wall -pedantic -O2
LIBS=-lm

all: bms1A bms1B


bms1A: bms1A.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

bms1B: bms1B.cpp
	$(CC) $(CFLAGS) $(LIBS) $^ -o $@ libsndfile.a

clean:
	rm -f *.o bms1A bms1B xamgam00.zip
zip:
	zip xmagam00.zip Makefile bms1A.cpp bms1B.cpp sndfile.h sndfile.hh
