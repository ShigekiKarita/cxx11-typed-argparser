.PHONY: test

CXXFLAGS := -std=c++11

test: test.out
	./test.out

test.out: test.cpp ../include/typed_argparser.hpp catch.hpp
	$(CXX) $< -o $@ $(CXXFLAGS) -Wall -Wextra -g3 -O0 -I../include -I../third_party/rapidjson/include -I.

catch.hpp:
	wget https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp
