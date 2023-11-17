#include "cmdlp/option_parser.hpp"

void test_copy(cmdlp::OptionParser parser)
{
    std::cout << "Usage :\n"
              << parser.getHelp() << "\n";
    std::cout << "-d : " << parser.getOption<double>('d') << "\n";
    std::cout << "-i : " << parser.getOption<int>('i') << "\n";
    std::cout << "-s : " << parser.getOption<std::string>('s') << "\n";
    std::cout << "-v : " << parser.getOption<bool>('v') << "\n";
}

int main(int, char *[])
{
    std::vector<char *> arguments;
    arguments.push_back((char *)"test_cmdlp");
    arguments.push_back((char *)"--double");
    arguments.push_back((char *)"0.00006456");

    arguments.push_back((char *)"--int");
    arguments.push_back((char *)"42");

    arguments.push_back((char *)"-s");
    arguments.push_back((char *)"Hello");

    arguments.push_back((char *)"--verb");

    arguments.push_back((char *)"--verbose");

    cmdlp::OptionParser parser(static_cast<int>(arguments.size() - 1), arguments.data());
    parser.addOption('d', "--double", "Double value", 0.2);
    parser.addOption('i', "--int", "An integer value", 1);
    parser.addOption('s', "--string", "A string.. actually, a single word", "hello");
    parser.addToggle('v', "--verbose", "Enables verbose output");
    parser.parseOptions();

    test_copy(parser);
    
    return 0;
}
