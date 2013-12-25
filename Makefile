CFLAGS=-Wall -g -std=c++11
CC=g++

yal:
	$(CC) $(CFLAGS) yal.hpp yal_main.cpp -o test

clean:
	rm test.exe
	rm test
