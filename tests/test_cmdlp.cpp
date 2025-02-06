#include "cmdlp/parser.hpp"

#include <type_traits>

// Floating-point comparison
template <typename T1, typename T2>
typename std::enable_if<std::is_floating_point<T1>::value || std::is_floating_point<T2>::value, int>::type
test_option(const cmdlp::Parser &parser, const T1 &opt, const T2 &value)
{
    if (std::abs(opt - value) > 1e-09) {
        std::cerr << "The option `" << opt << "` is different than `" << value << "`\n";
        std::cerr << parser.getHelp() << "\n";
        return 1;
    }
    return 0; // Test passed
}

// General comparison for other types
template <typename T1, typename T2>
typename std::enable_if<!std::is_floating_point<T1>::value && !std::is_floating_point<T2>::value, int>::type
test_option(const cmdlp::Parser &parser, const T1 &opt, const T2 &value)
{
    if (opt != value) {
        std::cerr << "The option `" << opt << "` is different than `" << value << "`\n";
        std::cerr << parser.getHelp() << "\n";
        return 1;
    }
    return 0; // Test passed
}

int main(int, char *[])
{
    std::vector<const char *> arguments = {
        "test_cmdlp", "--double", "0.00006456", "--int", "-42", "-u", "17", "-s", "Hello", "--verbose",
    };

    cmdlp::Parser parser(static_cast<int>(arguments.size()), const_cast<char **>(arguments.data()));
    parser.addOption("-h", "--help", "Shows this help for the program.", false, false);
    parser.addOption("-d", "--double", "Double value", 0.2, false);
    parser.addOption("-i", "--int", "An integer value", -1, false);
    parser.addOption("-u", "--unsigned", "An unsigned value", 1, false);
    parser.addOption("-s", "--string", "A string.. actually, a single word", "hello", false);
    parser.addToggle("-v", "--verbose", "Enables verbose output", false);
    parser.parseOptions();

    if (test_option(parser, parser.getOption<double>("-d"), 0.00006456))
        return 1;
    if (test_option(parser, parser.getOption<int>("-i"), -42))
        return 1;
    if (test_option(parser, parser.getOption<int>("-u"), 17))
        return 1;
    if (test_option(parser, parser.getOption<std::string>("-s"), "Hello"))
        return 1;
    if (test_option(parser, parser.getOption<bool>("-v"), true))
        return 1;

    return 0;
}
