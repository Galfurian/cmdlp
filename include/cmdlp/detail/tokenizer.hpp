/// @file tokenizer.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
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
/// @brief A class to tokenize and parse command-line arguments.
/// @details This class parses command-line arguments into options and their values.
/// Options are expected to start with '-' or '--', and values are expected to follow the respective options.
class Tokenizer
{
private:
    /// @brief Stores the arguments as a list of strings.
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
    /// @details Searches for the specified option in the tokens. If found, it returns the next token as the value.
    inline const std::string &getOption(const std::string &option) const
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
    /// @details Searches for the specified option in the tokens and returns whether it exists.
    inline bool hasOption(const std::string &option) const
    {
        return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
    }

private:
    /// @brief Determines whether a token is an option.
    /// @param token The token to check.
    /// @return True if the token starts with '-', false otherwise.
    /// @details Checks whether the given token starts with a '-' character, indicating it is an option.
    static inline bool isOption(const std::string &token)
    {
        return (!token.empty()) && (token[0] == '-') && !isNumber(token);
    }

    /// @brief Determines whether a token represents a number.
    /// @param token The token to check.
    /// @return True if the token represents a number, false otherwise.
    /// @details Checks if the given token contains only digit characters, indicating it is a numeric value.
    static inline bool isNumber(const std::string &token)
    {
        return !token.empty() && (token.find_first_not_of("-.eE0123456789") == std::string::npos);
    }
};

} // namespace detail

} // namespace cmdlp
