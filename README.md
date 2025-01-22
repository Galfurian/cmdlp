# Command-Line Parsing Library (`cmdlp`)

[![Ubuntu](https://github.com/Galfurian/cmdlp/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/Galfurian/cmdlp/actions/workflows/ubuntu.yml)
[![Windows](https://github.com/Galfurian/cmdlp/actions/workflows/windows.yml/badge.svg)](https://github.com/Galfurian/cmdlp/actions/workflows/windows.yml)
[![MacOS](https://github.com/Galfurian/cmdlp/actions/workflows/macos.yml/badge.svg)](https://github.com/Galfurian/cmdlp/actions/workflows/macos.yml)
[![Documentation](https://github.com/Galfurian/cmdlp/actions/workflows/documentation.yml/badge.svg)](https://github.com/Galfurian/cmdlp/actions/workflows/documentation.yml)

## Overview

`cmdlp` is a lightweight, easy-to-use C++ library for parsing command-line arguments. It simplifies the process of handling various types of options, including toggle switches, value-based options, and multi-value options, while providing a clean and extensible interface.

## Features

- Support for short (`-o`) and long (`--option`) options.
- Toggle options (e.g., `--verbose` for enabling verbose output).
- Value-based options (e.g., `--file input.txt`).
- Multi-value options (e.g., `--mode auto|manual|test`).
- Grouping and labeling options in help messages.
- Automatically generated help messages.

## Installation

To include `cmdlp` in your project, simply add the `cmdlp` source files to your build system. For example:

```bash
mkdir cmdlp
cp tokenizer.hpp option.hpp option_list.hpp parser.hpp cmdlp/
```

Then, include the necessary headers in your project:

```cpp
#include "cmdlp/parser.hpp"
```

## Usage

### Defining and Parsing Command-Line Options

Here is an example of how to define and parse command-line options using `cmdlp`:

```cpp
#include <iostream>
#include "cmdlp/parser.hpp"

int main(int argc, char *argv[]) {
    cmdlp::Parser parser(argc, argv);

    // Define options
    parser.addOption("-db", "--double", "Double value", 0.2, false);
    parser.addOption("-i", "--int", "An integer value", -1, false);
    parser.addToggle("-h", "--help", "Shows help for the program.", false);

    // Parse options
    parser.parseOptions();

    // Retrieve and display parsed options
    std::cout << "Double value: " << parser.getOption<double>("--double") << "\n";
    std::cout << "Integer value: " << parser.getOption<int>("--int") << "\n";
    std::cout << "Help: " << parser.getOption<bool>("--help") << "\n";

    return 0;
}
```

### Example Output

Running the above program with the following arguments:

```bash
./example --double 3.14 --int 42 --help
```

Produces:

```bash
Double value: 3.14
Integer value: 42
Help: true
```

## Example

Here is a more comprehensive example demonstrating various features of `cmdlp`:

```cpp
#include <iostream>
#include "cmdlp/parser.hpp"

int main(int argc, char *argv[]) {
    cmdlp::Parser parser(argc, argv);

    parser.addSeparator("General Options:");
    parser.addOption("-db", "--double", "Double value", 0.5, false);
    parser.addOption("-i", "--int", "Integer value", -1, false);

    parser.addSeparator("Toggle Options:");
    parser.addToggle("-v", "--verbose", "Enable verbose mode", false);

    parser.addSeparator("Multi-value Options:");
    parser.addMultiOption("-m", "--mode", "Operation mode", {"auto", "manual"}, "auto");

    parser.parseOptions();

    std::cout << "Help: \n" << parser.getHelp() << "\n";
    std::cout << "Parsed Options: \n";
    std::cout << "  Double: " << parser.getOption<double>("--double") << "\n";
    std::cout << "  Integer: " << parser.getOption<int>("--int") << "\n";
    std::cout << "  Verbose: " << parser.getOption<bool>("--verbose") << "\n";
    std::cout << "  Mode: " << parser.getOption<std::string>("--mode") << "\n";

    return 0;
}
```

### Example Output

Running with:

```bash
./example --double 3.14 --int 42 --verbose --mode manual
```

Produces:

```bash
Help:
General Options:
  -db, --double (0.5) : Double value
  -i, --int (-1)      : Integer value
Toggle Options:
  -v, --verbose (false) : Enable verbose mode
Multi-value Options:
  -m, --mode (auto)   : Operation mode [auto, manual]

Parsed Options:
  Double: 3.14
  Integer: 42
  Verbose: true
  Mode: manual
```

## Tests

To verify the correctness of the library, compile and run the test file:

```cpp
#include "cmdlp/parser.hpp"
#define TEST_OPTION(OPT, VALUE)                                                              if (OPT != VALUE) {                                                                          std::cerr << "The option `" << OPT << "` is different than `" << VALUE << "`\n";         std::cerr << parser.getHelp() << "\n";                                                  return 1;                                                                            }

int main(int, char *[]) {
    std::vector<const char *> arguments = {
        "test_cmdlp",
        "--double", "0.00006456",
        "--int", "-42",
        "-u", "17",
        "-s", "Hello",
        "--verbose",
    };

    cmdlp::Parser parser(static_cast<int>(arguments.size()), const_cast<char **>(arguments.data()));
    parser.addOption("-d", "--double", "Double value", 0.2, false);
    parser.addOption("-i", "--int", "An integer value", -1, false);
    parser.addOption("-u", "--unsigned", "An unsigned value", 1, false);
    parser.addOption("-s", "--string", "A string", "hello", false);
    parser.addToggle("-v", "--verbose", "Verbose output", false);
    parser.parseOptions();

    TEST_OPTION(parser.getOption<double>("-d"), 0.00006456);
    TEST_OPTION(parser.getOption<int>("-i"), -42);
    TEST_OPTION(parser.getOption<int>("-u"), 17);
    TEST_OPTION(parser.getOption<std::string>("-s"), "Hello");
    TEST_OPTION(parser.getOption<bool>("-v"), true);

    return 0;
}
```

Run:

```bash
g++ test.cpp -o test && ./test
```

## API Reference

### Key Classes

- **Tokenizer**: Parses raw command-line arguments.
- **Option**: Base class for all options.
  - **ValueOption**: Represents options with a single value.
  - **ToggleOption**: Represents boolean flags.
  - **MultiOption**: Represents options with predefined values.
- **Parser**: Central class for defining and parsing options.

## Contributing

Contributions are welcome! Feel free to open issues or submit pull requests to enhance the library.

## License

The code is completely unlicensed.
