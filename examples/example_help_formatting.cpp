#include <iostream>
#include <string>
#include <vector>

#include "cmdlp/parser.hpp"

int main(int argc, char *argv[])
{
    cmdlp::Parser parser(argc, argv);

    parser.addSeparator("Help Formatting Test:");
    parser.addOption(
        "-vln",
        "--very-long-option-name",
        "This is a very long description that should wrap nicely onto multiple lines "
        "and demonstrate the improved help formatting. It should align correctly "
        "after the option name and its value.",
        false,
        "default_value_is_also_quite_long");

    parser.addOption(
        "-s",
        "--short",
        "A short option description.",
        false,
        "short_val");

    // Add a help option for this example
    parser.addToggle("-h", "--help", "Shows this help for the program.", false);

    try {
        parser.parseOptions();

        // Application-level help handling
        if (parser.getOption<bool>("--help")) {
            std::cout << std::string(40, '=') << "\n";
            std::cout << parser.getHelp() << "\n";
            std::cout << std::string(40, '=') << "\n";
            return 0;
        }

        parser.validateOptions();

        std::cout << "Parsing successful, but this example is for help formatting.\n";

    } catch (const cmdlp::ParsingError &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    } catch (const cmdlp::BadConversion &e) {
        std::cerr << "Conversion Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
