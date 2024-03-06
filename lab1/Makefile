
CC=g++
FLAGS=-std=c++20 -pedantic -Wall -Wextra -Wno-unused-variable
OUTPUT=main

all: sort.o  main.cpp
	$(CC) $(FLAGS) sort.o  main.cpp -o $(OUTPUT)
sort.o: sort.cpp sort.hpp
	$(CC) $(FLAGS) -c sort.cpp

benchmark: sort.o  benchmark.cpp
	$(CC) $(FLAGS) sort.o  benchmark.cpp -o benchmark
clean:	
	rm *.o $(OUTPUT)