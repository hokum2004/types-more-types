CXXFLAGS=-std=c++2b -Wall
CXX=g++

program=a.out

$(program): main.o
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $^

main.o: new_type.hpp util.hpp

.PHONY=clean fmt test

clean:
	rm -rf *.o $(program)
	$(MAKE) -f Makefile.test clean

fmt:
	clang-format -i *.cpp *.hpp

test:
	$(MAKE) -f Makefile.test test
