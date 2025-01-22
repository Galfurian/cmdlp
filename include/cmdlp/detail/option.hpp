/// @file option.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief Defines the Option base class and its derived classes for command-line argument parsing.

#pragma once

#include <stdexcept>
#include <sstream>
#include <string>

namespace cmdlp
{

namespace detail
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

/// @class MultiOption
/// @brief A command-line option that allows selecting from a predefined set of values.
class MultiOption : public Option {
public:
    /// @brief The set of allowed values for this option.
    const std::vector<std::string> allowed_values;
    /// @brief The selected value for this option.
    std::string selected_value;

    /// @brief Constructs a `MultiOption` object.
    /// @param _opt_short The short version of the option (e.g., "-m").
    /// @param _opt_long The long version of the option (e.g., "--mode").
    /// @param _description The description of the option.
    /// @param _allowed_values The set of allowed values for the option.
    /// @param _default_value The default value for the option.
    MultiOption(std::string _opt_short, std::string _opt_long, std::string _description, std::vector<std::string> _allowed_values, std::string _default_value)
        : Option(std::move(_opt_short), std::move(_opt_long), std::move(_description)),
          allowed_values(std::move(_allowed_values)),
          selected_value(std::move(_default_value))
    {
        if (!this->isValueAllowed(selected_value)) {
            std::ostringstream oss;
            oss << "Value \"" << selected_value << "\" is not in the list of allowed values: " << print_list();
            throw std::invalid_argument(oss.str());
        }
    }

    /// @brief Virtual destructor.
    virtual ~MultiOption() = default;

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
    virtual std::size_t get_value_length() const override
    {
        std::size_t max_length = 0;
        for (const auto &value : allowed_values) {
            if (value.size() > max_length) {
                max_length = value.size();
            }
        }
        return max_length;
    }

    /// @brief Prints the list of allowed values.
    /// @return A formatted string containing all allowed values.
    std::string print_list() const
    {
        std::ostringstream oss;
        oss << "[";
        for (size_t i = 0; i < allowed_values.size(); ++i) {
            oss << allowed_values[i];
            if (i < allowed_values.size() - 1) {
                oss << ", ";
            }
        }
        oss << "]";
        return oss.str();
    }

private:
    /// @brief Checks if a value is allowed.
    /// @param value The value to check.
    /// @return True if the value is in the allowed values, false otherwise.
    bool isValueAllowed(const std::string &value) const
    {
        return std::find(allowed_values.begin(), allowed_values.end(), value) != allowed_values.end();
    }
};

/// @class Separator
/// @brief A special type of option used for grouping and labeling sections in help messages.
class Separator : public Option {
public:
    /// @brief Constructs a `Separator` object.
    /// @param _description The description of the separator (e.g., a section title).
    explicit Separator(std::string _description)
        : Option("", "", std::move(_description))
    {
    }

    /// @brief Retrieves the length of the value associated with the separator.
    /// @return Always returns 0 as separators do not have values.
    virtual std::size_t get_value_length() const override
    {
        return 0;
    }
};

} // namespace detail

} // namespace cmdlp
