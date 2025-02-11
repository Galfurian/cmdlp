#include <iostream>
#include <string>
#include <vector>

#include "cmdlp/parser.hpp"

int main(int argc, char *argv[])
{
    // Initializing the parser with simulated arguments
    cmdlp::Parser parser(argc, argv);

    // Adding options with default values and descriptions
    parser.addSeparator("Normal options:");
    parser.addOption("-db", "--double", "Double value", false, 0.2);
    parser.addOption("-i", "--int", "An integer value", false, -1);
    parser.addOption("-u", "--unsigned", "An unsigned value", false, 1);
    parser.addOption("-s", "--string", "A string.", true);
    parser.addOption("-out", "--output", "Output file.", false, "output.txt");

    parser.addSeparator("Toggle options:");
    parser.addToggle("-h", "--help", "Shows this help for the program.", false);
    parser.addToggle("-v", "--verbose", "Enables verbose output", false);

    parser.addSeparator("Multi options:");
    parser.addMultiOption("-m", "--mode", "Select the operation mode.", {"auto", "manual", "test"}, "auto");
    parser.addMultiOption("-id", "--index", "Select the index.", {"0", "1"}, "1");

    parser.addSeparator("Positional options:");
    parser.addPositionalOption("-in", "--input", "Input file.", true, "input.txt");

    // Parse options.
    parser.parseOptions();
    std::cout << parser.getHelp() << "\n";

    // Retrieve and display parsed options.
    std::cout << "Parsed options:\n";
    std::cout << "    Double    (-db) : " << parser.getOption<double>("--double") << "\n";
    std::cout << "    Integer   (-i)  : " << parser.getOption<int>("--int") << "\n";
    std::cout << "    Unsigned  (-u)  : " << parser.getOption<unsigned>("--unsigned") << "\n";
    std::cout << "    String    (-s)  : " << parser.getOption<std::string>("--string") << "\n\n";

    std::cout << "    Help      (-h)  : " << parser.getOption<bool>("--help") << "\n";
    std::cout << "    Verbose   (-v)  : " << parser.getOption<bool>("--verbose") << "\n\n";

    std::cout << "    Mode      (-m)  : " << parser.getOption<std::string>("--mode") << "\n";
    std::cout << "    Index     (-id) : " << parser.getOption<std::string>("--index") << "\n";

    std::cout << "    Input      (-in)  : " << parser.getOption<std::string>("--input") << "\n";
    std::cout << "    Output     (-out) : " << parser.getOption<std::string>("--output") << "\n";

    return 0;
}
