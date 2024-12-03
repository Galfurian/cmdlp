#include <iostream>
#include <string>
#include <vector>

#include "cmdlp/parser.hpp"

int main(int argc, char *argv[])
{
    // Initializing the parser with simulated arguments
    cmdlp::Parser parser(argc, argv);

    // Adding options with default values and descriptions
    parser.addOption("-h", "--help", "Shows this help for the program.", false, false);
    parser.addOption("-db", "--double", "Double value", 0.2, false);
    parser.addOption("-i", "--int", "An integer value", -1, false);
    parser.addOption("-u", "--unsigned", "An unsigned value", 1, false);
    parser.addOption("-s", "--string", "A string.. actually, a single word", "hello", false);
    parser.addToggle("-v", "--verbose", "Enables verbose output", false);

    // Parse options
    parser.parseOptions();
    std::cout << parser.getHelp() << "\n";

    // Retrieve and display parsed options.
    std::cout << "Parsed options:\n";
    std::cout << "    Double    (-db) : " << parser.getOption<std::string>("-db") << "\n";
    std::cout << "    Integer   (-i)  : " << parser.getOption<std::string>("-i") << "\n";
    std::cout << "    Unsigned  (-u)  : " << parser.getOption<std::string>("-u") << "\n";
    std::cout << "    String    (-s)  : " << parser.getOption<std::string>("-s") << "\n";
    std::cout << "    Verbose   (-v)  : " << (parser.getOption<bool>("-v") ? "true" : "false") << "\n";

    return 0;
}
