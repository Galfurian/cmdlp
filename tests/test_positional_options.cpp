#include "cmdlp/parser.hpp"
#include <iostream>
#include <vector>

// General comparison template for non-floating types
template <typename T1, typename T2>
bool test_option(const cmdlp::Parser &parser, const T1 &opt, const T2 &value)
{
    if (opt != value) {
        std::cerr << "FAIL: Option value `" << opt << "` does not match expected `" << value << "`\n";
        std::cerr << parser.getHelp() << "\n";
        return false;
    }
    return true;
}

int main(int, char *[])
{
    std::vector<std::string> arguments = {
        "test_positional_options",
        "input.txt",
        "config.txt",
        "file1.txt",  "file2.txt",
    };

    cmdlp::Parser parser(arguments);

    // Adding positional options
    parser.addPositionalOption("-in", "--input", "Input file", true);
    parser.addPositionalOption("-cfg", "--config", "Configuration file", true);
    parser.addPositionalList("-f", "--files", "List of input files", false);

    // Parsing options
    parser.parseOptions();
    // Validate options (should not throw for this test as all required positional options are provided)
    parser.validateOptions();

    // Testing positional options
    if (!test_option(parser, parser.getOption<std::string>("--input"), "input.txt"))
        return 1;
    if (!test_option(parser, parser.getOption<std::string>("--config"), "config.txt"))
        return 1;
    auto files = parser.getOption<std::vector<std::string>>("--files");
    if (!test_option(parser, files.size(), 2UL))
        return 1;
    if (!test_option(parser, files[0], "file1.txt"))
        return 1;
    if (!test_option(parser, files[1], "file2.txt"))
        return 1;

    std::cout << "All positional options tests passed successfully!\n";
    return 0;
}
