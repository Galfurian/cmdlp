#include <iostream>
#include <string>
#include <vector>

#include "cmdlp/parser.hpp"

int main(int argc, char *argv[])
{
    // Initializing the parser with simulated arguments
    cmdlp::Parser parser(argc, argv);

    parser.addSeparator("Positional options:");
    parser.addPositionalOption("-in", "--input", "Input file.", true);
    parser.addPositionalList("-files", "--files", "List of extra files.", false);

    // Parse options.
    parser.parseOptions();
    std::cout << parser.getHelp() << "\n";

    // Retrieve and display parsed positional options.
    std::cout << "Parsed positional options:\n";
    std::cout << "    Input (-in)    : " << parser.getOption<std::string>("--input") << "\n";
    const auto &extra_files = parser.getOption<cmdlp::Values>("--files");
    std::cout << "    Extra (-files) : ";
    for (const auto &file : extra_files) {
        std::cout << file << " ";
    }
    std::cout << "\n";

    return 0;
}
