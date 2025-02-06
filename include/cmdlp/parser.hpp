/// @file parser.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Defines the `Parser` class for managing and parsing command-line arguments.

#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>

#include "detail/option.hpp"
#include "detail/option_list.hpp"
#include "detail/tokenizer.hpp"

enum : unsigned char {
    CMDLP_MAJOR_VERSION = 1, ///< Major version of the library.
    CMDLP_MINOR_VERSION = 1, ///< Minor version of the library.
    CMDLP_MICRO_VERSION = 0  ///< Micro version of the library.
};

namespace cmdlp
{

/// @class Parser
/// @brief A class to define, parse, and manage command-line options.
class Parser
{
public:
    /// @brief Constructs an `Parser` object.
    /// @param argc The number of command-line arguments.
    /// @param argv The array of command-line arguments.
    /// @details Initializes the tokenizer and the option list.
    Parser(int argc, char **argv)
        : tokenizer(argc, argv)

    {
    }

    /// @brief Adds a multi-value option to the parser.
    /// @param _opt_short The short version of the option (e.g., "-m").
    /// @param _opt_long The long version of the option (e.g., "--mode").
    /// @param _description A description of the option, displayed in the help text.
    /// @param _allowed_values The set of predefined values for the option.
    /// @param _default_value The default value for the option.
    /// @throws std::invalid_argument if the default value is not in the list of allowed values.
    void addMultiOption(
        const std::string &_opt_short,
        const std::string &_opt_long,
        const std::string &_description,
        const std::vector<std::string> &_allowed_values,
        const std::string &_default_value)
    {
        // Add the option to the list.
        options.addOption(std::make_shared<detail::MultiOption>(
            _opt_short, _opt_long, _description, _allowed_values, _default_value));
    }

    /// @brief Adds a value-based option to the parser.
    /// @tparam T The type of the option's default value.
    /// @param _opt_short The short version of the option (e.g., "-f").
    /// @param _opt_long The long version of the option (e.g., "--file").
    /// @param _description A description of the option, displayed in the help text.
    /// @param _value The default value for the option.
    /// @param _required Indicates whether the option is required.
    template <typename T>
    void addOption(
        const std::string &_opt_short,
        const std::string &_opt_long,
        const std::string &_description,
        const T &_value,
        bool _required)
    {
        // Turn the value to string.
        std::stringstream ss;
        ss << _value;
        // Add the option to the list.
        options.addOption(
            std::make_shared<detail::ValueOption>(_opt_short, _opt_long, _description, ss.str(), _required));
    }

    /// @brief Adds a toggle-based option to the parser.
    /// @param _opt_short The short version of the option (e.g., "-v").
    /// @param _opt_long The long version of the option (e.g., "--verbose").
    /// @param _description A description of the option, displayed in the help text.
    /// @param _toggled The default state of the toggle (true = enabled).
    void addToggle(
        const std::string &_opt_short,
        const std::string &_opt_long,
        const std::string &_description,
        bool _toggled)
    {
        // Add the option to the list.
        options.addOption(std::make_shared<detail::ToggleOption>(_opt_short, _opt_long, _description, _toggled));
    }

    /// @brief Adds a positional option to the parser.
    /// @param _opt_short The short version of the option (e.g., "-f").
    /// @param _opt_long The long version of the option (e.g., "--file").
    /// @param _default_value The default value for the option.
    /// @param _description A description of the option.
    /// @param _required Whether the option is required.
    void addPositionalOption(
        const std::string &_opt_short,
        const std::string &_opt_long,
        const std::string &_default_value,
        const std::string &_description,
        bool _required)
    {
        options.addOption(
            std::make_shared<detail::PositionalOption>(_opt_short, _opt_long, _default_value, _description, _required));
    }

    /// @brief Adds a separator for grouping options in the help message.
    /// @param _description The description of the separator (e.g., section title).
    void addSeparator(const std::string &_description)
    {
        // Add the option to the list.
        options.addOption(std::make_shared<detail::Separator>(_description));
    }

    /// @brief Retrieves the value of an option.
    /// @tparam T The expected type of the option value.
    /// @param opt The short or long name of the option.
    /// @return The value of the option, or the default value of `T` if not found.
    template <typename T>
    auto getOption(const std::string &opt) const -> T
    {
        return options.getOption<T>(opt);
    }

    /// @brief Parses the registered options from the command-line arguments.
    /// @details Reads the command-line arguments and assigns values to the corresponding options.
    /// If a required option is missing, the program will print an error and exit.
    void parseOptions()
    {
        size_t pos_arg_index = 0;

        for (const auto &option : options) {
            if (auto vopt = std::dynamic_pointer_cast<detail::ValueOption>(option)) {
                this->parseValueOption(vopt);
            } else if (auto mopt = std::dynamic_pointer_cast<detail::MultiOption>(option)) {
                this->parseMultiOption(mopt);
            } else if (auto topt = std::dynamic_pointer_cast<detail::ToggleOption>(option)) {
                this->parseToggleOption(topt);
            } else if (auto posopt = std::dynamic_pointer_cast<detail::PositionalOption>(option)) {
                if (tokenizer.hasPositionalArgument(pos_arg_index)) {
                    posopt->value = tokenizer.getPositionalArgument(pos_arg_index++);
                } else if (posopt->required) {
                    std::cerr << "Missing required positional argument: " << posopt->description << "\n";
                    std::cerr << this->getHelp() << "\n";
                    std::exit(1);
                }
            }
        }

        option_parsed = true;
    }

    /// @brief Generates a help string for all registered options.
    /// @return A string containing the help text for all options.
    /// @details Lists all options with their short and long names, default values, and descriptions.
    auto getHelp() const -> std::string
    {
        std::stringstream ss;
        for (const auto &option : options) {
            auto sepr = std::dynamic_pointer_cast<detail::Separator>(option);
            if (sepr) {
                ss << "\n" << sepr->description << "\n";
            } else {
                auto vopt = std::dynamic_pointer_cast<detail::ValueOption>(option);
                auto topt = std::dynamic_pointer_cast<detail::ToggleOption>(option);
                auto mopt = std::dynamic_pointer_cast<detail::MultiOption>(option);
                auto popt = std::dynamic_pointer_cast<detail::PositionalOption>(option);
                ss << "[" << std::setw(options.getLongestShortOption<int>()) << std::left << option->opt_short << "] ";
                ss << std::setw(options.getLongestLongOption<int>()) << std::left << option->opt_long;
                ss << " (" << std::setw(options.getLongestValue<int>()) << std::right;
                if (vopt) {
                    ss << vopt->value << ") " << (vopt->required ? "R" : " ") << " : ";
                } else if (topt) {
                    ss << (topt->toggled ? "true" : "false") << ")   : ";
                } else if (mopt) {
                    ss << mopt->selected_value << ")   : ";
                } else if (popt) {
                    ss << popt->value << ") " << (popt->required ? "R" : " ") << " : ";
                }
                ss << option->description;
                if (mopt != nullptr) {
                    ss << " " << mopt->print_list();
                }
                ss << "\n";
            }
        }
        return ss.str();
    }

private:
    /// @brief Parses a value-holding option from the command-line arguments.
    /// @param vopt The ValueOption to be parsed.
    /// @return true if the option is parsed successfully, false otherwise.
    auto parseValueOption(const std::shared_ptr<detail::ValueOption> &vopt) -> bool
    {
        std::string value = tokenizer.getOption(vopt->opt_short);
        if (value.empty()) {
            value = tokenizer.getOption(vopt->opt_long);
            if (value.empty()) {
                if (vopt->required) {
                    std::cerr << "Cannot find required option: " << vopt->opt_long << " [" << vopt->opt_short << "]\n";
                    std::cerr << this->getHelp() << "\n";
                    std::exit(1); // Exit if required option is missing.
                }
                return false; // Skip optional missing options.
            }
        }
        vopt->value = value;
        options.updateLongestValue(vopt->value.length());
        return true;
    }

    /// @brief Parses a multi-option from the command-line arguments.
    /// @param mopt The MultiOption to be parsed.
    /// @return true if the option is parsed successfully, false otherwise.
    auto parseMultiOption(const std::shared_ptr<detail::MultiOption> &mopt) -> bool
    {
        std::string value = tokenizer.getOption(mopt->opt_short);
        if (value.empty()) {
            value = tokenizer.getOption(mopt->opt_long);
            if (value.empty()) {
                return false; // MultiOptions are not required, skip if not found.
            }
        }
        mopt->setValue(value);
        options.updateLongestValue(mopt->selected_value.length());
        return true;
    }

    /// @brief Parses a toggle option from the command-line arguments.
    /// @param topt The ToggleOption to be parsed.
    void parseToggleOption(const std::shared_ptr<detail::ToggleOption> &topt)
    {
        if (tokenizer.hasOption(topt->opt_short) || tokenizer.hasOption(topt->opt_long)) {
            topt->toggled = true;
        }
    }

    /// @brief Tokenizer for parsing command-line arguments.
    detail::Tokenizer tokenizer;
    /// @brief The list of registered options.
    detail::OptionList options;
    /// @brief Indicates whether options have been parsed.
    bool option_parsed{false};
};

} // namespace cmdlp