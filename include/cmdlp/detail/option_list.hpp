/// @file option_list.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Defines classes for managing and storing command-line options.

#pragma once

#include <algorithm>
#include <exception>
#include <memory>
#include <sstream>
#include <vector>

#include "option.hpp"

namespace cmdlp
{

namespace detail
{

/// @class OptionExistException
/// @brief Exception thrown when attempting to add a duplicate option.
class OptionExistException : public std::exception
{
private:
    /// @brief Message describing the conflict.
    std::string msg;

public:
    /// @brief Constructs an `OptionExistException`.
    /// @param _new_option The new option that caused the conflict.
    /// @param _existing_option The existing option that conflicts with the new one.
    OptionExistException(const std::shared_ptr<Option> &_new_option, const std::shared_ptr<Option> &_existing_option)
    {
        std::stringstream ss;
        ss << "Option (" << _new_option->opt_short << ", " << _new_option->opt_long << ") already exists: ("
           << _existing_option->opt_short << ", " << _existing_option->opt_long << ")\n";
        msg = ss.str();
    }

    /// @brief Returns the error message.
    /// @return A C-string containing the error message.
    auto what() const noexcept -> const char * override { return msg.c_str(); }
};

/// @class OptionList
/// @brief Manages a list of command-line options.
class OptionList
{
public:
    /// @brief Alias for a vector of `Option` pointers.
    using option_list_t            = std::vector<std::shared_ptr<Option>>;
    /// @brief Alias for an iterator over the option list.
    using iterator_t               = std::vector<std::shared_ptr<Option>>::iterator;
    /// @brief Alias for a const iterator over the option list.
    using const_iterator_t         = std::vector<std::shared_ptr<Option>>::const_iterator;
    /// @brief Alias for a reverse iterator over the option list.
    using reverse_iterator_t       = std::vector<std::shared_ptr<Option>>::reverse_iterator;
    /// @brief Alias for a const reverse iterator over the option list.
    using const_reverse_iterator_t = std::vector<std::shared_ptr<Option>>::const_reverse_iterator;

    /// @brief Constructs an empty `OptionList`.
    OptionList() = default;

    /// @brief Copy constructor.
    /// @param other The `OptionList` to copy.
    /// @details Creates deep copies of the options in the list.
    OptionList(const OptionList &other)
        : longest_short_option(other.longest_short_option)
        , longest_long_option(other.longest_long_option)
        , longest_value(other.longest_value)
    {
        if (this != &other) {
            for (const auto &option : other.options) {
                if (auto vopt = std::dynamic_pointer_cast<ValueOption>(option)) {
                    options.emplace_back(std::make_shared<ValueOption>(*vopt));
                } else if (auto topt = std::dynamic_pointer_cast<ToggleOption>(option)) {
                    options.emplace_back(std::make_shared<ToggleOption>(*topt));
                } else if (auto mopt = std::dynamic_pointer_cast<MultiOption>(option)) {
                    options.emplace_back(std::make_shared<MultiOption>(*mopt));
                } else if (auto popt = std::dynamic_pointer_cast<PositionalOption>(option)) {
                    options.emplace_back(std::make_shared<PositionalOption>(*popt));
                }
            }
        }
    }

    /// @brief Move constructor.
    /// @param other The `OptionList` to move.
    OptionList(OptionList &&other) noexcept
        : options(std::move(other.options))
        , longest_short_option(other.longest_short_option)
        , longest_long_option(other.longest_long_option)
        , longest_value(other.longest_value)
    {
        other.longest_short_option = 0;
        other.longest_long_option  = 0;
        other.longest_value        = 0;
    }

    /// @brief Copy assignment operator.
    /// @param other The `OptionList` to copy.
    /// @return A reference to this `OptionList`.
    auto operator=(const OptionList &other) -> OptionList &
    {
        if (this != &other) {
            longest_short_option = other.longest_short_option;
            longest_long_option  = other.longest_long_option;
            longest_value        = other.longest_value;
            options.clear();
            for (const auto &option : other.options) {
                if (auto vopt = std::dynamic_pointer_cast<ValueOption>(option)) {
                    options.emplace_back(std::make_shared<ValueOption>(*vopt));
                } else if (auto topt = std::dynamic_pointer_cast<ToggleOption>(option)) {
                    options.emplace_back(std::make_shared<ToggleOption>(*topt));
                } else if (auto mopt = std::dynamic_pointer_cast<MultiOption>(option)) {
                    options.emplace_back(std::make_shared<MultiOption>(*mopt));
                } else if (auto popt = std::dynamic_pointer_cast<PositionalOption>(option)) {
                    options.emplace_back(std::make_shared<PositionalOption>(*popt));
                }
            }
        }
        return *this;
    }

    /// @brief Move assignment operator.
    /// @param other The `OptionList` to move.
    /// @return A reference to this `OptionList`.
    auto operator=(OptionList &&other) noexcept -> OptionList &
    {
        if (this != &other) {
            options                    = std::move(other.options);
            longest_short_option       = other.longest_short_option;
            longest_long_option        = other.longest_long_option;
            longest_value              = other.longest_value;
            other.longest_short_option = 0;
            other.longest_long_option  = 0;
            other.longest_value        = 0;
        }
        return *this;
    }

    /// @brief Destructor.
    /// @details Cleans up all dynamically allocated options.
    ~OptionList() = default;

    /// @brief Finds an option by its short or long name.
    /// @param option_string The short or long name of the option.
    /// @return A pointer to the `Option` if found, or `nullptr` otherwise.
    auto findOption(const std::string &option_string) const -> std::shared_ptr<Option>
    {
        for (const auto &option : options) {
            if ((option->opt_short == option_string) || (option->opt_long == option_string)) {
                return option;
            }
        }
        return nullptr;
    }

    /// @brief Checks if an option exists by its name.
    /// @param option_string The short or long name of the option.
    /// @return True if the option exists, false otherwise.
    auto optionExists(const std::string &option_string) const -> bool
    {
        return this->findOption(option_string) != nullptr;
    }

    /// @brief Retrieves the value of an option.
    /// @tparam T The expected type of the option value.
    /// @param option_string The short or long name of the option.
    /// @return The value of the option, or the default value of `T` if not found.
    template <typename T>
    auto getOption(const std::string &option_string) const -> T
    {
        const std::shared_ptr<Option> option = this->findOption(option_string);
        if (option) {
            std::stringstream ss;
            if (auto vopt = std::dynamic_pointer_cast<ValueOption>(option)) {
                ss << vopt->value;
            } else if (auto topt = std::dynamic_pointer_cast<ToggleOption>(option)) {
                ss << topt->toggled;
            } else if (auto mopt = std::dynamic_pointer_cast<MultiOption>(option)) {
                ss << mopt->selected_value;
            } else if (auto popt = std::dynamic_pointer_cast<PositionalOption>(option)) {
                ss << popt->value;
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
    void addOption(const std::shared_ptr<Option> &option)
    {
        // If the option is a separator, skip all checks.
        if (std::dynamic_pointer_cast<Separator>(option)) {
            options.push_back(option);
            return;
        }

        // Check if the option already exists in the list of options.
        for (auto &it : options) {
            if (it->opt_short == option->opt_short || it->opt_long == option->opt_long) {
                throw OptionExistException(option, it);
            }
        }

        // Add the option to the list of options.
        options.push_back(option);

        // Update the length of the `longest` parameters.
        longest_short_option = std::max(option->opt_short.length(), longest_short_option);
        longest_long_option  = std::max(option->opt_long.length(), longest_long_option);
        longest_value        = std::max(option->get_value_length(), longest_value);
    }

    /// @brief Returns a const iterator to the beginning of the list.
    /// @return A const iterator to the beginning of the list.
    auto begin() const -> const_iterator_t { return options.begin(); }

    /// @brief Returns a const iterator to the end of the list.
    /// @return A const iterator to the end of the list.
    auto end() const -> const_iterator_t { return options.end(); }

    /// @brief Returns a const reverse iterator to the beginning of the list.
    /// @return A const reverse iterator to the beginning of the list.
    auto rbegin() const -> const_reverse_iterator_t { return options.rbegin(); }

    /// @brief Returns a const reverse iterator to the end of the list.
    /// @return A const reverse iterator to the end of the list.
    auto rend() const -> const_reverse_iterator_t { return options.rend(); }

    /// @brief Retrieves the length of the longest short option name.
    /// @tparam T The type to return (default is `std::size_t`).
    /// @return The length of the longest short option name.
    template <typename T = std::size_t>
    auto getLongestShortOption() const -> T
    {
        return static_cast<T>(longest_short_option);
    }

    /// @brief Retrieves the length of the longest long option name.
    /// @tparam T The type to return (default is `std::size_t`).
    /// @return The length of the longest long option name.
    template <typename T = std::size_t>
    auto getLongestLongOption() const -> T
    {
        return static_cast<T>(longest_long_option);
    }

    /// @brief Retrieves the length of the longest value.
    /// @tparam T The type to return (default is `std::size_t`).
    /// @return The length of the longest value.
    template <typename T = std::size_t>
    auto getLongestValue() const -> T
    {
        return static_cast<T>(longest_value);
    }

    /// @brief Updates the length of the longest value.
    /// @param length The new length to consider.
    void updateLongestValue(std::size_t length) { longest_value = std::max(length, longest_value); }

private:
    /// @brief The list of options.
    option_list_t options;
    /// @brief The length of the longest short option name.
    std::size_t longest_short_option{0};
    /// @brief The length of the longest long option name.
    std::size_t longest_long_option{0};
    /// @brief The length of the longest value.
    std::size_t longest_value{0};
};

/// @brief Retrieves the value of an option.
/// @tparam T The expected type of the option value.
/// @param option_string The short or long name of the option.
/// @return The value of the option, or the default value of `T` if not found.
template <>
auto OptionList::getOption(const std::string &option_string) const -> std::vector<std::string>
{
    const std::shared_ptr<Option> option = this->findOption(option_string);
    if (option != nullptr) {
        if (auto plopt = std::dynamic_pointer_cast<PositionalList>(option)) {
            return plopt->values;
        }
    }
    return std::vector<std::string>();
}

/// @brief Specialization of `getOption` for `std::string`.
/// @param option_string The short or long name of the option.
/// @return The value of the option as a string, or an empty string if not found.
template <>
auto OptionList::getOption(const std::string &option_string) const -> std::string
{
    const std::shared_ptr<Option> option = this->findOption(option_string);
    if (option != nullptr) {
        if (auto vopt = std::dynamic_pointer_cast<ValueOption>(option)) {
            return vopt->value;
        }
        if (auto topt = std::dynamic_pointer_cast<ToggleOption>(option)) {
            return topt->toggled ? "true" : "false";
        }
        if (auto mopt = std::dynamic_pointer_cast<MultiOption>(option)) {
            return mopt->selected_value;
        }
        if (auto popt = std::dynamic_pointer_cast<PositionalOption>(option)) {
            return popt->value;
        }
    }
    return "";
}

} // namespace detail

} // namespace cmdlp
