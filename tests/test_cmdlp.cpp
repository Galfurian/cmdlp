#include "cmdlp/option_parser.hpp"

#define TEST_OPTION(OPT, VALUE)                                                          \
    if (OPT != VALUE) {                                                                  \
        std::cerr << "The option `" << OPT << "` is different than `" << VALUE << "`\n"; \
        return 1;                                                                        \
    }

int main(int, char *[])
{
    std::vector<const char *> arguments;
    arguments.push_back("test_cmdlp");
    arguments.push_back("--double");
    arguments.push_back("0.00006456");
    arguments.push_back("--int");
    arguments.push_back("-42");
    arguments.push_back("-u");
    arguments.push_back("17");
    arguments.push_back("-s");
    arguments.push_back("Hello");
    arguments.push_back("--verbose");

    cmdlp::OptionParser parser(static_cast<int>(arguments.size()), arguments.data());
    parser.addOption("-h", "--help", "Shows this help for the program.", false, false);
    parser.addOption("-d", "--double", "Double value", 0.2, false);
    parser.addOption("-i", "--int", "An integer value", -1, false);
    parser.addOption("-u", "--unsigned", "An unsigned value", 1, false);
    parser.addOption("-s", "--string", "A string.. actually, a single word", "hello", false);
    parser.addToggle("-v", "--verbose", "Enables verbose output", false);
    parser.parseOptions();

    TEST_OPTION(parser.getOption<double>("-d"), 0.00006456);
    TEST_OPTION(parser.getOption<int>("-i"), -42);
    TEST_OPTION(parser.getOption<int>("-u"), 17);
    TEST_OPTION(parser.getOption<std::string>("-s"), "Hello");
    TEST_OPTION(parser.getOption<bool>("-v"), true);

    return 0;
}
