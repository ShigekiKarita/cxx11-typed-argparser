#include <catch.hpp>
#include <iostream>
#include <typed_argparser.hpp>

using namespace typed_argparser;

// TODO test user-defined assign
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
