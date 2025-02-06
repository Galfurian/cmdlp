/// @file tokenizer.hpp
/// @brief Contains the tokenizer class.

#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace cmdlp
{
namespace detail
{

/// @class Tokenizer
/// @brief Tokenizes command-line arguments and provides access to options and positional arguments.
/// @details This class parses command-line arguments into options, their values, and positional arguments.
class Tokenizer
{
private:
    /// @brief Stores all command-line arguments (options and positional arguments).
    std::vector<std::string> tokens;

public:
    /// @brief Initializes the tokenizer with command-line arguments.
    /// @param argc The number of arguments passed to the program.
    /// @param argv The array of arguments.
    /// @details The constructor skips the program name (argv[0]) and stores all subsequent arguments as strings.
    Tokenizer(int argc, char **argv)
    {
        for (int i = 1; i < argc; ++i) {
            tokens.emplace_back(argv[i]);
        }
    }

    /// @brief Retrieves the value associated with a given option.
    /// @param option The option to search for (e.g., "-o" or "--option").
    /// @return A reference to the value associated with the option, or an empty string if not found.
    auto getOption(const std::string &option) const -> const std::string &
    {
        auto it = std::find(tokens.begin(), tokens.end(), option);
        if (it != tokens.end() && isOption(*it)) {
            ++it;
            if (it != tokens.end() && !isOption(*it)) {
                return *it;
            }
        }
        static const std::string empty_string;
        return empty_string;
    }

    /// @brief Checks if a given option exists in the arguments.
    /// @param option The option to search for (e.g., "-o" or "--option").
    /// @return True if the option exists, false otherwise.
    auto hasOption(const std::string &option) const -> bool
    {
        return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
    }

    /// @brief Retrieves the positional argument at the given index.
    /// @param index The index of the positional argument (0-based).
    /// @return The positional argument at the given index, or an empty string if out of bounds.
    auto getPositionalArgument(std::size_t index) const -> const std::string &
    {
        std::size_t pos_count = 0;
        for (std::size_t pos = 0; pos < tokens.size(); ++pos) {
            if (isOption(tokens[pos])) {
                ++pos;
            } else {
                if (pos_count == index) {
                    return tokens[pos];
                }
                ++pos_count;
            }
        }
        static const std::string empty_string;
        return empty_string;
    }

    /// @brief Checks if a positional argument exists at the given index.
    /// @param index The index of the positional argument (0-based).
    /// @return True if a positional argument exists at the specified index, false otherwise.
    auto hasPositionalArgument(std::size_t index) const -> bool
    {
        std::size_t pos_count = 0;
        for (std::size_t pos = 0; pos < tokens.size(); ++pos) {
            if (isOption(tokens[pos])) {
                ++pos;
            } else {
                if (pos_count == index) {
                    return true;
                }
                ++pos_count;
            }
        }
        return false;
    }

    /// @brief Retrieves all tokens (options, values, and positional arguments).
    /// @return A vector containing all tokens for raw access.
    auto getTokens() const -> const std::vector<std::string> & { return tokens; }

private:
    /// @brief Determines whether a token is an option.
    /// @param token The token to check.
    /// @return True if the token starts with '-', false otherwise.
    static auto isOption(const std::string &token) -> bool
    {
        return (!token.empty()) && (token[0] == '-') && !isNumber(token);
    }

    /// @brief Determines whether a token represents a number.
    /// @param token The token to check.
    /// @return True if the token represents a number, false otherwise.
    static auto isNumber(const std::string &token) -> bool
    {
        return !token.empty() && (token.find_first_not_of("-.eE0123456789") == std::string::npos);
    }
};

} // namespace detail
} // namespace cmdlp
