/// @file option_parser.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include "tokenizer.hpp"
#include "option.hpp"
#include "option_list.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace cmdlp
{
class OptionParser {
public:
    OptionParser(int argc, char **argv)
        : tokenizer(argc, argv),
          options(),
          option_parsed()
    {
        options.addOption(new ToggleOption("-h", "--help", "Shows this help for the program.", false));
    }

    template <typename T>
    void addOption(char optc,
                   const std::string &opts,
                   const std::string &_description,
                   const T &_value,
                   bool _required = false)
    {
        std::stringstream ss;
        ss << _value;
        options.addOption(new ValueOption(cmdlp::optc_to_string(optc), opts, _description, ss.str(), _required));
    }

    void addToggle(char optc,
                   const std::string &opts,
                   const std::string &_description,
                   bool _toggled = false)
    {
        options.addOption(new ToggleOption(cmdlp::optc_to_string(optc), opts, _description, _toggled));
    }

    template <typename T>
    inline T getOption(const std::string &opts) const
    {
        return options.getOption<T>(opts);
    }

    template <typename T>
    inline T getOption(char optc) const
    {
        return options.getOption<T>(optc);
    }

    void parseOptions()
    {
        ValueOption *vopt;
        ToggleOption *topt;
        std::string value;
        for (OptionList::const_iterator_t it = options.begin(); it != options.end(); ++it) {
            // Check if it is a value-holding option.
            vopt = dynamic_cast<ValueOption *>(*it);
            if (vopt) {
                // Try to search '-*'
                value = tokenizer.getOption(vopt->optc);
                if (!value.empty()) {
                    vopt->value = value;
                    options.updateLongestValue(value.length());
                    continue;
                }
                // Try to search '--***'
                value = tokenizer.getOption(vopt->opts);
                if (!value.empty()) {
                    vopt->value = value;
                    options.updateLongestValue(value.length());
                    continue;
                }
                // If we did not find the option, and the option is required, print an error.
                if (vopt->required) {
                    std::cerr << "Cannot find required option : " << vopt->opts << "[" << vopt->optc << "]\n";
                    std::cerr << this->getHelp() << "\n";
                    std::exit(1);
                }
            } else {
                // Check if it is a toggle option.
                topt = dynamic_cast<ToggleOption *>(*it);
                if (topt) {
                    // Try to search '-*'
                    if (tokenizer.hasOption(topt->optc)) {
                        topt->toggled = true;
                    }
                    // Try to search '--***'
                    else if (tokenizer.hasOption(topt->opts)) {
                        topt->toggled = true;
                    }
                }
            }
        }
        option_parsed = true;
    }

    std::string getHelp() const
    {
        ValueOption *vopt;
        ToggleOption *topt;
        // Print the arguments.
        std::stringstream ss;
        for (OptionList::const_iterator_t it = options.begin(); it != options.end(); ++it) {
            ss << "[" << (*it)->optc << "] ";
            ss << std::setw(options.getLongestOption<int>()) << std::left << (*it)->opts;
            ss << " (" << std::setw(options.getLongestValue<int>()) << std::right;
            // Check if it is a value-holding option.
            vopt = dynamic_cast<ValueOption *>(*it);
            if (vopt) {
                ss << vopt->value;
            } else {
                // Check if it is a toggle option.
                topt = dynamic_cast<ToggleOption *>(*it);
                if (topt) {
                    ss << (topt->toggled ? "true" : "false");
                }
            }
            ss << ") : ";
            ss << (*it)->description << "\n";
        }
        return ss.str();
    }

private:
    Tokenizer tokenizer;
    OptionList options;
    bool option_parsed;
};

} // namespace cmdlp