#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch.hpp>

#include <iostream>
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

template <typename T, size_t N>
constexpr size_t asizeof(T(&)[N]) { return N; }

TEST_CASE( "to_json", "[class]" ) {
    const char* argv[] = {"prog.exe", "--batch_size", "4", "--units", "100", "200", "300"};
    static_assert(asizeof(argv) == 7, "");
    Opt opt(asizeof(argv), (char**) argv);

    CHECK( opt.batch_size == 4 );
    CHECK( opt.units == decltype(opt.units){100, 200, 300} );
    CHECK( opt.use_cuda );
    CHECK( opt.to_json(false) == R"({"--use_cuda":true,"--json":"","--expdir":"","--batch_size":4,"--units":[100,200,300]})" );
}

TEST_CASE( "from_json string", "[class]" ) {
    const char* argv[] = {
        "prog.exe", "--json",
        R"({"--batch_size":3,"--use_cuda":false,"--expdir":"/home","--units":[100,200]})"
    };
    Opt opt(asizeof(argv), argv);
    CHECK( opt.batch_size == 3 );
    CHECK( opt.units == decltype(opt.units){100, 200} );
    CHECK( opt.use_cuda == false );
    CHECK( opt.expdir == "/home" );
}

TEST_CASE( "from_json file", "[class]" ) {
    const char* argv[] = {
        "prog.exe", "--json", "test.json"
    };
    Opt opt(asizeof(argv), argv);
    CHECK( opt.batch_size == 3 );
    CHECK( opt.units == decltype(opt.units){100, 200} );
    CHECK( opt.use_cuda == false );
    CHECK( opt.expdir == "/home" );
}

TEST_CASE( "parse", "[non-class]" ) {
    std::string str = "";
    int foo = 2;
    double bar = 0;
    std::vector<int> vec;

    const char* argv[] = {"prog.exe", "--bar", "0.1", "--str", "foo", "--vec", "0", "1", "2"};
    int argc = asizeof(argv);
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

TEST_CASE( "help", "[non-class]" ) {
    std::string str = "foo";
    int foo = 2;
    double bar = 0;
    std::vector<int> vec = {1, 2, 3};

    const char* argv[] = {"prog.exe", "--help"};
    int argc = asizeof(argv);
    ArgParser parser(argc, argv, "prog.exe: help for test");
    parser.required("--str", str); // error when not provided
    parser.add("--foo", foo); // optional value
    parser.add("--bar", bar, "double value");  // optional comment
    parser.add("--vec", vec); // multiple value support with std::vector

    auto help = R"(prog.exe: help for test
  --str (required)
    type: std::string, default:foo

  --foo
    type: int, default:2

  --bar
    type: double, default:0
    double value

  --vec
    type: std::vector<int>, default:{1, 2, 3}

)";
    CHECK( parser.help_message() == help );
    // if (parser.help_wanted) {
    //     std::cout << "R\"(" << parser.help_message() << ")\"" << std::endl;
    // }
}
