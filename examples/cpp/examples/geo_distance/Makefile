CXXFLAGS=-std=c++2b -Wall
CXX=g++

program=a.out

$(program): main.o
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $^

main.o: new_type.hpp Makefile

.PHONY=clean fmt

clean:
	rm -rf *.o $(program)

fmt:
	clang-format -i *.cpp *.hpp
