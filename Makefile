CXXFLAGS += -std=c++20 -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror
CXXFLAGS += -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer
LINK.o = ${LINK.cc}

.PHONY: all check clean

all: check

check: tests/tests
	./tests/tests

tests/tests: tests/tests.o tests/test_array.o

tests/test_array.o: tests/test_array.cpp include/calculisto/array/array.hpp

clean:
	rm -f tests/tests tests/*.o
