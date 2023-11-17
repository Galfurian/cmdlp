/// @file tokenizer.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace cmdlp
{

class Tokenizer {
private:
    typedef std::vector<std::string> token_list_t;

    token_list_t tokens;

public:
    Tokenizer(int argc, char **argv)
    {
        for (int i = 1; i < argc; ++i) {
            tokens.push_back(std::string(argv[i]));
        }
    }

    /// @brief Retrieves the given option.
    /// @param option The option to search.
    /// @return The value for the given option.
    inline const std::string &getOption(const std::string &option) const
    {
        token_list_t::const_iterator it = std::find(tokens.begin(), tokens.end(), option);
        if ((it != tokens.end()) && (++it != tokens.end())) {
            if (!this->begin_with(*it, "-", false, 0)) {
                return (*it);
            }
        }
        static const std::string empty_string;
        return empty_string;
    }

    /// @brief Retrieves the given option.
    /// @param option The option to search.
    /// @return The value for the given option.
    inline const std::string &getOption(char option) const
    {
        static char sopt[2];
        sopt[0] = option;
        sopt[1] = '\0';
        return this->getOption(sopt);
    }

    /// @brief Checks if the given option exists.
    /// @param option The option to search.
    /// @return If the option exists.
    inline bool hasOption(const std::string &option) const
    {
        return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
    }

    /// @brief Checks if the given option exists.
    /// @param option The option to search.
    /// @return If the option exists.
    inline bool hasOption(char option) const
    {
        std::string strOption;
        strOption.push_back('-');
        strOption.push_back(option);
        return this->hasOption(strOption);
    }

private:
    /// @brief Compares the two characters.
    /// @param ch0 The first character.
    /// @param ch1 The second character.
    /// @param sensitive enables case-sensitive check.
    /// @return true if the characters are equal.
    /// @return false otherwise.
    inline bool compare_char(char ch0, char ch1, bool sensitive) const
    {
        return sensitive ? (ch0 == ch1) : (std::toupper(ch0) == std::toupper(ch1));
    }

    /// @brief Checks if the source string begins with a given string.
    /// @param s source string.
    /// @param prefix the prefix to check.
    /// @param sensitive enables case-sensitive check.
    /// @param n the number of characters to check (0 = all of prefix).
    /// @return true if the string beings with the given prefix.
    /// @return false otherwise.
    inline bool begin_with(const std::string &s, const std::string &prefix, bool sensitive, unsigned n) const
    {
        if (&prefix == &s) {
            return true;
        }
        if (prefix.length() > s.length()) {
            return false;
        }
        if (s.empty() || prefix.empty()) {
            return false;
        }
        std::string::const_iterator it0 = s.begin(), it1 = prefix.begin();
        while ((it1 != prefix.end()) && this->compare_char(*it0, *it1, sensitive)) {
            if ((n > 0) && (--n <= 0)) {
                return true;
            }
            ++it0, ++it1;
        }
        return it1 == prefix.end();
    }
};

} // namespace cmdlp
