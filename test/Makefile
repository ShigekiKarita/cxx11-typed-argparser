.PHONY: test clean

CXXFLAGS := -std=c++11

test: test.out example_class.out example_simple.out
	./test.out
	./example_simple.out --str hello
	./example_class.out --batch_size 3 --use_cuda false --expdir "/home" --units 100 200
	./example_class.out --json '{"--batch_size": 3, "--use_cuda": false, "--expdir": "/home", "--units": [100, 200]}'

clean:
	rm -fv *.out *.o

%.out: %.cpp
	$(CXX) $< -o $@ $(CXXFLAGS) -Wall -Wextra -g3 -O0 -I../include -I../third_party/rapidjson/include -I.

test.out: test_main.o test_simple.o test_class.o test_traits.o
	$(CXX) $^ -o $@

%.o: %.cpp catch.hpp
	$(CXX) -c $< $(CXXFLAGS) -Wall -Wextra -g3 -O0 -I../include -I../third_party/rapidjson/include -I.

catch.hpp:
	wget https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp
