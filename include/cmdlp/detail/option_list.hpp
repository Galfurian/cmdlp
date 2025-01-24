/// @file option_list.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Defines classes for managing and storing command-line options.

#pragma once

#include "option.hpp"

#include <exception>
#include <sstream>
#include <vector>

namespace cmdlp
{

namespace detail
{

/// @class OptionExistException
/// @brief Exception thrown when attempting to add a duplicate option.
class OptionExistException : public std::exception {
private:
    /// @brief Message describing the conflict.
    std::string msg;

public:
    /// @brief Constructs an `OptionExistException`.
    /// @param _new_option The new option that caused the conflict.
    /// @param _existing_option The existing option that conflicts with the new one.
    OptionExistException(Option *_new_option, Option *_existing_option)
        : std::exception()
    {
        std::stringstream ss;
        ss << "Option (" << _new_option->opt_short << ", " << _new_option->opt_long
           << ") already exists: (" << _existing_option->opt_short << ", " << _existing_option->opt_long << ")\n";
        msg = ss.str();
    }

    /// @brief Returns the error message.
    /// @return A C-string containing the error message.
    const char *what() const noexcept override
    {
        return msg.c_str();
    }
};

/// @class OptionList
/// @brief Manages a list of command-line options.
class OptionList {
public:
    /// @brief Alias for a vector of `Option` pointers.
    using option_list_t = std::vector<Option *>;
    /// @brief Alias for an iterator over the option list.
    using iterator_t = std::vector<Option *>::iterator;
    /// @brief Alias for a const iterator over the option list.
    using const_iterator_t = std::vector<Option *>::const_iterator;

    /// @brief Constructs an empty `OptionList`.
    OptionList()
        : options(),
          longest_short_option(0),
          longest_long_option(0),
          longest_value(0)
    {
    }

    /// @brief Copy constructor.
    /// @param other The `OptionList` to copy.
    /// @details Creates deep copies of the options in the list.
    OptionList(const OptionList &other)
        : options(),
          longest_short_option(other.longest_short_option),
          longest_long_option(other.longest_long_option),
          longest_value(other.longest_value)
    {
        const MultiOption *mopt;
        const ToggleOption *topt;
        const ValueOption *vopt;
        for (const_iterator_t it = other.options.begin(); it != other.options.end(); ++it) {
            if ((vopt = dynamic_cast<const ValueOption *>(*it))) {
                options.emplace_back(new ValueOption(*vopt));
            } else if ((topt = dynamic_cast<const ToggleOption *>(*it))) {
                options.emplace_back(new ToggleOption(*topt));
            } else if ((mopt = dynamic_cast<const MultiOption *>(*it))) {
                options.emplace_back(new MultiOption(*mopt));
            }
        }
    }

    /// @brief Destructor.
    /// @details Cleans up all dynamically allocated options.
    virtual ~OptionList()
    {
        for (iterator_t it = options.begin(); it != options.end(); ++it) {
            delete *it;
        }
    }

    /// @brief Finds an option by its short or long name.
    /// @param option_string The short or long name of the option.
    /// @return A pointer to the `Option` if found, or `nullptr` otherwise.
    inline const Option *findOption(const std::string &option_string) const
    {
        for (const_iterator_t it = options.begin(); it != options.end(); ++it) {
            if (((*it)->opt_short == option_string) || ((*it)->opt_long == option_string)) {
                return *it;
            }
        }
        return nullptr;
    }

    /// @brief Checks if an option exists by its name.
    /// @param option_string The short or long name of the option.
    /// @return True if the option exists, false otherwise.
    inline bool optionExists(const std::string &option_string) const
    {
        return this->findOption(option_string) != nullptr;
    }

    /// @brief Retrieves the value of an option.
    /// @tparam T The expected type of the option value.
    /// @param option_string The short or long name of the option.
    /// @return The value of the option, or the default value of `T` if not found.
    template <typename T>
    inline T getOption(const std::string &option_string) const
    {
        const Option *option = this->findOption(option_string);
        if (option) {
            const MultiOption *mopt;
            const ToggleOption *topt;
            const ValueOption *vopt;
            std::stringstream ss;
            if ((vopt = dynamic_cast<const ValueOption *>(option))) {
                ss << vopt->value;
            } else if ((topt = dynamic_cast<const ToggleOption *>(option))) {
                ss << topt->toggled;
            } else if ((mopt = dynamic_cast<const MultiOption *>(option))) {
                ss << mopt->selected_value;
            }
            T data;
            ss >> data;
            return data;
        }
        return T(0);
    }

    /// @brief Adds an option to the list.
    /// @param option The option to add.
    /// @throws OptionExistException if the option already exists.
    inline void addOption(Option *option)
    {
        // If the option is a separator, skip all checks.
        if (dynamic_cast<detail::Separator *>(option)) {
            options.push_back(option);
            return;
        }

        // Check if the option already exists in the list of options.
        for (iterator_t it = options.begin(); it != options.end(); ++it) {
            if ((*it)->opt_short == option->opt_short || (*it)->opt_long == option->opt_long) {
                throw OptionExistException(option, *it);
            }
        }

        // Add the option to the list of options.
        options.push_back(option);

        // Update the length of the `longest` parameters.
        if (option->opt_short.length() > longest_short_option) {
            longest_short_option = option->opt_short.length();
        }
        if (option->opt_long.length() > longest_long_option) {
            longest_long_option = option->opt_long.length();
        }
        if (option->get_value_length() > longest_value) {
            longest_value = option->get_value_length();
        }
    }

    /// @brief Returns a const iterator to the beginning of the list.
    /// @return A const iterator to the beginning of the list.
    inline const_iterator_t begin() const
    {
        return options.begin();
    }

    /// @brief Returns a const iterator to the end of the list.
    /// @return A const iterator to the end of the list.
    inline const_iterator_t end() const
    {
        return options.end();
    }

    /// @brief Retrieves the length of the longest short option name.
    /// @tparam T The type to return (default is `std::size_t`).
    /// @return The length of the longest short option name.
    template <typename T = std::size_t>
    inline T getLongestShortOption() const
    {
        return static_cast<T>(longest_short_option);
    }

    /// @brief Retrieves the length of the longest long option name.
    /// @tparam T The type to return (default is `std::size_t`).
    /// @return The length of the longest long option name.
    template <typename T = std::size_t>
    inline T getLongestLongOption() const
    {
        return static_cast<T>(longest_long_option);
    }

    /// @brief Retrieves the length of the longest value.
    /// @tparam T The type to return (default is `std::size_t`).
    /// @return The length of the longest value.
    template <typename T = std::size_t>
    inline T getLongestValue() const
    {
        return static_cast<T>(longest_value);
    }

    /// @brief Updates the length of the longest value.
    /// @param length The new length to consider.
    inline void updateLongestValue(std::size_t length)
    {
        if (length > longest_value) {
            longest_value = length;
        }
    }

private:
    /// @brief The list of options.
    option_list_t options;
    /// @brief The length of the longest short option name.
    std::size_t longest_short_option;
    /// @brief The length of the longest long option name.
    std::size_t longest_long_option;
    /// @brief The length of the longest value.
    std::size_t longest_value;
};

/// @brief Specialization of `getOption` for `std::string`.
/// @param option_string The short or long name of the option.
/// @return The value of the option as a string, or an empty string if not found.
template <>
std::string OptionList::getOption(const std::string &option_string) const
{
    const Option *option = this->findOption(option_string);
    if (option) {
        const MultiOption *mopt;
        const ToggleOption *topt;
        const ValueOption *vopt;
        if ((vopt = dynamic_cast<const ValueOption *>(option))) {
            return vopt->value;
        } else if ((topt = dynamic_cast<const ToggleOption *>(option))) {
            return topt->toggled ? "true" : "false";
        } else if ((mopt = dynamic_cast<const MultiOption *>(option))) {
            return mopt->selected_value;
        }
    }
    return "";
}

} // namespace detail

} // namespace cmdlp
