/// @file option.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Defines the Option base class and its derived classes for command-line argument parsing.

#pragma once

#include <string>

namespace cmdlp::detail
{

/// @class Option
/// @brief Base class for command-line options.
class Option {
public:
    /// @brief The short version of the option (e.g., "-o").
    const std::string opt_short;
    /// @brief The long version of the option (e.g., "--option").
    const std::string opt_long;
    /// @brief A description of the option, typically used in help messages.
    const std::string description;

    /// @brief Constructs an `Option` object.
    /// @param _opt_short The short version of the option.
    /// @param _opt_long The long version of the option.
    /// @param _description The description of the option.
    Option(std::string _opt_short, std::string _opt_long, std::string _description)
        : opt_short(std::move(_opt_short)),
          opt_long(std::move(_opt_long)),
          description(std::move(_description))
    {
        // Constructor logic (currently empty).
    }

    /// @brief Virtual destructor.
    virtual ~Option() = default;

    /// @brief Retrieves the length of the value associated with the option.
    /// @return The length of the value as a `std::size_t`.
    /// @details This method is pure virtual and must be implemented by derived classes.
    virtual std::size_t get_value_length() const = 0;
};

/// @class ToggleOption
/// @brief A command-line option that represents a toggle or flag.
class ToggleOption : public Option {
public:
    /// @brief Indicates whether the toggle is enabled or disabled.
    bool toggled;

    /// @brief Constructs a `ToggleOption` object.
    /// @param _opt_short The short version of the option (e.g., "-v").
    /// @param _opt_long The long version of the option (e.g., "--verbose").
    /// @param _description The description of the option.
    /// @param _toggled The initial state of the toggle (true = enabled, false = disabled).
    ToggleOption(std::string _opt_short, std::string _opt_long, std::string _description, bool _toggled)
        : Option(std::move(_opt_short), std::move(_opt_long), std::move(_description)),
          toggled(_toggled)
    {
        // Constructor logic (currently empty).
    }

    /// @brief Virtual destructor.
    virtual ~ToggleOption() = default;

    /// @brief Retrieves the length of the toggle value.
    /// @return A constant value of 5 (e.g., "true" or "false").
    virtual std::size_t get_value_length() const
    {
        return 5; // Length of the string "false" or "true".
    }
};

/// @class ValueOption
/// @brief A command-line option that requires an associated value.
class ValueOption : public Option {
public:
    /// @brief The value associated with the option.
    std::string value;
    /// @brief Indicates whether the option is required.
    bool required;

    /// @brief Constructs a `ValueOption` object.
    /// @param _opt_short The short version of the option (e.g., "-f").
    /// @param _opt_long The long version of the option (e.g., "--file").
    /// @param _description The description of the option.
    /// @param _value The default value for the option.
    /// @param _required Indicates whether the option is mandatory (true = required).
    ValueOption(std::string _opt_short, std::string _opt_long, std::string _description, std::string _value, bool _required)
        : Option(std::move(_opt_short), std::move(_opt_long), std::move(_description)),
          value(std::move(_value)),
          required(_required)
    {
        // Constructor logic (currently empty).
    }

    /// @brief Virtual destructor.
    virtual ~ValueOption() = default;

    /// @brief Retrieves the length of the value associated with the option.
    /// @return The length of the value as a `std::size_t`.
    virtual std::size_t get_value_length() const
    {
        return value.size();
    }
};

} // namespace cmdlp::detail
