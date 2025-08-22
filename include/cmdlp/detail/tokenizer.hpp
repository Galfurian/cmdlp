/// @file tokenizer.hpp
/// @brief Contains the tokenizer class.

#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <list>
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
        : tokens()
    {
        for (int i = 0; i < argc; ++i) {
            tokens.emplace_back(argv[i]);
        }
    }

    /// @brief Constructor for std::list<std::string>
    /// @param args The list of arguments.
    Tokenizer(const std::list<std::string> &args)
        : tokens()
    {
        for (const auto &argument : args) {
            tokens.emplace_back(argument);
        }
    }

    /// @brief Constructor for std::vector<std::string>
    /// @param args The list of arguments.
    Tokenizer(const std::vector<std::string> &args)
        : tokens()
    {
        for (const auto &argument : args) {
            tokens.emplace_back(argument);
        }
    }

    /// @brief Constructor for std::array<std::string, N>
    /// @tparam N The number of elements.
    /// @param args The list of arguments.
    template <std::size_t N>
    Tokenizer(const std::array<std::string, N> &args)
        : tokens()
    {
        for (const auto &argument : args) {
            tokens.emplace_back(argument);
        }
    }

    /// @brief Retrieves the value associated with a given option.
    /// @param option The option to search for (e.g., "-o" or "--option").
    /// @return A reference to the value associated with the option, or an empty string if not found.
    auto getOption(const std::string &option) const -> std::string
    {
        for (auto it = std::next(tokens.begin()); it != tokens.end(); ++it) {
            // Case 1: Option followed by a separate value token (e.g., --option value)
            if (*it == option) {
                auto next_it = std::next(it);
                if (next_it != tokens.end() && !isOption(*next_it)) {
                    return *next_it;
                }
            }
            // Case 2: Long option with an equals sign (e.g., --option=value)
            else if (option.length() > 2 && option.substr(0, 2) == "--" && it->rfind(option + "=", 0) == 0) {
                return it->substr(option.length() + 1);
            }
            // Case 3: Short option with concatenated value (e.g., -ovalue)
            else if (option.length() == 2 && option.substr(0, 1) == "-" && it->rfind(option, 0) == 0 && it->length() > 2) {
                return it->substr(2);
            }
        }
        return "";
    }

    /// @brief Checks if a given option exists in the arguments.
    /// @param option The option to search for (e.g., "-o" or "--option").
    /// @return True if the option exists, false otherwise.
    auto hasOption(const std::string &option) const -> bool
    {
        return std::find(std::next(tokens.begin()), tokens.end(), option) != tokens.end();
    }

    /// @brief Retrieves all tokens (options, values, and positional arguments).
    /// @return A vector containing all tokens for raw access.
    auto getTokens() const -> const std::vector<std::string> & { return tokens; }

    /// @brief Checks if the tokenizer has no tokens.
    /// @return True if there are no tokens, false otherwise.
    auto empty() const -> bool { return tokens.empty(); }

    /// @brief Returns the number of tokens in the tokenizer.
    /// @return The total count of tokens.
    auto size() const -> std::size_t { return tokens.size(); }

    /// @brief Removes all tokens from the tokenizer.
    void clear() { tokens.clear(); }

    /// @brief Finds the index of the specified token.
    /// @param token The token to search for.
    /// @return The index of the token, or std::string::npos if not found.
    auto find(const std::string &token) const -> std::size_t
    {
        auto it = std::find(tokens.begin(), tokens.end(), token);
        return (it != tokens.end()) ? static_cast<std::size_t>(std::distance(tokens.begin(), it)) : std::string::npos;
    }

    /// @brief Provides non-const access to the token at the specified index.
    /// @param index The index of the token to access.
    /// @return Reference to the token at the specified index.
    /// @note No bounds checking is performed. Use at() for bounds-checked access.
    auto operator[](std::size_t index) -> std::string & { return tokens[index]; }

    /// @brief Provides const access to the token at the specified index.
    /// @param index The index of the token to access.
    /// @return Const reference to the token at the specified index.
    /// @note No bounds checking is performed. Use at() for bounds-checked access.
    auto operator[](std::size_t index) const -> const std::string & { return tokens[index]; }

    /// @brief Provides bounds-checked access to a token at a specific index.
    /// @param index The index of the token.
    /// @return Reference to the token at the specified index.
    /// @throws std::out_of_range if the index is out of bounds.
    auto at(std::size_t index) -> std::string & { return tokens.at(index); }

    /// @brief Provides const bounds-checked access to a token at a specific index.
    /// @param index The index of the token.
    /// @return Const reference to the token at the specified index.
    /// @throws std::out_of_range if the index is out of bounds.
    auto at(std::size_t index) const -> const std::string & { return tokens.at(index); }

    /// @brief Returns a reference to the first token.
    /// @return Reference to the first token.
    auto front() -> std::string & { return tokens.front(); }

    /// @brief Returns a const reference to the first token.
    /// @return Const reference to the first token.
    auto front() const -> const std::string & { return tokens.front(); }

    /// @brief Returns a reference to the last token.
    /// @return Reference to the last token.
    auto back() -> std::string & { return tokens.back(); }

    /// @brief Returns a const reference to the last token.
    /// @return Const reference to the last token.
    auto back() const -> const std::string & { return tokens.back(); }

    /// @brief Returns an iterator to the beginning of the tokens.
    /// @return Iterator to the first element.
    auto begin() -> std::vector<std::string>::iterator { return tokens.begin(); }

    /// @brief Returns an iterator to the end of the tokens.
    /// @return Iterator to one past the last element.
    auto end() -> std::vector<std::string>::iterator { return tokens.end(); }

    /// @brief Returns a reverse iterator to the beginning (last element) of the tokens.
    /// @return Reverse iterator to the last element.
    auto rbegin() -> std::vector<std::string>::reverse_iterator { return tokens.rbegin(); }

    /// @brief Returns a reverse iterator to the end (before first element) of the tokens.
    /// @return Reverse iterator to one before the first element.
    auto rend() -> std::vector<std::string>::reverse_iterator { return tokens.rend(); }

    /// @brief Returns a const iterator to the beginning of the tokens.
    /// @return Const iterator to the first element.
    auto begin() const -> std::vector<std::string>::const_iterator { return tokens.begin(); }

    /// @brief Returns a const iterator to the end of the tokens.
    /// @return Const iterator to one past the last element.
    auto end() const -> std::vector<std::string>::const_iterator { return tokens.end(); }

    /// @brief Returns a const reverse iterator to the beginning (last element) of the tokens.
    /// @return Const reverse iterator to the last element.
    auto rbegin() const -> std::vector<std::string>::const_reverse_iterator { return tokens.rbegin(); }

    /// @brief Returns a const reverse iterator to the end (before first element) of the tokens.
    /// @return Const reverse iterator to one before the first element.
    auto rend() const -> std::vector<std::string>::const_reverse_iterator { return tokens.rend(); }

    /// @brief Returns a const iterator to the beginning of the tokens (for const-qualified objects).
    /// @return Const iterator to the first element.
    auto cbegin() const -> std::vector<std::string>::const_iterator { return tokens.cbegin(); }

    /// @brief Returns a const iterator to the end of the tokens (for const-qualified objects).
    /// @return Const iterator to one past the last element.
    auto cend() const -> std::vector<std::string>::const_iterator { return tokens.cend(); }

    /// @brief Returns a const reverse iterator to the beginning of the tokens (for const-qualified objects).
    /// @return Const reverse iterator to the last element.
    auto crbegin() const -> std::vector<std::string>::const_reverse_iterator { return tokens.crbegin(); }

    /// @brief Returns a const reverse iterator to the end of the tokens (for const-qualified objects).
    /// @return Const reverse iterator to one before the first element.
    auto crend() const -> std::vector<std::string>::const_reverse_iterator { return tokens.crend(); }

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
