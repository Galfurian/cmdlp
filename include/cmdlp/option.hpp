/// @file option.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include <string>

namespace cmdlp
{

class Option {
public:
    /// The character that identifies the option.
    std::string optc;
    /// The entire word for the option.
    std::string opts;
    /// Description of the option.
    std::string description;

    Option(std::string _optc, std::string _opts, std::string _description)
        : optc(std::move(_optc)),
          opts(std::move(_opts)),
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

    ToggleOption(std::string _optc, std::string _opts, std::string _description, bool _toggled)
        : Option(_optc, _opts, _description),
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

    ValueOption(std::string _optc, std::string _opts, std::string _description, std::string _value, bool _required)
        : Option(_optc, _opts, _description),
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

static inline std::string &optc_to_string(char c)
{
    static std::string optcs = "-*";
    optcs[1]                 = c;
    return optcs;
}

} // namespace cmdlp
