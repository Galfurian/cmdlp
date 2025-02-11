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
    CMDLP_MINOR_VERSION = 2, ///< Minor version of the library.
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
        bool _required,
        const T &_value)
    {
        // Turn the value to string.
        std::stringstream ss;
        ss << _value;
        // Add the option to the list.
        options.addOption(
            std::make_shared<detail::ValueOption>(_opt_short, _opt_long, _description, _required, ss.str()));
    }

    /// @brief Adds a value-based option to the parser.
    /// @tparam T The type of the option's default value.
    /// @param _opt_short The short version of the option (e.g., "-f").
    /// @param _opt_long The long version of the option (e.g., "--file").
    /// @param _description A description of the option, displayed in the help text.
    /// @param _required Indicates whether the option is required.
    void addOption(
        const std::string &_opt_short,
        const std::string &_opt_long,
        const std::string &_description,
        bool _required)
    {
        // Add the option to the list.
        options.addOption(
            std::make_shared<detail::ValueOption>(_opt_short, _opt_long, _description, _required, std::string()));
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

    /// @brief Adds a multi-value option to the parser.
    /// @param _opt_short The short version of the option (e.g., "-m").
    /// @param _opt_long The long version of the option (e.g., "--mode").
    /// @param _description A description of the option, displayed in the help text.
    /// @param _allowed_values The set of predefined values for the option.
    /// @param _value The default value for the option.
    /// @throws std::invalid_argument if the default value is not in the list of allowed values.
    void addMultiOption(
        const std::string &_opt_short,
        const std::string &_opt_long,
        const std::string &_description,
        const std::vector<std::string> &_allowed_values,
        const std::string &_value)
    {
        // Add the option to the list.
        options.addOption(
            std::make_shared<detail::MultiOption>(_opt_short, _opt_long, _description, _allowed_values, _value));
    }

    /// @brief Adds a positional option to the parser.
    /// @param _opt_short The short version of the option (e.g., "-f").
    /// @param _opt_long The long version of the option (e.g., "--file").
    /// @param _description A description of the option.
    /// @param _required Whether the option is required.
    /// @param _value The default value for the option.
    void addPositionalOption(
        const std::string &_opt_short,
        const std::string &_opt_long,
        const std::string &_description,
        bool _required            = false,
        const std::string &_value = std::string())
    {
        // Check if any PositionalList exists already (must be the last positional argument).
        for (auto it = options.rbegin(); it != options.rend(); ++it) {
            if (std::dynamic_pointer_cast<detail::PositionalList>(*it)) {
                throw std::runtime_error("PositionalList must be the last positional argument.");
            }
            // Stop as soon as we encounter a non-positional option.
            if (!std::dynamic_pointer_cast<detail::PositionalOption>(*it)) {
                break;
            }
        }
        options.addOption(
            std::make_shared<detail::PositionalOption>(_opt_short, _opt_long, _description, _required, _value));
    }

    /// @brief Adds a positional list option to the parser.
    /// @param _opt_short The short version of the option (e.g., "-f").
    /// @param _opt_long The long version of the option (e.g., "--files").
    /// @param _description A description of the positional list.
    /// @param _required Whether the list is required (default: false).
    void addPositionalList(
        const std::string &_opt_short,
        const std::string &_opt_long,
        const std::string &_description,
        bool _required = false)
    {
        for (auto it = options.rbegin(); it != options.rend(); ++it) {
            if (std::dynamic_pointer_cast<detail::PositionalList>(*it)) {
                throw std::runtime_error("Only one PositionalList is allowed.");
            }
        }
        options.addOption(std::make_shared<detail::PositionalList>(_opt_short, _opt_long, _description, _required));
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
        size_t pos_arg_index         = 0;
        size_t total_positional_args = tokenizer.getPositionalArgumentCount();

        for (const auto &option : options) {
            if (auto vopt = std::dynamic_pointer_cast<detail::ValueOption>(option)) {
                this->parseValueOption(vopt);
            } else if (auto mopt = std::dynamic_pointer_cast<detail::MultiOption>(option)) {
                this->parseMultiOption(mopt);
            } else if (auto topt = std::dynamic_pointer_cast<detail::ToggleOption>(option)) {
                this->parseToggleOption(topt);
            } else if (auto posopt = std::dynamic_pointer_cast<detail::PositionalOption>(option)) {
                this->parsePositionalOption(posopt, pos_arg_index, total_positional_args);
            } else if (auto poslist = std::dynamic_pointer_cast<detail::PositionalList>(option)) {
                this->parsePositionalList(poslist, pos_arg_index, total_positional_args);
            }
        }

        if (pos_arg_index < total_positional_args) {
            std::cerr << "Unexpected extra positional arguments provided.\n";
            std::cerr << this->getHelp() << "\n";
            std::exit(1);
        }

        option_parsed = true;
    }

    /// @brief Generates a concise usage string for the command.
    /// @return A formatted usage string.
    auto getUsage() const -> std::string
    {
        std::stringstream usage;
        usage << "Usage: command";

        bool has_optional_options = false;
        std::vector<std::string> positional_args;

        for (const auto &option : options) {
            // Handle required ValueOption and MultiOption
            if (auto vopt = std::dynamic_pointer_cast<detail::ValueOption>(option)) {
                if (vopt->required) {
                    usage << " " << vopt->opt_long << " VALUE";
                } else {
                    has_optional_options = true;
                }
            } else if (auto mopt = std::dynamic_pointer_cast<detail::MultiOption>(option)) {
                usage << " " << mopt->opt_long << " {" << mopt->print_list() << "}";
            }
            // Mark if any optional ToggleOption exists
            else if (std::dynamic_pointer_cast<detail::ToggleOption>(option)) {
                has_optional_options = true;
            }
            // Collect positional arguments
            else if (auto posopt = std::dynamic_pointer_cast<detail::PositionalOption>(option)) {
                std::string clean_name = posopt->opt_long.substr(2); // Remove '--' prefix
                positional_args.push_back("<" + clean_name + ">");
            } else if (auto poslist = std::dynamic_pointer_cast<detail::PositionalList>(option)) {
                std::string clean_name = poslist->opt_long.substr(2); // Remove '--' prefix
                positional_args.push_back("<" + clean_name + "...>");
            }
        }

        // Add generic placeholder for optional options
        if (has_optional_options) {
            usage << " [OPTIONS...]";
        }

        // Append positional arguments
        for (const auto &pos_arg : positional_args) {
            usage << " " << pos_arg;
        }

        return usage.str();
    }

    /// @brief Generates a help string for all registered options.
    /// @details Lists all options with their short and long names, default
    /// values, and descriptions.
    /// @return A string containing the help text for all options.
    auto getHelp() const -> std::string
    {
        std::stringstream ss;
        ss << this->getUsage() << "\n";
        for (const auto &option : options) {
            auto sepr = std::dynamic_pointer_cast<detail::Separator>(option);
            if (sepr) {
                ss << "\n" << sepr->description << "\n";
            } else {
                auto vopt  = std::dynamic_pointer_cast<detail::ValueOption>(option);
                auto topt  = std::dynamic_pointer_cast<detail::ToggleOption>(option);
                auto mopt  = std::dynamic_pointer_cast<detail::MultiOption>(option);
                auto popt  = std::dynamic_pointer_cast<detail::PositionalOption>(option);
                auto plopt = std::dynamic_pointer_cast<detail::PositionalList>(option);
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
                } else if (plopt) {
                    ss << plopt->print_values() << ") " << (plopt->required ? "R" : " ") << " : ";
                }
                ss << option->description;
                if (mopt != nullptr) {
                    ss << " [" << mopt->print_list() << "]";
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

    /// @brief Parses a positional option from the command-line arguments.
    /// @param posopt The PositionalOption to be parsed.
    /// @param pos_arg_index The current index of positional arguments.
    /// @param total_positional_args The total number of positional arguments.
    void parsePositionalOption(
        const std::shared_ptr<detail::PositionalOption> &posopt,
        std::size_t &pos_arg_index,
        std::size_t total_positional_args)
    {
        if (pos_arg_index < total_positional_args) {
            posopt->value = tokenizer.getPositionalArgument(pos_arg_index++);
            options.updateLongestValue(posopt->value.length());
        } else if (posopt->required) {
            std::cerr << "Missing required positional argument: " << posopt->description << "\n";
            std::cerr << this->getHelp() << "\n";
            std::exit(1);
        }
    }

    /// @brief Parses a positional list from the command-line arguments.
    /// @param poslist The PositionalList to be parsed.
    /// @param pos_arg_index The current index of positional arguments.
    /// @param total_positional_args The total number of positional arguments.
    void parsePositionalList(
        const std::shared_ptr<detail::PositionalList> &poslist,
        std::size_t &pos_arg_index,
        std::size_t total_positional_args)
    {
        while (pos_arg_index < total_positional_args) {
            poslist->values.push_back(tokenizer.getPositionalArgument(pos_arg_index++));
        }
        options.updateLongestValue(poslist->print_values().length());
        if (poslist->required && poslist->values.empty()) {
            std::cerr << "Missing required positional list of arguments for: `" << poslist->description << "`\n";
            std::cerr << this->getHelp() << "\n";
            std::exit(1);
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