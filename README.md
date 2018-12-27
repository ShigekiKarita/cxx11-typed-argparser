# C++11 Typed Argument Parser

`#include <typed_argparser.hpp>` offers

- single header only library (except for optional rapidjson)
- json save/load (if you installed rapidjson)
- typed argument parsing
- python's argparse like API

## requirements

- C++11,14,17 supported compiler

| compiler | CI status                                                                                                          |
| -------- | -------------------------------------------------------------------------------------------------                  |
| gcc-7    | ![gcc-7](https://travis-matrix-badges.herokuapp.com/repos/ShigekiKarita/cxx11-typed-argparser/branches/master/1)   |
| gcc-8    | ![gcc-8](https://travis-matrix-badges.herokuapp.com/repos/ShigekiKarita/cxx11-typed-argparser/branches/master/2)   |
| clang-5  | ![clang-5](https://travis-matrix-badges.herokuapp.com/repos/ShigekiKarita/cxx11-typed-argparser/branches/master/3) |
| clang-6  | ![clang-6](https://travis-matrix-badges.herokuapp.com/repos/ShigekiKarita/cxx11-typed-argparser/branches/master/4) |
| clang-7  | ![clang-7](https://travis-matrix-badges.herokuapp.com/repos/ShigekiKarita/cxx11-typed-argparser/branches/master/5) |


## how to install

```
$ git clone https://github.com/ShigekiKarita/typed_argparser --recursive
$ export CPLUS_INCLUDE_PATH=`pwd`/typed_argparser/include:$CPLUS_INCLUDE_PATH
# if you do not have rapidjson
$ export CPLUS_INCLUDE_PATH=`pwd`/typed_argparser/third_party/rapidjson/include:$CPLUS_INCLUDE_PATH
$ g++ typed_argparser/test/example.cpp -std=c++11 # or -std=c++14, -std=c++17
$ ./example.out --help
```

see [test/example.cpp](test/example.cpp) for more details.

# limitations

For simplicity, this library only supports

- key should start with two dash: "--"
- arg should **not** start with two dash. you need to escape it `--str "\--foo"`
- value should be `bool`, integral (where `std::integral<T>::value == true`), `float`, `double`, `std::string`
- list value should be something like `std::vector` where `typed_argparse::is_container<T>::value == true`. (NOTE: std::vector, deque, list are tested but std::array<T, N> is not supported now)
- bool arg should be "true", "false" (`--flag true`) or nothing but key `--bool_flag --next_flag ...`

# usage

- simple usage (basic assign, json load/dump) [test/example_simple.cpp](test/example_simple.cpp)

``` c++
// $ prog.exe --bar 0.1 --str foo --vec 0 1 2 --use_cuda
#include <typed_argparser.hpp>
#include <iostream>

using typed_argparser::ArgParser;

int main(int argc, char* argv[]) {
    std::string json;
    std::string str = "foo";
    int foo = 2;
    double bar = 0;
    std::vector<int> vec = {1, 2, 3};
    bool use_cuda = false;

    ArgParser parser(argc, argv, "prog.exe: help for test");
    parser.add("--json", json, "json path or body");
    if (!json.empty()) {
        parser.from_json(json);
    }
    parser.require("--str", str); // .require value throws error when not provided
    parser.add("--foo", foo, ".add registers optional value");
    parser.add("--bar", bar, "double value");  // optional comment
    parser.add("--vec", vec, "multiple value support with std::vector/deque/list, etc");
    parser.add("--use_cuda", use_cuda, R"(bool value accepts "true" and nothing to store true, and "false" to store false.)");

    if (parser.help_wanted) {
        std::cout << parser.help_message() << std::endl;
        std::exit(0);
    }
    std::cout << parser.to_json() << std::endl;
}
```

this example prints auto-generated help message with type and default values as follows:

``` console
$ ./prog.exe --help
prog.exe: help for test
  --json
    type: std::string, default:
    json path or body

  --str (require)
    type: std::string, default:foo

  --foo
    type: int, default:2
    .add registers optional value

  --bar
    type: double, default:0
    double value

  --vec
    type: std::vector<int>, default:{1, 2, 3}
    multiple value support with std::vector/deque/list, etc

  --use_cuda
    type: bool, default:false
    bool value accepts "true" and nothing to store true, and "false" to store false.


```

you can load json by your `--json` option.

```
$ prog.exe --json '{"--batch_size":3,"--use_cuda":false,"--expdir":"/home","--units":[100,200]}'
{
    "--vec": [1, 2, 3],
    "--use_cuda": false,
    "--str": "foo",
    "--batch_size": "3",
    "--expdir": "/home",
    "--bar": 0.0,
    "--units": "100",
    "--foo": 2
}
```

- TODO: user-defined `assign(const Value& src, T& dst)` example with the class API

for more details, see [test/](test/)

## note

this library was first developed for https://github.com/ShigekiKarita/thxx
