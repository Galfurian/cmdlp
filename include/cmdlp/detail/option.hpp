/// @file option.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Defines the Option base class and its derived classes for command-line argument parsing.

#pragma once

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace cmdlp
{

/// @brief A list of multiple values.
using Values = std::vector<std::string>;

namespace detail
{

/// @class Option
/// @brief Base class for command-line options.
class Option
{
public:
    /// @brief The short version of the option (e.g., "-o").
    std::string opt_short;
    /// @brief The long version of the option (e.g., "--option").
    std::string opt_long;
    /// @brief A description of the option, typically used in help messages.
    std::string description;

    /// @brief Constructs an `Option` object.
    /// @param _opt_short The short version of the option (should start with '-').
    /// @param _opt_long The long version of the option (should start with '--').
    /// @param _description The description of the option.
    Option(std::string _opt_short, std::string _opt_long, std::string _description)
        : opt_short(std::move(_opt_short))
        , opt_long(std::move(_opt_long))
        , description(std::move(_description))
    {
        if (!opt_short.empty() && opt_short[0] != '-') {
            throw std::invalid_argument("Short option '" + opt_short + "' must start with '-'");
        }
        if (!opt_long.empty() && (opt_long.size() < 2 || opt_long.substr(0, 2) != "--")) {
            throw std::invalid_argument("Long option '" + opt_long + "' must start with '--'");
        }
    }

    /// @brief Copy constructor.
    /// @param other The other entity to copy.
    Option(const Option &other) = default;

    /// @brief Copy assignment operator.
    /// @param other The other entity to copy.
    /// @return A reference to this object.
    auto operator=(const Option &other) -> Option & = default;

    /// @brief Move constructor.
    /// @param other The other entity to move.
    Option(Option &&other) noexcept = default;

    /// @brief Move assignment operator.
    /// @param other The other entity to move.
    /// @return A reference to this object.
    auto operator=(Option &&other) noexcept -> Option & = default;

    /// @brief Virtual destructor.
    virtual ~Option() = default;

    /// @brief Retrieves the length of the value associated with the option.
    /// @return The length of the value as a `std::size_t`.
    /// @details This method is pure virtual and must be implemented by derived classes.
    virtual auto get_value_length() const -> std::size_t = 0;
};

/// @class ValueOption
/// @brief A command-line option that requires an associated value.
class ValueOption : public Option
{
public:
    /// @brief Indicates whether the option is required.
    bool required;
    /// @brief The value associated with the option.
    std::string value;

    /// @brief Constructs a `ValueOption` object.
    /// @param _opt_short The short version of the option (e.g., "-f").
    /// @param _opt_long The long version of the option (e.g., "--file").
    /// @param _description The description of the option.
    /// @param _required Indicates whether the option is mandatory (true = required).
    /// @param _value The default value for the option.
    ValueOption(
        std::string _opt_short,
        std::string _opt_long,
        std::string _description,
        bool _required,
        std::string _value)
        : Option(std::move(_opt_short), std::move(_opt_long), std::move(_description))
        , required(_required)
        , value(std::move(_value))
    {
        // Constructor logic (currently empty).
    }

    /// @brief Copy constructor.
    /// @param other The other entity to copy.
    ValueOption(const ValueOption &other) = default;

    /// @brief Copy assignment operator.
    /// @param other The other entity to copy.
    /// @return A reference to this object.
    auto operator=(const ValueOption &other) -> ValueOption & = default;

    /// @brief Move constructor.
    /// @param other The other entity to move.
    ValueOption(ValueOption &&other) noexcept = default;

    /// @brief Move assignment operator.
    /// @param other The other entity to move.
    /// @return A reference to this object.
    auto operator=(ValueOption &&other) noexcept -> ValueOption & = default;

    /// @brief Virtual destructor.
    ~ValueOption() override = default;

    /// @brief Retrieves the length of the value associated with the option.
    /// @return The length of the value as a `std::size_t`.
    auto get_value_length() const -> std::size_t override { return value.size(); }
};

/// @class ToggleOption
/// @brief A command-line option that represents a toggle or flag.
class ToggleOption : public Option
{
public:
    /// @brief Indicates whether the toggle is enabled or disabled.
    bool toggled;

    /// @brief Constructs a `ToggleOption` object.
    /// @param _opt_short The short version of the option (e.g., "-v").
    /// @param _opt_long The long version of the option (e.g., "--verbose").
    /// @param _description The description of the option.
    /// @param _toggled The initial state of the toggle (true = enabled, false = disabled).
    ToggleOption(std::string _opt_short, std::string _opt_long, std::string _description, bool _toggled)
        : Option(std::move(_opt_short), std::move(_opt_long), std::move(_description))
        , toggled(_toggled)
    {
        // Nothing to do.
    }

    /// @brief Copy constructor.
    /// @param other The other entity to copy.
    ToggleOption(const ToggleOption &other) = default;

    /// @brief Copy assignment operator.
    /// @param other The other entity to copy.
    /// @return A reference to this object.
    auto operator=(const ToggleOption &other) -> ToggleOption & = default;

    /// @brief Move constructor.
    /// @param other The other entity to move.
    ToggleOption(ToggleOption &&other) noexcept = default;

    /// @brief Move assignment operator.
    /// @param other The other entity to move.
    /// @return A reference to this object.
    auto operator=(ToggleOption &&other) noexcept -> ToggleOption & = default;

    /// @brief Virtual destructor.
    ~ToggleOption() override = default;

    /// @brief Retrieves the length of the toggle value.
    /// @return A constant value of 5 (e.g., "true" or "false").
    auto get_value_length() const -> std::size_t override
    {
        return 5; // Length of the string "false" or "true".
    }
};

/// @class MultiOption
/// @brief A command-line option that allows selecting from a predefined set of values.
class MultiOption : public Option
{
public:
    /// @brief The set of allowed values for this option.
    Values allowed_values;
    /// @brief The selected value for this option.
    std::string selected_value;

    /// @brief Constructs a `MultiOption` object.
    /// @param _opt_short The short version of the option (e.g., "-m").
    /// @param _opt_long The long version of the option (e.g., "--mode").
    /// @param _description The description of the option.
    /// @param _allowed_values The set of allowed values for the option.
    /// @param _value The default value for the option.
    MultiOption(
        std::string _opt_short,
        std::string _opt_long,
        std::string _description,
        Values _allowed_values,
        std::string _value)
        : Option(std::move(_opt_short), std::move(_opt_long), std::move(_description))
        , allowed_values(std::move(_allowed_values))
        , selected_value(std::move(_value))
    {
        if (!this->isValueAllowed(selected_value)) {
            std::ostringstream oss;
            oss << "Value \"" << selected_value << "\" is not in the list of allowed values: " << print_list();
            throw std::invalid_argument(oss.str());
        }
    }

    /// @brief Copy constructor.
    /// @param other The other entity to copy.
    MultiOption(const MultiOption &other) = default;

    /// @brief Copy assignment operator.
    /// @param other The other entity to copy.
    /// @return A reference to this object.
    auto operator=(const MultiOption &other) -> MultiOption & = default;

    /// @brief Move constructor.
    /// @param other The other entity to move.
    MultiOption(MultiOption &&other) noexcept = default;

    /// @brief Move assignment operator.
    /// @param other The other entity to move.
    /// @return A reference to this object.
    auto operator=(MultiOption &&other) noexcept -> MultiOption & = default;

    /// @brief Virtual destructor.
    ~MultiOption() override = default;

    /// @brief Sets the selected value for this option.
    /// @param value The value to set.
    /// @throws std::invalid_argument if the value is not in the list of allowed values.
    void setValue(const std::string &value)
    {
        if (!this->isValueAllowed(value)) {
            std::ostringstream oss;
            oss << "Value \"" << value << "\" is not in the list of allowed values: " << print_list();
            throw std::invalid_argument(oss.str());
        }
        selected_value = value;
    }

    /// @brief Retrieves the length of the selected value.
    /// @return The length of the selected value as a `std::size_t`.
    auto get_value_length() const -> std::size_t override
    {
        std::size_t max_length = 0;
        for (const auto &value : allowed_values) {
            max_length = std::max(value.size(), max_length);
        }
        return max_length;
    }

    /// @brief Prints the list of allowed values.
    /// @return A formatted string containing all allowed values.
    auto print_list() const -> std::string
    {
        std::ostringstream oss;
        for (size_t i = 0; i < allowed_values.size(); ++i) {
            oss << allowed_values[i];
            if (i < allowed_values.size() - 1) {
                oss << ", ";
            }
        }
        return oss.str();
    }

private:
    /// @brief Checks if a value is allowed.
    /// @param value The value to check.
    /// @return True if the value is in the allowed values, false otherwise.
    auto isValueAllowed(const std::string &value) const -> bool
    {
        return std::find(allowed_values.begin(), allowed_values.end(), value) != allowed_values.end();
    }
};

/// @class PositionalOption
/// @brief Represents a positional argument in the command-line input.
class PositionalOption : public Option
{
public:
    /// @brief Indicates whether the argument is required.
    bool required;
    /// @brief The value provided for the positional argument.
    std::string value;

    /// @brief Constructs a `PositionalOption` object.
    /// @param _opt_short The short version of the option (e.g., "-m").
    /// @param _opt_long The long version of the option (e.g., "--mode").
    /// @param _description A description of the positional argument.
    /// @param _required Whether the argument is mandatory.
    /// @param _value The default value for the option.
    PositionalOption(
        std::string _opt_short,
        std::string _opt_long,
        std::string _description,
        bool _required,
        std::string _value)
        : Option(std::move(_opt_short), std::move(_opt_long), std::move(_description))
        , required(_required)
        , value(std::move(_value))
    {
        // Nothing to do.
    }

    /// @brief Copy constructor.
    /// @param other The other entity to copy.
    PositionalOption(const PositionalOption &other) = default;

    /// @brief Copy assignment operator.
    /// @param other The other entity to copy.
    /// @return A reference to this object.
    auto operator=(const PositionalOption &other) -> PositionalOption & = default;

    /// @brief Move constructor.
    /// @param other The other entity to move.
    PositionalOption(PositionalOption &&other) noexcept = default;

    /// @brief Move assignment operator.
    /// @param other The other entity to move.
    /// @return A reference to this object.
    auto operator=(PositionalOption &&other) noexcept -> PositionalOption & = default;

    /// @brief Virtual destructor.
    ~PositionalOption() override = default;

    /// @brief Retrieves the length of the positional argument's value.
    /// @return The length of the value as a `std::size_t`.
    auto get_value_length() const -> std::size_t override { return value.size(); }
};

/// @class PositionalList
/// @brief Represents a list of positional arguments in the command-line input.
class PositionalList : public Option
{
public:
    /// @brief Indicates whether the argument is required.
    bool required;
    /// @brief The values.
    Values values;

    /// @brief Constructs a `PositionalOption` object.
    /// @param _opt_short The short version of the option (e.g., "-m").
    /// @param _opt_long The long version of the option (e.g., "--mode").
    /// @param _description A description of the positional argument.
    /// @param _required Whether the argument is mandatory.
    PositionalList(std::string _opt_short, std::string _opt_long, std::string _description, bool _required)
        : Option(std::move(_opt_short), std::move(_opt_long), std::move(_description))
        , required(_required)
        , values()
    {
        // Nothing to do.
    }

    /// @brief Copy constructor.
    /// @param other The other entity to copy.
    PositionalList(const PositionalList &other) = default;

    /// @brief Copy assignment operator.
    /// @param other The other entity to copy.
    /// @return A reference to this object.
    auto operator=(const PositionalList &other) -> PositionalList & = default;

    /// @brief Move constructor.
    /// @param other The other entity to move.
    PositionalList(PositionalList &&other) noexcept = default;

    /// @brief Move assignment operator.
    /// @param other The other entity to move.
    /// @return A reference to this object.
    auto operator=(PositionalList &&other) noexcept -> PositionalList & = default;

    /// @brief Virtual destructor.
    ~PositionalList() override = default;

    /// @brief Retrieves the length of the positional argument's value.
    /// @return The length of the value as a `std::size_t`.
    auto get_value_length() const -> std::size_t override
    {
        std::size_t max_length = 0;
        for (const auto &value : values) {
            max_length = std::max(max_length, value.size());
        }
        return max_length;
    }

    /// @brief Prints the list of values.
    /// @return A formatted string containing all the values.
    auto print_values() const -> std::string
    {
        std::ostringstream oss;
        for (size_t i = 0; i < values.size(); ++i) {
            oss << values[i];
            if (i < values.size() - 1) {
                oss << ", ";
            }
        }
        return oss.str();
    }
};

/// @class Separator
/// @brief A special type of option used for grouping and labeling sections in help messages.
class Separator : public Option
{
public:
    /// @brief Constructs a `Separator` object.
    /// @param _description The description of the separator (e.g., a section title).
    explicit Separator(std::string _description)
        : Option("", "", std::move(_description))
    {
        // Nothing to do.
    }

    /// @brief Copy constructor.
    /// @param other The other entity to copy.
    Separator(const Separator &other) = default;

    /// @brief Copy assignment operator.
    /// @param other The other entity to copy.
    /// @return A reference to this object.
    auto operator=(const Separator &other) -> Separator & = default;

    /// @brief Move constructor.
    /// @param other The other entity to move.
    Separator(Separator &&other) noexcept = default;

    /// @brief Move assignment operator.
    /// @param other The other entity to move.
    /// @return A reference to this object.
    auto operator=(Separator &&other) noexcept -> Separator & = default;

    /// @brief Virtual destructor.
    ~Separator() override = default;

    /// @brief Retrieves the length of the value associated with the separator.
    /// @return Always returns 0 as separators do not have values.
    auto get_value_length() const -> std::size_t override { return 0; }
};

} // namespace detail

} // namespace cmdlp
