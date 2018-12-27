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
