# C++11 Typed Argument Parser

`#include <typed_argparser.hpp>` provides:

- single header only library (except for optional rapidjson)
- json save/load (if you installed rapidjson)
- typed arg registration
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

```console
$ git clone https://github.com/ShigekiKarita/typed_argparser --recursive
$ export CPLUS_INCLUDE_PATH=`pwd`/typed_argparser/include:$CPLUS_INCLUDE_PATH
# if you do not have rapidjson
$ export CPLUS_INCLUDE_PATH=`pwd`/typed_argparser/third_party/rapidjson/include:$CPLUS_INCLUDE_PATH
$ g++ your_file.cpp -std=c++11 # or -std=c++17
```

# usage

- simple usage

``` c++
// $ prog.exe --bar "0.1 --str foo --vec 0 1 2
#include <typed_argparser.hpp>

using typed_argparser::ArgParser;

int main(int argc, char* argv[]) {
    std::string str = "";
    int foo = 2;
    double bar = 0;
    std::vector<int> vec;

    ArgParser parser(argc, argv);
    parser.required("--str", str); // error when not provided
    parser.add("--foo", foo); // optional value
    parser.add("--bar", bar, "double value");  // optional comment
    parser.add("--vec", vec); // multiple value support with std::vector

    CHECK( foo == 2 );
    CHECK( bar == 0.1 );
    CHECK( str == "foo" );
    CHECK( vec == decltype(vec){0, 1, 2} );
}
```

- auto generated help message with type and default values

``` c++
// $ prog.exe --help
#include <typed_argparser.hpp>

using typed_argparser::ArgParser;

int main(int argc, char* argv[]) {
    std::string str = "foo";
    int foo = 2;
    double bar = 0;
    std::vector<int> vec = {1, 2, 3};

    ArgParser parser(argc, argv, "prog.exe: help for test");
    parser.required("--str", str); // error when not provided
    parser.add("--foo", foo); // optional value
    parser.add("--bar", bar, "double value");  // optional comment
    parser.add("--vec", vec); // multiple value support with std::vector

    if (parser.help_wanted) {
        std::cout << "R\"(" << parser.help_message() << ")\"" << std::endl;
    }
}
```

it prints as follows:

``` console
$ ./prog.exe --help
prog.exe: help for test
  --str (required)
    type: std::string, default:foo

  --foo
    type: int, default:2

  --bar
    type: double, default:0
    double value

  --vec
    type: std::vector<int>, default:{1, 2, 3}
```

- class based config. json save

``` c++
#include <typed_argparser.hpp>

using typed_argparser::ArgParser;

struct Opt : ArgParser {
    std::int64_t batch_size = 32;
    std::vector<std::int64_t> units = {1, 2, 3};
    bool use_cuda = true;
    std::string expdir = "";
    std::string json = "";

    Opt(int argc, const char* const argv[]) : ArgParser(argc, argv) {
        add("--json", json);
        if (!json.empty()) {
            from_json(json);
        }
        required("--batch_size", batch_size);
        add("--use_cuda", use_cuda);
        add("--units", units);
        add("--expdir", expdir);
        check();
    }
};

int main(int argc, char* argv[]) {
    const char* argv[] = {"prog.exe", "--batch_size", "4", "--units", "100", "200", "300"};
    static_assert(asizeof(argv) == 7);
    Opt opt(asizeof(argv), (char**) argv);

    CHECK( opt.batch_size == 4 );
    CHECK( opt.units == decltype(opt.units){100, 200, 300} );
    CHECK( opt.use_cuda );
    // if you pass true or nothing, output json is pretty formatted.
    CHECK( opt.to_json(false) == R"({"--use_cuda":true,"--json":"","--expdir":"","--batch_size":4,"--units":[100,200,300]})" );
}
```

- json load

``` c++
// $ prog.exe --json '{"--batch_size":3,"--use_cuda":false,"--expdir":"/home","--units":[100,200]}'
/// struct Opt ...

int main(int argc, char* argv[]) {
    Opt opt(asizeof(argv), argv);
    CHECK( opt.batch_size == 3 );
    CHECK( opt.units == decltype(opt.units){100, 200} );
    CHECK( opt.use_cuda == false );
    CHECK( opt.expdir == "/home" );
}
```


for more details, see [test/test.cpp](test/test.cpp)
