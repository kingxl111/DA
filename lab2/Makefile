CXX = g++
CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra -Werror

all: main lab2_test lab2_profiler

main: main.cpp B-tree.hpp
	$(CXX) $(CXXFLAGS) main.cpp -o main

lab2_test: test.cpp B-tree.hpp
	$(CXX) $(CXXFLAGS) test.cpp -o lab2_test

lab2_profiler: main.cpp
	$(CXX) $(CXXFLAGS) -pg main.cpp -o lab2_profiler

test:
	./lab2_test

clean:
	rm -f main lab2_profiler lab2_test *.o