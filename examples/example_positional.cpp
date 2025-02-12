#include <iostream>
#include <string>
#include <vector>

#include "cmdlp/parser.hpp"

int main(int argc, char *argv[])
{
    // Initializing the parser with simulated arguments
    cmdlp::Parser parser(argc, argv);

    parser.addSeparator("Positional options:");
    parser.addPositionalOption("-i", "--input", "Input file.", true);
    parser.addPositionalOption("-o", "--output", "Output file.", true);
    parser.addPositionalList("-f", "--files", "List of extra files.", false);

    // Parse options.
    parser.parseOptions();
    std::cout << std::string(40, '=') << "\n";
    std::cout << parser.getHelp();
    std::cout << std::string(40, '=') << "\n";

    // Retrieve and display parsed positional options.
    std::cout << "Parsed positional options:\n";
    std::cout << " Input  (-i) : " << parser.getOption<std::string>("--input") << "\n";
    std::cout << " Output (-o) : " << parser.getOption<std::string>("--output") << "\n";
    const auto &extra_files = parser.getOption<cmdlp::Values>("--files");
    std::cout << " Extra  (-f) : ";
    for (const auto &file : extra_files) {
        std::cout << file << " ";
    }
    std::cout << "\n";

    return 0;
}
