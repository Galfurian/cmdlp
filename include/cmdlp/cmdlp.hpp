/// @file cmdlp.hpp
/// @author Enrico Fraccaroli (enry.frak@gmail.com)
/// @brief

#pragma once

#include "line_parser.hpp"
#include "option_list.hpp"
#include "option.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>

namespace cmdlp
{
class OptionParser {
public:
    OptionParser(int argc, char **argv)
        : parser(argc, argv),
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
            if ((vopt = dynamic_cast<ValueOption *>(*it))) {
                // Try to search '-*'
                if (!(value = parser.getOption(vopt->optc)).empty()) {
                    vopt->value = value;
                    options.updateLongestValue(value.length());
                }
                // Try to search '--***'
                else if (!(value = parser.getOption(vopt->opts)).empty()) {
                    vopt->value = value;
                    options.updateLongestValue(value.length());
                }
                // If we did not find the option, and the option is required, print an error.
                else if (vopt->required) {
                    std::cerr << "Cannot find required option : " << vopt->opts << "[" << vopt->optc << "]\n";
                    std::cerr << this->getHelp() << "\n";
                    std::exit(1);
                }
            } else if ((topt = dynamic_cast<ToggleOption *>(*it))) {
                // Try to search '-*'
                if (parser.hasOption(topt->optc)) {
                    topt->toggled = true;
                }
                // Try to search '--***'
                else if (parser.hasOption(topt->opts)) {
                    topt->toggled = true;
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
            ss << std::setw(options.getLongestOption()) << std::left << (*it)->opts;
            ss << " (" << std::setw(options.getLongestValue()) << std::right;
            if ((vopt = dynamic_cast<ValueOption *>(*it))) {
                ss << vopt->value;
            } else if ((topt = dynamic_cast<ToggleOption *>(*it))) {
                ss << (topt->toggled ? "true" : "false");
            }
            ss << ") : ";
            ss << (*it)->description << "\n";
        }
        return ss.str();
    }

private:
    LineParser parser;
    OptionList options;
    bool option_parsed;
};

} // namespace cmdlp