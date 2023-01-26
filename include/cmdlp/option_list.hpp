/// @file option_list.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include "option.hpp"

#include <exception>
#include <sstream>

namespace cmdlp
{
class OptionExistException : public std::exception {
private:
    std::string msg;

public:
    OptionExistException(Option *_new_option, Option *_existing_option)
        : std::exception()
    {
        std::stringstream ss;
        ss << "Option (" << _new_option->optc << ", " << _new_option->opts << ") already exists: "
           << "(" << _existing_option->optc << ", " << _existing_option->opts << ")\n";
        msg = ss.str();
    }

    const char *what() const noexcept
    {
        return msg.c_str();
    }
};

class OptionList {
public:
    typedef std::vector<Option *> option_list_t;
    typedef std::vector<Option *>::iterator iterator_t;
    typedef std::vector<Option *>::const_iterator const_iterator_t;

    /// @brief Construct a new Option List object.
    OptionList()
        : options(),
          longest_option(),
          longest_value()
    {
    }

    OptionList(const OptionList &other)
        : options(),
          longest_option(other.longest_option),
          longest_value(other.longest_value)
    {
        for (const_iterator_t it = other.options.begin(); it != other.options.end(); ++it) {
            // First try to check if it is a value option.
            const ValueOption *vopt = dynamic_cast<const ValueOption *>(*it);
            if (vopt) {
                options.emplace_back(new ValueOption(*vopt));
            } else {
                // Then, check if it is a toggle option.
                const ToggleOption *topt = dynamic_cast<const ToggleOption *>(*it);
                if (topt) {
                    options.emplace_back(new ToggleOption(*topt));
                }
            }
        }
    }

    virtual ~OptionList()
    {
        for (iterator_t it = options.begin(); it != options.end(); ++it)
            delete *it;
    }

    inline bool optionExhists(Option *option) const
    {
        return this->optionExhists(option->optc, option->opts);
    }

    inline bool optionExhists(char optc) const
    {
        return this->optionExhists(cmdlp::optc_to_string(optc), "");
    }

    inline bool optionExhists(const std::string &opts) const
    {
        return this->optionExhists("", opts);
    }

    inline Option *findOption(char optc) const
    {
        return this->findOption(cmdlp::optc_to_string(optc), "");
    }

    inline Option *findOption(const std::string &opts) const
    {
        return this->findOption("", opts);
    }

    template <typename T>
    inline T getOption(Option *option) const
    {
        return this->getOption<T>(option->optc, option->opts);
    }

    template <typename T>
    inline T getOption(char optc) const
    {
        return this->getOption<T>(cmdlp::optc_to_string(optc), "");
    }

    template <typename T>
    inline T getOption(const std::string &opts) const
    {
        return this->getOption<T>("", opts);
    }

    inline void addOption(Option *option)
    {
        for (iterator_t it = options.begin(); it != options.end(); ++it) {
            if ((*it)->optc == option->optc)
                throw OptionExistException(option, *it);
            if ((*it)->opts == option->opts)
                throw OptionExistException(option, *it);
        }
        options.push_back(option);
        if (option->opts.length() > longest_option)
            longest_option = option->opts.length();
        if (option->get_value_length() > longest_value)
            longest_value = option->get_value_length();
    }

    inline const_iterator_t begin() const
    {
        return options.begin();
    }

    inline const_iterator_t end() const
    {
        return options.end();
    }

    template <typename T = std::size_t>
    inline T getLongestOption() const
    {
        return static_cast<T>(longest_option);
    }

    template <typename T = std::size_t>
    inline T getLongestValue() const
    {
        return static_cast<T>(longest_value);
    }

    inline void updateLongestValue(std::size_t length)
    {
        if (length > longest_value)
            longest_value = length;
    }

private:
    inline bool optionExhists(const std::string &optc, const std::string &opts) const
    {
        for (const_iterator_t it = options.begin(); it != options.end(); ++it) {
            if (!optc.empty() && ((*it)->optc == optc))
                return true;
            if (!opts.empty() && ((*it)->opts == opts))
                return true;
        }
        return false;
    }

    Option *findOption(const std::string &optc, const std::string &opts) const
    {
        for (const_iterator_t it = options.begin(); it != options.end(); ++it) {
            if (!optc.empty() && ((*it)->optc == optc))
                return *it;
            if (!opts.empty() && ((*it)->opts == opts))
                return *it;
        }
        return NULL;
    }

    template <typename T>
    T getOption(const std::string &optc, const std::string &opts) const
    {
        Option *option = this->findOption(optc, opts);
        if (option) {
            // Create a stringstream to parse the values.
            std::stringstream ss;
            // First try to check if it is a value option.
            ValueOption *vopt = dynamic_cast<ValueOption *>(option);
            if (vopt) {
                ss << vopt->value;
            } else {
                // Then, check if it is a toggle option.
                ToggleOption *topt = dynamic_cast<ToggleOption *>(option);
                if (topt) {
                    ss << topt->toggled;
                }
            }
            // Parse the data.
            T data;
            ss >> data;
            return data;
        }
        return T(0);
    }

    option_list_t options;
    std::size_t longest_option;
    std::size_t longest_value;
};

template <>
std::string OptionList::getOption(const std::string &optc, const std::string &opts) const
{
    Option *option = this->findOption(optc, opts);
    if (option) {
        ValueOption *vopt = dynamic_cast<ValueOption *>(option);
        if (vopt)
            return vopt->value;
        ToggleOption *topt = dynamic_cast<ToggleOption *>(option);
        if (topt)
            return topt->toggled ? "true" : "false";
    }
    return "";
}

} // namespace cmdlp