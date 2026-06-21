CXX := clang++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude

SRC := src/socket.cc
TEST_SRCS := $(wildcard tests/test_*.cc)
TEST_BINS := $(patsubst tests/%.cc, build/%, $(TEST_SRCS))

.PHONY: all test run-test clean

all: test

test: $(TEST_BINS)

build/%: tests/%.cc $(SRC) include/server/alsocket.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(SRC) $< -o $@

run-test: $(TEST_BINS)
	@for t in $(TEST_BINS); do echo "Running $$t"; ./$$t; done

clean:
	rm -rf build
