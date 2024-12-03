/// @file parser.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Defines the `Parser` class for managing and parsing command-line arguments.

#pragma once

#include "detail/tokenizer.hpp"
#include "detail/option.hpp"
#include "detail/option_list.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace cmdlp
{

/// @class Parser
/// @brief A class to define, parse, and manage command-line options.
class Parser {
public:
    /// @brief Constructs an `Parser` object.
    /// @param argc The number of command-line arguments.
    /// @param argv The array of command-line arguments.
    /// @details Initializes the tokenizer and the option list.
    Parser(int argc, char **argv)
        : tokenizer(argc, argv),
          options(),
          option_parsed(false)
    {
    }

    /// @brief Adds a value-based option to the parser.
    /// @tparam T The type of the option's default value.
    /// @param _opt_short The short version of the option (e.g., "-f").
    /// @param _opt_long The long version of the option (e.g., "--file").
    /// @param _description A description of the option, displayed in the help text.
    /// @param _value The default value for the option.
    /// @param _required Indicates whether the option is required.
    template <typename T>
    void addOption(const std::string &_opt_short,
                   const std::string &_opt_long,
                   const std::string &_description,
                   const T &_value,
                   bool _required)
    {
        // Turn the value to string.
        std::stringstream ss;
        ss << _value;
        // Create the option.
        auto option = new detail::ValueOption(_opt_short, _opt_long, _description, ss.str(), _required);
        // Add the option.
        options.addOption(option);
    }

    /// @brief Adds a toggle-based option to the parser.
    /// @param _opt_short The short version of the option (e.g., "-v").
    /// @param _opt_long The long version of the option (e.g., "--verbose").
    /// @param _description A description of the option, displayed in the help text.
    /// @param _toggled The default state of the toggle (true = enabled).
    void addToggle(const std::string &_opt_short,
                   const std::string &_opt_long,
                   const std::string &_description,
                   bool _toggled)
    {
        // Create the option.
        auto option = new detail::ToggleOption(_opt_short, _opt_long, _description, _toggled);
        // Add the option.
        options.addOption(option);
    }

    /// @brief Retrieves the value of an option.
    /// @tparam T The expected type of the option value.
    /// @param opt The short or long name of the option.
    /// @return The value of the option, or the default value of `T` if not found.
    template <typename T>
    inline T getOption(const std::string &opt) const
    {
        return options.getOption<T>(opt);
    }

    /// @brief Parses the registered options from the command-line arguments.
    /// @details Reads the command-line arguments and assigns values to the corresponding options.
    /// If a required option is missing, the program will print an error and exit.
    void parseOptions()
    {
        detail::ValueOption *vopt;
        detail::ToggleOption *topt;
        std::string value;
        for (detail::OptionList::const_iterator_t it = options.begin(); it != options.end(); ++it) {
            // Check if it is a value-holding option.
            if ((vopt = dynamic_cast<detail::ValueOption *>(*it))) {
                // Try to search for the option.
                value = tokenizer.getOption(vopt->opt_short);
                if (value.empty()) {
                    // Try to search for the option again using the long version.
                    value = tokenizer.getOption(vopt->opt_long);
                    // If the option is required but missing, print an error and exit.
                    if (value.empty()) {
                        if (vopt->required) {
                            std::cerr << "Cannot find required option: " << vopt->opt_long << "[" << vopt->opt_short << "]\n";
                            std::cerr << this->getHelp() << "\n";
                            std::exit(1);
                        } else {
                            continue;
                        }
                    } else {
                        std::cout << "Found value: " << value << ", for option: " << vopt->opt_short << "\n";
                    }
                } else {
                    std::cout << "Found value: " << value << ", for option: " << vopt->opt_short << "\n";
                }
                vopt->value = value;
                options.updateLongestValue(vopt->value.length());
            }
            // Check if it is a toggle option.
            else if ((topt = dynamic_cast<detail::ToggleOption *>(*it))) {
                if (tokenizer.hasOption(topt->opt_short) || tokenizer.hasOption(topt->opt_long)) {
                    topt->toggled = true;
                    std::cout << "Found option: " << topt->opt_short << "\n";
                }
            }
        }
        option_parsed = true;
    }

    /// @brief Generates a help string for all registered options.
    /// @return A string containing the help text for all options.
    /// @details Lists all options with their short and long names, default values, and descriptions.
    std::string getHelp() const
    {
        detail::ValueOption *vopt;
        detail::ToggleOption *topt;
        std::stringstream ss;
        for (detail::OptionList::const_iterator_t it = options.begin(); it != options.end(); ++it) {
            ss << "[" << std::setw(options.getLongestShortOption<int>()) << std::left << (*it)->opt_short << "] ";
            ss << std::setw(options.getLongestLongOption<int>()) << std::left << (*it)->opt_long;
            ss << " (" << std::setw(options.getLongestValue<int>()) << std::right;
            if ((vopt = dynamic_cast<detail::ValueOption *>(*it))) {
                ss << vopt->value;
            } else if ((topt = dynamic_cast<detail::ToggleOption *>(*it))) {
                ss << (topt->toggled ? "true" : "false");
            }
            ss << ") : ";
            ss << (*it)->description << "\n";
        }
        return ss.str();
    }

private:
    /// @brief Tokenizer for parsing command-line arguments.
    detail::Tokenizer tokenizer;
    /// @brief The list of registered options.
    detail::OptionList options;
    /// @brief Indicates whether options have been parsed.
    bool option_parsed;
};

} // namespace cmdlp