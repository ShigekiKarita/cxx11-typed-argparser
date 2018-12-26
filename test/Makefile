.PHONY: test clean

CXXFLAGS := -std=c++11

test: test.out example.out
	./test.out
	./example.out --batch_size 3 --use_cuda false --expdir "/home" --units 100 200
	./example.out --json '{"--batch_size": 3, "--use_cuda": false, "--expdir": "/home", "--units": [100, 200]}'

clean:
	rm -v *.out

%.out: %.cpp ../include/typed_argparser.hpp catch.hpp
	$(CXX) $< -o $@ $(CXXFLAGS) -Wall -Wextra -g3 -O0 -I../include -I../third_party/rapidjson/include -I.

catch.hpp:
	wget https://raw.githubusercontent.com/catchorg/Catch2/master/single_include/catch2/catch.hpp
