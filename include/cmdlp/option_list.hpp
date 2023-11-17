/// @file option_list.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include "option.hpp"

#include <exception>
#include <sstream>
#include <vector>

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
        ss << "Option (" << _new_option->opt_short << ", " << _new_option->opt_long << ") already exists: "
           << "(" << _existing_option->opt_short << ", " << _existing_option->opt_long << ")\n";
        msg = ss.str();
    }

    const char *what() const noexcept
    {
        return msg.c_str();
    }
};

class OptionList {
public:
    using option_list_t    = std::vector<Option *>;
    using iterator_t       = std::vector<Option *>::iterator;
    using const_iterator_t = std::vector<Option *>::const_iterator;

    /// @brief Construct a new Option List object.
    OptionList()
        : options(),
          longest_option(),
          longest_value()
    {
        // Nothing to do.
    }

    OptionList(const OptionList &other)
        : options(),
          longest_option(other.longest_option),
          longest_value(other.longest_value)
    {
        const ToggleOption *topt;
        const ValueOption *vopt;
        for (const_iterator_t it = other.options.begin(); it != other.options.end(); ++it) {
            // First try to check if it is a value option.
            vopt = dynamic_cast<const ValueOption *>(*it);
            if (vopt) {
                options.emplace_back(new ValueOption(*vopt));
            } else {
                // Then, check if it is a toggle option.
                topt = dynamic_cast<const ToggleOption *>(*it);
                if (topt) {
                    options.emplace_back(new ToggleOption(*topt));
                }
            }
        }
    }

    virtual ~OptionList()
    {
        for (iterator_t it = options.begin(); it != options.end(); ++it) {
            delete *it;
        }
    }

    inline bool optionExhists(Option *option) const
    {
        return this->findOption(option) != nullptr;
    }

    inline bool optionExhists(const std::string &option_string) const
    {
        return this->findOption(option_string) != nullptr;
    }

    inline const Option *findOption(Option *option) const
    {
        const Option *_option = this->findOption(option->opt_short);
        if (_option == nullptr) {
            _option = this->findOption(option->opt_long);
        }
        return _option;
    }

    inline const Option *findOption(const std::string &option_string) const
    {
        for (const_iterator_t it = options.begin(); it != options.end(); ++it) {
            if (((*it)->opt_short == option_string) || ((*it)->opt_long == option_string)) {
                return *it;
            }
        }
        return nullptr;
    }

    template <typename T>
    inline T getOption(Option *option) const
    {
        return this->getOption<T>(option->opt_short, option->opt_long);
    }

    template <typename T>
    inline T getOption(const std::string &option_string) const
    {
        const Option *option = this->findOption(option_string);
        if (option) {
            // Create a stringstream to parse the values.
            std::stringstream ss;
            // First try to check if it is a value option.
            const ValueOption *vopt = dynamic_cast<const ValueOption *>(option);
            if (vopt) {
                ss << vopt->value;
            } else {
                // Then, check if it is a toggle option.
                const ToggleOption *topt = dynamic_cast<const ToggleOption *>(option);
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

    inline void addOption(Option *option)
    {
        for (iterator_t it = options.begin(); it != options.end(); ++it) {
            if ((*it)->opt_short == option->opt_short) {
                throw OptionExistException(option, *it);
            }
            if ((*it)->opt_long == option->opt_long) {
                throw OptionExistException(option, *it);
            }
        }
        options.push_back(option);
        if (option->opt_long.length() > longest_option) {
            longest_option = option->opt_long.length();
        }
        if (option->get_value_length() > longest_value) {
            longest_value = option->get_value_length();
        }
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
        if (length > longest_value) {
            longest_value = length;
        }
    }

private:
    option_list_t options;
    std::size_t longest_option;
    std::size_t longest_value;
};

template <>
std::string OptionList::getOption(const std::string &option_string) const
{
    const Option *option = this->findOption(option_string);
    if (option) {
        const ValueOption *vopt = dynamic_cast<const ValueOption *>(option);
        if (vopt) {
            return vopt->value;
        }
        const ToggleOption *topt = dynamic_cast<const ToggleOption *>(option);
        if (topt) {
            return topt->toggled ? "true" : "false";
        }
    }
    return "";
}

} // namespace cmdlp