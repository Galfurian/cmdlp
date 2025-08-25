/// @file parser.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Defines the `Parser` class for managing and parsing command-line arguments.

#pragma once

#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>

#include "detail/option.hpp"
#include "detail/option_list.hpp"
#include "detail/tokenizer.hpp"

enum : unsigned char {
    CMDLP_MAJOR_VERSION = 1, ///< Major version of the library.
    CMDLP_MINOR_VERSION = 5, ///< Minor version of the library.
    CMDLP_MICRO_VERSION = 1  ///< Micro version of the library.
};

namespace cmdlp
{

/// @class ParsingError
/// @brief Exception thrown when a parsing error occurs.
class ParsingError : public std::runtime_error
{
public:
    /// @brief Constructs a ParsingError exception.
    /// @param message The error message.
    explicit ParsingError(const std::string &message)
        : std::runtime_error(message)
    {
    }
};

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
        , options()
        , option_parsed()
    {
    }

    /// @brief Constructor for std::list<std::string>
    /// @param args The list of arguments.
    Parser(const std::list<std::string> &args)
        : tokenizer(args)
        , options()
        , option_parsed()
    {
        // Nothing to do.
    }

    /// @brief Constructor for std::vector<std::string>
    /// @param args The list of arguments.
    Parser(const std::vector<std::string> &args)
        : tokenizer(args)
        , options()
        , option_parsed()
    {
        // Nothing to do.
    }

    /// @brief Constructor for std::array<std::string, N>
    /// @tparam N The number of elements.
    /// @param args The list of arguments.
    template <std::size_t N>
    Parser(const std::array<std::string, N> &args)
        : tokenizer(args)
        , options()
        , option_parsed()
    {
        // Nothing to do.
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
        for (const auto &option : options) {
            if (std::dynamic_pointer_cast<detail::PositionalList>(option)) {
                throw std::runtime_error("PositionalList must be the last positional argument.");
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
        for (const auto &option : options) {
            if (std::dynamic_pointer_cast<detail::PositionalList>(option)) {
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

    /// @brief Validates the parsed options, ensuring all required options are present.
    /// @details This method should be called after parseOptions() to perform validation.
    /// It will throw a ParsingError if any required options are missing.
    void validateOptions() const
    {
        for (const auto &option : options) {
            if (auto vopt = std::dynamic_pointer_cast<detail::ValueOption>(option)) {
                if (vopt->required && vopt->value.empty()) {
                    std::stringstream ss;
                    ss << "Missing required option: " << vopt->opt_long << " [" << vopt->opt_short << "]";
                    throw ParsingError(ss.str());
                }
            } else if (auto popt = std::dynamic_pointer_cast<detail::PositionalOption>(option)) {
                if (popt->required && popt->value.empty()) {
                    std::stringstream ss;
                    ss << "Missing required positional argument: " << popt->description;
                    throw ParsingError(ss.str());
                }
            } else if (auto plopt = std::dynamic_pointer_cast<detail::PositionalList>(option)) {
                if (plopt->required && plopt->values.empty()) {
                    std::stringstream ss;
                    ss << "Missing required positional list argument: " << plopt->description;
                    throw ParsingError(ss.str());
                }
            }
        }
    }

    /// @brief Parses the registered options from the command-line arguments.
    /// @details Reads the command-line arguments and assigns values to the corresponding options.
    /// If a required option is missing, the program will print an error and exit.
    void parseOptions()
    {
        std::set<std::size_t> used_tokens;
        size_t pos_arg_index = 0;

        for (const auto &option : options) {
            if (auto vopt = std::dynamic_pointer_cast<detail::ValueOption>(option)) {
                this->parseValueOption(vopt);
            } else if (auto mopt = std::dynamic_pointer_cast<detail::MultiOption>(option)) {
                this->parseMultiOption(mopt);
            } else if (auto topt = std::dynamic_pointer_cast<detail::ToggleOption>(option)) {
                this->parseToggleOption(topt);
            } else if (auto posopt = std::dynamic_pointer_cast<detail::PositionalOption>(option)) {
                this->parsePositionalOption(posopt, pos_arg_index);
            } else if (auto poslist = std::dynamic_pointer_cast<detail::PositionalList>(option)) {
                this->parsePositionalList(poslist, pos_arg_index);
            }
        }

        option_parsed = true;
    }

    /// @brief Generates a concise usage string for the command.
    /// @return A formatted usage string.
    auto getUsage() const -> std::string
    {
        constexpr std::size_t max_line_length = 80;
        constexpr std::size_t indent          = 7;
        std::ostringstream oss;
        std::string usage_prefix   = "Usage: " + tokenizer.front();
        std::size_t current_length = usage_prefix.length();
        oss << usage_prefix;
        for (const auto &option : options) {
            if (dynamic_cast<detail::Separator *>(option.get()) != nullptr) {
                continue; // Skip separators in usage.
            }
            std::string entry_usage = option->get_usage_entry();
            if (current_length + entry_usage.length() > max_line_length) {
                oss << "\n"
                    << std::string(indent, ' ')
                    << entry_usage;
                current_length = indent;
            } else {
                oss << " " << entry_usage;
            }
            current_length += entry_usage.length() + 1;
        }
        return oss.str();
    }

    /// @brief Generates a help string for all registered options.
    /// @details Lists all options with their short and long names, default
    /// values, and descriptions.
    /// @return A string containing the help text for all options.
    auto getHelp() const -> std::string
    {
        std::stringstream ss;
        // Pre-compute the maximum length of the short+long+value for all options.
        std::size_t max_length        = options.getLongestShortOption<int>() + options.getLongestLongOption<int>() + options.getLongestValue<int>() + 4;
        // If the max length is above 40, we need to move all options description to the next line.
        bool description_on_next_line = max_length > 40;

        ss << this->getUsage() << "\n";
        for (const auto &option : options) {
            auto sepr = std::dynamic_pointer_cast<detail::Separator>(option);
            if (sepr) {
                ss << "\n"
                   << sepr->description << "\n";
            } else {
                auto vopt = std::dynamic_pointer_cast<detail::ValueOption>(option);
                auto topt = std::dynamic_pointer_cast<detail::ToggleOption>(option);
                auto mopt = std::dynamic_pointer_cast<detail::MultiOption>(option);
                auto popt = std::dynamic_pointer_cast<detail::PositionalOption>(option);
                auto lopt = std::dynamic_pointer_cast<detail::PositionalList>(option);

                std::stringstream ssopt;
                ssopt << " " << std::setw(options.getLongestShortOption<int>()) << std::left << option->opt_short;
                ssopt << " " << std::setw(options.getLongestLongOption<int>()) << std::left << option->opt_long;
                ssopt << " " << std::setw(options.getLongestValue<int>()) << std::left;

                auto description = option->description;
                if (vopt) {
                    ssopt << ((vopt->required && vopt->value.empty()) ? "<req>" : vopt->value);
                } else if (topt) {
                    ssopt << (topt->toggled ? "true" : "false");
                } else if (mopt) {
                    ssopt << mopt->value;
                    description += " [" + mopt->print_list() + "]";
                } else if (popt) {
                    ssopt << ((popt->required && popt->value.empty()) ? "<req>" : popt->value);
                } else if (lopt) {
                    ssopt << ((lopt->required && lopt->values.empty()) ? "<req>" : lopt->print_values());
                }
                std::size_t initial_offset, tabulation;
                if (description_on_next_line) {
                    ssopt << "\n    : ";
                    initial_offset = 6;
                    tabulation     = 6;
                } else {
                    ssopt << " : ";
                    initial_offset = ssopt.str().length();
                    tabulation     = initial_offset;
                }
                ssopt << this->format_paragraph(description, initial_offset, tabulation, 80);
                ssopt << "\n";
                ss << ssopt.str();
            }
        }
        return ss.str();
    }

private:
    /// @brief Parses a value-holding option from the command-line arguments.
    /// @param option The ValueOption to be parsed.
    /// @return true if the option is parsed successfully, false otherwise.
    auto parseValueOption(const std::shared_ptr<detail::ValueOption> &option) -> bool
    {
        std::string value = tokenizer.getOption(option->opt_short);
        if (value.empty()) {
            value = tokenizer.getOption(option->opt_long);
            if (value.empty()) {
                return false; // Skip optional missing options.
            }
        }
        option->value = value;
        options.updateLongestValue(option->value.length());
        return true;
    }

    /// @brief Parses a multi-option from the command-line arguments.
    /// @param option The MultiOption to be parsed.
    /// @return true if the option is parsed successfully, false otherwise.
    auto parseMultiOption(const std::shared_ptr<detail::MultiOption> &option) -> bool
    {
        std::string value = tokenizer.getOption(option->opt_short);
        if (value.empty()) {
            value = tokenizer.getOption(option->opt_long);
            if (value.empty()) {
                return false; // MultiOptions are not required, skip if not found.
            }
        }
        option->setValue(value);
        options.updateLongestValue(option->value.length());
        return true;
    }

    /// @brief Parses a toggle option from the command-line arguments.
    /// @param option The ToggleOption to be parsed.
    void parseToggleOption(const std::shared_ptr<detail::ToggleOption> &option)
    {
        if (tokenizer.hasOption(option->opt_short) || tokenizer.hasOption(option->opt_long)) {
            option->toggled = true;
        }
    }

    /// @brief Parses a positional option from the command-line arguments.
    /// @param option The PositionalOption to be parsed.
    /// @param pos_arg_index The current index being considered for positional arguments.
    void parsePositionalOption(const std::shared_ptr<detail::PositionalOption> &option, std::size_t &pos_arg_index)
    {
        // Get all toggle options to differentiate them from other arguments.
        auto toggles          = this->getToggleOptions();
        // Keep track of the current positional argument.
        std::size_t arg_index = 0;
        // Iterate over tokens.
        for (auto prev = tokenizer.begin(), it = std::next(prev); it != tokenizer.end(); ++it, ++prev) {
            // Skip the token if it is an option.
            if (detail::Tokenizer::isOption(*it)) {
                continue;
            }
            // Check if the previous token is either not an option or is a toggle.
            // If not, skip this token as it isn't a valid positional argument.
            if (detail::Tokenizer::isOption(*prev) && !isToggle(toggles, *prev)) {
                continue;
            }
            // We assign the positional argument.
            if (arg_index++ == pos_arg_index) {
                // Set the value.
                option->value = *it;
                // Move the argument index.
                ++pos_arg_index;
                // Update the longest value.
                options.updateLongestValue(it->length());
                // Exit after assigning the positional argument.
                return;
            }
        }
        // No longer throwing for missing required positional argument here.
        // Validation will be handled by validateOptions().
    }

    /// @brief Parses a positional list from the command-line arguments.
    /// @param option The PositionalList to be parsed.
    /// @param tokens The list of tokens from the tokenizer.
    /// @param used_tokens A set of indices marking which tokens are already used.
    /// @param pos_arg_index The current index of positional arguments.
    void parsePositionalList(const std::shared_ptr<detail::PositionalList> &option, std::size_t &pos_arg_index)
    {
        // Get all toggle options to differentiate them from other arguments.
        auto toggles          = this->getToggleOptions();
        // Keep track of the current positional argument.
        std::size_t arg_index = 0;
        // Iterate over tokens.
        for (auto prev = tokenizer.begin(), it = std::next(prev); it != tokenizer.end(); ++it, ++prev) {
            // Skip the token if it is an option.
            if (detail::Tokenizer::isOption(*it)) {
                continue;
            }
            // Check if the previous token is either not an option or is a toggle.
            // If not, skip this token as it isn't a valid positional argument.
            if (detail::Tokenizer::isOption(*prev) && !isToggle(toggles, *prev)) {
                continue;
            }
            // We assign the positional argument.
            if (arg_index++ >= pos_arg_index) {
                // Add the value.
                option->values.emplace_back(*it);
                // Move the argument index.
                ++pos_arg_index;
                // Update the longest value.
                options.updateLongestValue(option->print_values().length());
            }
        }
        // No longer throwing for missing required positional list argument here.
        // Validation will be handled by validateOptions().
    }

    /// @brief Formats a paragraph to fit within a specified line width, applying indentation.
    /// @param text The paragraph to be formatted.
    /// @param initial_offset The character length already printed on the first line.
    /// @param tabulation The indentation applied to all subsequent lines.
    /// @param max_line_length The maximum length of each line.
    /// @return A formatted string with proper line breaks and indentation.
    auto format_paragraph(
        const std::string &text,
        std::size_t initial_offset,
        std::size_t tabulation,
        std::size_t max_line_length) const -> std::string
    {
        std::ostringstream formatted;
        std::size_t current_length = initial_offset;
        std::string indent(tabulation, ' ');
        auto it = text.begin();
        while (it != text.end()) {
            auto word_start = it;
            // Find the end of the current word.
            auto word_end   = std::find(it, text.end(), ' ');
            std::string word(word_start, word_end);
            // Check if adding the word would exceed the max line length.
            if (current_length + word.length() + 1 > max_line_length) {
                formatted << "\n"
                          << indent;
                current_length = tabulation;
            } else if (it != text.begin()) {
                formatted << ' ';
                ++current_length;
            }
            // Append the word.
            formatted << word;
            current_length += word.length();
            // Move iterator to next word.
            it = (word_end == text.end()) ? word_end : std::next(word_end);
        }

        return formatted.str();
    }

    /// @brief Retrieves all toggle options from the current option list.
    /// @return A vector of shared pointers to ToggleOptions.
    auto getToggleOptions() const -> std::vector<std::shared_ptr<detail::ToggleOption>>
    {
        auto toggles = options.filter([](const std::shared_ptr<detail::Option> &opt) {
            return std::dynamic_pointer_cast<detail::ToggleOption>(opt) != nullptr;
        });
        std::vector<std::shared_ptr<detail::ToggleOption>> toggle_options;
        for (const auto &entry : toggles) {
            toggle_options.emplace_back(std::dynamic_pointer_cast<detail::ToggleOption>(entry));
        }
        return toggle_options;
    }

    /// @brief Checks if a given option string is a toggle.
    /// @param toggles The vector of toggle options.
    /// @param option The option to check.
    /// @return true if the option matches a toggle option, false otherwise.
    static auto isToggle(const std::vector<std::shared_ptr<detail::ToggleOption>> &toggles, const std::string &option)
        -> bool
    {
        return std::any_of(toggles.begin(), toggles.end(), [&](const auto &entry) {
            return option == entry->opt_short || option == entry->opt_long;
        });
    }

    /// @brief Tokenizer for parsing command-line arguments.
    detail::Tokenizer tokenizer;
    /// @brief The list of registered options.
    detail::OptionList options;
    /// @brief Indicates whether options have been parsed.
    bool option_parsed{false};
};

} // namespace cmdlp