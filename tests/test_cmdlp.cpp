#include "cmdlp/parser.hpp"
#include <cmath>
#include <complex>
#include <iostream>
#include <type_traits>
#include <vector>

// Floating-point comparison template
template <typename T1, typename T2>
typename std::enable_if<std::is_floating_point<T1>::value || std::is_floating_point<T2>::value, bool>::type
test_option(const cmdlp::Parser &parser, const T1 &opt, const T2 &value)
{
    if (std::abs(opt - value) > 1e-09) {
        std::cerr << "FAIL: Option value `" << opt << "` does not match expected `" << value << "`\n";
        std::cerr << parser.getHelp() << "\n";
        return false;
    }
    return true;
}

// General comparison template for non-floating types
template <typename T1, typename T2>
typename std::enable_if<!std::is_floating_point<T1>::value && !std::is_floating_point<T2>::value, bool>::type
test_option(const cmdlp::Parser &parser, const T1 &opt, const T2 &value)
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
        "test_cmdlp",                 //
        "--double",   "0.00006456",   //
        "--int",      "-42",          //
        "-u",         "17",           //
        "-s",         "Hello there!", //
        "-c",         "(3,4)",        //
        "--verbose",                  //
        "--mode",     "auto",         //
        "input.txt",                  //
        "config.txt",                 //
        "file1.txt",  "file2.txt",
    };

    cmdlp::Parser parser(arguments);

    // Adding different types of options
    parser.addOption("-d", "--double", "Double value", false);
    parser.addOption("-i", "--int", "An integer value", false);
    parser.addOption("-u", "--unsigned", "An unsigned value", false);
    parser.addOption("-s", "--string", "A string", false);
    parser.addOption("-c", "--complex", "A complex number", false);
    parser.addToggle("-v", "--verbose", "Enables verbose output", false);
    parser.addMultiOption("-m", "--mode", "Select mode", {"auto", "manual", "test"}, "manual");

    // Adding positional options
    parser.addPositionalOption("-in", "--input", "Input file", true);
    parser.addPositionalOption("-cfg", "--config", "Configuration file", true);
    parser.addPositionalList("-f", "--files", "List of input files", false);

    // Parsing options
    parser.parseOptions();

    // Testing parsed options
    if (!test_option(parser, parser.getOption<double>("--double"), 0.00006456))
        return 1;
    if (!test_option(parser, parser.getOption<int>("--int"), -42))
        return 1;
    if (!test_option(parser, parser.getOption<unsigned>("--unsigned"), 17U))
        return 1;
    if (!test_option(parser, parser.getOption<std::string>("--string"), "Hello there!"))
        return 1;

    // Parsing complex number from string
    std::complex<double> expected_complex(3.0, 4.0);
    std::istringstream ss(parser.getOption<std::string>("--complex"));
    std::complex<double> parsed_complex;
    ss >> parsed_complex;
    if (!test_option(parser, parsed_complex, expected_complex))
        return 1;

    if (!test_option(parser, parser.getOption<bool>("--verbose"), true))
        return 1;

    // Test MultiOption default
    if (!test_option(parser, parser.getOption<std::string>("--mode"), "auto"))
        return 1;

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

    std::cout << "All tests passed successfully!\n";
    return 0;
}
