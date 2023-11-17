/// @file option.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include <string>

namespace cmdlp
{

class Option {
public:
    /// The short version of the option.
    const std::string opt_short;
    /// The entire word for the option.
    const std::string opt_long;
    /// Description of the option.
    const std::string description;

    Option(std::string _opt_short, std::string _opt_long, std::string _description)
        : opt_short(std::move(_opt_short)),
          opt_long(std::move(_opt_long)),
          description(std::move(_description))
    {
        // Nothing to do.
    }

    virtual ~Option() = default;

    virtual std::size_t get_value_length() const = 0;
};

class ToggleOption : public Option {
public:
    /// If the option is toggled.
    bool toggled;

    ToggleOption(std::string _opt_short, std::string _opt_long, std::string _description, bool _toggled)
        : Option(_opt_short, _opt_long, _description),
          toggled(_toggled)
    {
        // Nothing to do.
    }

    virtual ~ToggleOption() = default;

    virtual std::size_t get_value_length() const
    {
        return 5;
    }
};

class ValueOption : public Option {
public:
    /// The actual value.
    std::string value;
    /// The option is required.
    bool required;

    ValueOption(std::string _opt_short, std::string _opt_long, std::string _description, std::string _value, bool _required)
        : Option(_opt_short, _opt_long, _description),
          value(std::move(_value)),
          required(_required)
    {
        // Nothing to do.
    }

    virtual ~ValueOption() = default;

    virtual std::size_t get_value_length() const
    {
        return value.size();
    }
};

} // namespace cmdlp
