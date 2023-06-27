CXXFLAGS=-std=c++2b -Wall
CXX=g++

program=a.out

objects=main.o util/semigroup_string.o multiple_errors.o

$(program): $(objects)
	$(CXX) -o $@ $(CXXFLAGS) $(LDFLAGS) $^

main.o: new_type.hpp util/ostream_printers.hpp multiple_errors.hpp util/expected_mapn.hpp

multiple_errors.o: multiple_errors.hpp

.PHONY=clean fmt test

clean:
	rm -rf *.o $(program)
	$(MAKE) -f Makefile.test clean

fmt:
	clang-format -i *.cpp *.hpp

test:
	$(MAKE) -C util -f Makefile.test test
