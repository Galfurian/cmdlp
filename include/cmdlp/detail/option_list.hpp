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

/// @class BadConversion
/// @brief Exception thrown when a value cannot be converted to the requested type.
class BadConversion : public std::runtime_error
{
public:
    /// @brief Constructs a BadConversion exception.
    /// @param message The error message.
    explicit BadConversion(const std::string &message)
        : std::runtime_error(message)
    {
    }
};

/// @class OptionList

/// @brief Manages a list of command-line options.
class OptionList
{
public:
    /// @brief Alias for a vector of `Option` pointers.
    using option_list_t = std::vector<std::shared_ptr<Option>>;

    /// @brief Constructs an empty `OptionList`.
    OptionList() = default;

    /// @brief Copy constructor.
    /// @param other The `OptionList` to copy.
    /// @details Creates deep copies of the options in the list.
    OptionList(const OptionList &other) = delete;

    /// @brief Move constructor.
    /// @param other The `OptionList` to move.
    OptionList(OptionList &&other) noexcept = default;

    /// @brief Copy assignment operator.
    /// @param other The `OptionList` to copy.
    /// @return A reference to this `OptionList`.
    auto operator=(const OptionList &other) -> OptionList & = delete;

    /// @brief Move assignment operator.
    /// @param other The `OptionList` to move.
    /// @return A reference to this `OptionList`.
    auto operator=(OptionList &&other) noexcept -> OptionList & = default;

    /// @brief Destructor.
    /// @details Cleans up all dynamically allocated options.
    ~OptionList() = default;

    /// @brief Checks if an option exists by its name.
    /// @param option_string The short or long name of the option.
    /// @return True if the option exists, false otherwise.
    auto optionExists(const std::string &option_string) const -> bool { return this->find(option_string) != nullptr; }

    /// @brief Retrieves the value of an option.
    /// @tparam T The expected type of the option value.
    /// @param option_string The short or long name of the option.
    /// @return The value of the option, or the default value of `T` if not found.
    template <typename T>
    auto getOption(const std::string &option_string) const -> T
    {
        const std::shared_ptr<Option> option = this->find(option_string);
        if (option) {
            std::stringstream ss;
            std::string value_str;
            if (auto vopt = std::dynamic_pointer_cast<ValueOption>(option)) {
                value_str = vopt->value;
            } else if (auto topt = std::dynamic_pointer_cast<ToggleOption>(option)) {
                value_str = (topt->toggled ? "true" : "false");
            } else if (auto mopt = std::dynamic_pointer_cast<MultiOption>(option)) {
                value_str = mopt->value;
            } else if (auto popt = std::dynamic_pointer_cast<PositionalOption>(option)) {
                value_str = popt->value;
            } else {
                // This case should ideally not be reached if all Option types are handled
                // or if the option_string doesn't correspond to a value-holding option.
                // For now, return default or throw an error if no value can be extracted.
                // Returning T() might be misleading if the option exists but has no convertible value.
                // Throwing an exception is more explicit.
                std::stringstream error_msg;
                error_msg << "Option '" << option_string << "' does not hold a convertible value.";
                throw BadConversion(error_msg.str());
            }

            ss << value_str;
            T data;
            ss >> data;
            if (ss.fail() || !ss.eof()) { // Check for conversion failure or leftover characters
                std::stringstream error_msg;
                error_msg << "Failed to convert value '" << value_str << "' to requested type.";
                throw BadConversion(error_msg.str());
            }
            return data;
        }
        // If option not found, throw an exception.
        std::stringstream error_msg;
        error_msg << "Option '" << option_string << "' not found.";
        throw std::out_of_range(error_msg.str());
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

    /// @brief Filters options based on a provided predicate.
    /// @tparam Predicate The type of the callable predicate.
    /// @param predicate A callable that takes a std::shared_ptr<Option> and returns a bool.
    /// @return A vector of options that satisfy the predicate.
    /// @details
    /// Here is an example of filter:
    /// @code {cpp}
    /// auto required_value_options = optionList.filter([](const std::shared_ptr<Option>& option) {
    ///     auto value_option = std::dynamic_pointer_cast<ValueOption>(option);
    ///     return value_option && value_option->required;
    /// });
    /// @endcode
    template <typename Predicate>
    auto filter(Predicate predicate) const -> option_list_t
    {
        option_list_t result;
        for (const auto &option : options) {
            if (predicate(option)) {
                result.push_back(option);
            }
        }
        return result;
    }

    /// @brief Checks if there are no options.
    /// @return True if there are no options, false otherwise.
    auto empty() const -> bool { return options.empty(); }

    /// @brief Returns the number of options in the option list.
    /// @return The total count of options.
    auto size() const -> std::size_t { return options.size(); }

    /// @brief Removes all options from the option list.
    void clear() { options.clear(); }

    /// @brief Finds an option by its short or long name.
    /// @param option The short or long name of the option.
    /// @return A pointer to the `Option` if found, or `nullptr` otherwise.
    auto find(const std::string &option) const -> std::shared_ptr<Option>
    {
        for (const auto &entry : options) {
            if ((entry->opt_short == option) || (entry->opt_long == option)) {
                return entry;
            }
        }
        return nullptr;
    }

    /// @brief Provides non-const access to the option at the specified index.
    /// @param index The index of the option to access.
    /// @return Reference to the option at the specified index.
    /// @note No bounds checking is performed. Use at() for bounds-checked access.
    auto operator[](std::size_t index) -> std::shared_ptr<Option> { return options[index]; }

    /// @brief Provides const access to the token at the specified index.
    /// @param index The index of the token to access.
    /// @return Const reference to the token at the specified index.
    /// @note No bounds checking is performed. Use at() for bounds-checked access.
    auto operator[](std::size_t index) const -> const std::shared_ptr<Option> & { return options[index]; }

    /// @brief Provides bounds-checked access to a token at a specific index.
    /// @param index The index of the token.
    /// @return Reference to the token at the specified index.
    /// @throws std::out_of_range if the index is out of bounds.
    auto at(std::size_t index) -> std::shared_ptr<Option> { return options.at(index); }

    /// @brief Provides const bounds-checked access to a token at a specific index.
    /// @param index The index of the token.
    /// @return Const reference to the token at the specified index.
    /// @throws std::out_of_range if the index is out of bounds.
    auto at(std::size_t index) const -> const std::shared_ptr<Option> & { return options.at(index); }

    /// @brief Returns a reference to the first token.
    /// @return Reference to the first token.
    auto front() -> std::shared_ptr<Option> { return options.front(); }

    /// @brief Returns a const reference to the first token.
    /// @return Const reference to the first token.
    auto front() const -> const std::shared_ptr<Option> & { return options.front(); }

    /// @brief Returns a reference to the last token.
    /// @return Reference to the last token.
    auto back() -> std::shared_ptr<Option> { return options.back(); }

    /// @brief Returns a const reference to the last token.
    /// @return Const reference to the last token.
    auto back() const -> const std::shared_ptr<Option> & { return options.back(); }

    /// @brief Returns an iterator to the beginning of the options.
    /// @return Iterator to the first element.
    auto begin() -> option_list_t::iterator { return options.begin(); }

    /// @brief Returns an iterator to the end of the options.
    /// @return Iterator to one past the last element.
    auto end() -> option_list_t::iterator { return options.end(); }

    /// @brief Returns a reverse iterator to the beginning (last element) of the options.
    /// @return Reverse iterator to the last element.
    auto rbegin() -> option_list_t::reverse_iterator { return options.rbegin(); }

    /// @brief Returns a reverse iterator to the end (before first element) of the options.
    /// @return Reverse iterator to one before the first element.
    auto rend() -> option_list_t::reverse_iterator { return options.rend(); }

    /// @brief Returns a const iterator to the beginning of the options.
    /// @return Const iterator to the first element.
    auto begin() const -> option_list_t::const_iterator { return options.begin(); }

    /// @brief Returns a const iterator to the end of the options.
    /// @return Const iterator to one past the last element.
    auto end() const -> option_list_t::const_iterator { return options.end(); }

    /// @brief Returns a const reverse iterator to the beginning (last element) of the options.
    /// @return Const reverse iterator to the last element.
    auto rbegin() const -> option_list_t::const_reverse_iterator { return options.rbegin(); }

    /// @brief Returns a const reverse iterator to the end (before first element) of the options.
    /// @return Const reverse iterator to one before the first element.
    auto rend() const -> option_list_t::const_reverse_iterator { return options.rend(); }

    /// @brief Returns a const iterator to the beginning of the options (for const-qualified objects).
    /// @return Const iterator to the first element.
    auto cbegin() const -> option_list_t::const_iterator { return options.cbegin(); }

    /// @brief Returns a const iterator to the end of the options (for const-qualified objects).
    /// @return Const iterator to one past the last element.
    auto cend() const -> option_list_t::const_iterator { return options.cend(); }

    /// @brief Returns a const reverse iterator to the beginning of the options (for const-qualified objects).
    /// @return Const reverse iterator to the last element.
    auto crbegin() const -> option_list_t::const_reverse_iterator { return options.crbegin(); }

    /// @brief Returns a const reverse iterator to the end of the options (for const-qualified objects).
    /// @return Const reverse iterator to one before the first element.
    auto crend() const -> option_list_t::const_reverse_iterator { return options.crend(); }

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
    const std::shared_ptr<Option> option = this->find(option_string);
    if (option != nullptr) {
        if (auto plopt = std::dynamic_pointer_cast<PositionalList>(option)) {
            return plopt->values;
        }
    }
    // If option not found, throw an exception.
    std::stringstream error_msg;
    error_msg << "Option '" << option_string << "' not found.";
    throw std::out_of_range(error_msg.str());
}

/// @brief Specialization of `getOption` for `std::string`.
/// @param option_string The short or long name of the option.
/// @return The value of the option as a string, or an empty string if not found.
template <>
auto OptionList::getOption(const std::string &option_string) const -> std::string
{
    const std::shared_ptr<Option> option = this->find(option_string);
    if (option != nullptr) {
        if (auto vopt = std::dynamic_pointer_cast<ValueOption>(option)) {
            return vopt->value;
        }
        if (auto topt = std::dynamic_pointer_cast<ToggleOption>(option)) {
            return topt->toggled ? "true" : "false";
        }
        if (auto mopt = std::dynamic_pointer_cast<MultiOption>(option)) {
            return mopt->value;
        }
        if (auto popt = std::dynamic_pointer_cast<PositionalOption>(option)) {
            return popt->value;
        }
    }
    // If option not found, throw an exception.
    std::stringstream error_msg;
    error_msg << "Option '" << option_string << "' not found.";
    throw std::out_of_range(error_msg.str());
}

/// @brief Specialization of `getOption` for `bool`.
/// @param option_string The short or long name of the option.
/// @return The value of the option as a bool.
/// @throws BadConversion if the value is not "true" or "false".
template <>
auto OptionList::getOption(const std::string &option_string) const -> bool
{
    const std::shared_ptr<Option> option = this->find(option_string);
    if (option != nullptr) {
        std::string value_str;
        if (auto topt = std::dynamic_pointer_cast<ToggleOption>(option)) {
            value_str = (topt->toggled ? "true" : "false");
        } else if (auto vopt = std::dynamic_pointer_cast<ValueOption>(option)) {
            value_str = vopt->value;
        } else {
            std::stringstream error_msg;
            error_msg << "Option '" << option_string << "' does not hold a convertible boolean value.";
            throw BadConversion(error_msg.str());
        }

        if (value_str == "true") {
            return true;
        } else if (value_str == "false") {
            return false;
        } else {
            std::stringstream error_msg;
            error_msg << "Failed to convert value '" << value_str << "' to bool. Expected 'true' or 'false'.";
            throw BadConversion(error_msg.str());
        }
    }
    // If option not found, throw an exception.
    std::stringstream error_msg;
    error_msg << "Option '" << option_string << "' not found.";
    throw std::out_of_range(error_msg.str());
}

} // namespace detail

} // namespace cmdlp
