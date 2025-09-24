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

    // Add a help option for this example
    parser.addToggle("-h", "--help", "Shows this help for the program.", false);

    try {
        // Parse options. This will not throw for missing required options.
        parser.parseOptions();

        // Application-level help handling
        if (parser.getOption<bool>("--help")) {
            std::cout << std::string(40, '=') << "\n";
            std::cout << parser.getHelp();
            std::cout << std::string(40, '=') << "\n";
            return 0; // Exit after showing help
        }

        // Validate options. This will throw if required options are missing.
        parser.validateOptions();

        std::cout << std::string(40, '=') << "\n";
        std::cout << parser.getHelp();
        std::cout << std::string(40, '=') << "\n";

    } catch (const cmdlp::ParsingError &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1; // Exit with error code
    } catch (const cmdlp::BadConversion &e) {
        std::cerr << "Conversion Error: " << e.what() << "\n";
        return 1; // Exit with error code
    }

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
