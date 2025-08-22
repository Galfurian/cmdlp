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
        "test_toggle_options",
        "--verbose",
    };

    cmdlp::Parser parser(arguments);

    // Adding toggle options
    parser.addToggle("-v", "--verbose", "Enables verbose output", false);
    parser.addToggle("-q", "--quiet", "Disables verbose output", false);

    // Parsing options
    parser.parseOptions();
    // Validate options (should not throw for this test as no required options are missing)
    parser.validateOptions();

    // Testing parsed options
    if (!test_option(parser, parser.getOption<bool>("--verbose"), true))
        return 1;
    if (!test_option(parser, parser.getOption<bool>("--quiet"), false)) // Should be false as it's not in arguments
        return 1;

    std::cout << "All toggle options tests passed successfully!\n";
    return 0;
}
