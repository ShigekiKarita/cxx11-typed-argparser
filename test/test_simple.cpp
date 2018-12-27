#include <typed_argparser.hpp>

#include <iostream>
#include <list>
#include <deque>
#include <vector>

#include <catch.hpp>

using namespace typed_argparser;

TEST_CASE( "parse", "[simple]" ) {
    std::string str = "";
    int foo = 2;
    double bar = 0;

    std::vector<int> vec;
    std::deque<int> deq;
    std::list<int> lis;
    std::vector<bool> bools;

    const char* argv[] = {
        "prog.exe", "--bar", "0.1", "--str", "foo",
        "--vec", "0", "1", "2",
        "--deq", "0", "1", "2",
        "--lis", "0", "1", "2",
        "--bools", "true", "false", "true"
    };
    int argc = asizeof(argv);
    ArgParser parser(argc, argv);
    parser.require("--str", str); // error when not provided
    parser.add("--foo", foo); // optional value
    parser.add("--bar", bar, "double value");  // optional comment
    parser.add("--vec", vec); // multiple value support with std::vector
    parser.add("--deq", deq);
    parser.add("--lis", lis);
    parser.add("--bools", bools);

    CHECK( foo == 2 );
    CHECK( bar == 0.1 );
    CHECK( str == "foo" );
    CHECK( vec == decltype(vec){0, 1, 2} );
    CHECK( deq == decltype(deq){0, 1, 2} );
    CHECK( lis == decltype(lis){0, 1, 2} );
    CHECK( bools == decltype(bools){true, false, true} );
}

TEST_CASE( "help", "[simple]" ) {
    std::string str = "foo";
    int foo = 2;
    double bar = 0;
    std::vector<int> vec = {1, 2, 3};

    const char* argv[] = {"prog.exe", "--help"};
    int argc = asizeof(argv);
    ArgParser parser(argc, argv, "prog.exe: help for test");
    parser.require("--str", str); // error when not provided
    parser.add("--foo", foo); // optional value
    parser.add("--bar", bar, "double value");  // optional comment
    parser.add("--vec", vec); // multiple value support with std::vector

    auto help = R"(prog.exe: help for test
  --str (require)
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
