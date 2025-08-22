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
        "test_multi_options",
        "--mode",
        "auto",
    };

    cmdlp::Parser parser(arguments);

    // Adding multi options
    parser.addMultiOption("-m", "--mode", "Select mode", {"auto", "manual", "test"}, "manual");
    parser.addMultiOption("-c", "--color", "Select color", {"red", "green", "blue"}, "blue");

    // Parsing options
    parser.parseOptions();

    // Testing parsed options
    if (!test_option(parser, parser.getOption<std::string>("--mode"), "auto"))
        return 1;
    if (!test_option(parser, parser.getOption<std::string>("--color"), "blue"))
        return 1;

    // Test invalid multi option value (should throw std::invalid_argument during addMultiOption or parseOptions)
    // This test case is for demonstrating, actual error handling for invalid values
    // is done during addMultiOption or parseOptions.
    // For this test, we'll simulate an invalid argument to ensure it's caught.
    try {
        std::vector<std::string> invalid_args = {"test_multi_options", "--mode", "INVALID"};
        cmdlp::Parser invalid_parser(invalid_args);
        invalid_parser.addMultiOption("-m", "--mode", "Select mode", {"auto", "manual", "test"}, "manual");
        invalid_parser.parseOptions();
        std::cerr << "FAIL: Invalid multi option value was not caught!\n";
        std::cerr << invalid_parser.getHelp() << "\n";
        return 1;
    } catch (const std::invalid_argument &e) {
        std::cout << "Caught expected exception for invalid multi option: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "FAIL: Caught unexpected exception type for invalid multi option.\n";
        return 1;
    }

    std::cout << "All multi options tests passed successfully!\n";
    return 0;
}
