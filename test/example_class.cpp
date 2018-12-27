#include <typed_argparser.hpp>
#include <iostream>

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

        add("--batch_size", batch_size, "batch size for training");
        add("--use_cuda", use_cuda, "flag to enable cuda");
        add("--units", units, "number of units");
        add("--expdir", expdir, "experiment directory");
        if (help_wanted) {
            std::cout << help_message() << std::endl;
            std::exit(0);
        }
        check();
    }
};

int main(int argc, char *argv[]) {
    Opt opt(argc, argv);
    std::cout << opt.to_json() << std::endl;
}
