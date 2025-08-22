#include "cmdlp/detail/option_list.hpp" // For BadConversion
#include "cmdlp/parser.hpp"
#include <iostream>
#include <stdexcept> // For std::out_of_range
#include <string>
#include <vector>

// Helper to test if a specific exception is thrown
template <typename ExceptionType, typename Func>
bool test_exception_thrown(Func func, const std::string &expected_message_part)
{
    try {
        func();
        std::cerr << "FAIL: Expected exception not thrown.\n";
        return false;
    } catch (const ExceptionType &e) {
        std::string what_str = e.what();
        if (what_str.find(expected_message_part) != std::string::npos) {
            std::cout << "SUCCESS: Caught expected exception: " << e.what() << "\n";
            return true;
        } else {
            std::cerr << "FAIL: Caught exception with unexpected message: " << e.what() << "\n";
            std::cerr << "Expected message part: " << expected_message_part << "\n";
            return false;
        }
    } catch (const std::exception &e) {
        std::cerr << "FAIL: Caught unexpected exception type: " << e.what() << "\n";
        return false;
    } catch (...) {
        std::cerr << "FAIL: Caught unknown exception type.\n";
        return false;
    }
}

int main(int, char *[])
{
    bool all_tests_passed = true;

    // Test 1: Missing required value option
    std::cout << "--- Test: Missing required value option ---\n";
    all_tests_passed &= test_exception_thrown<cmdlp::ParsingError>(
        [] {
            std::vector<std::string> args = {"test_error_handling"};
            cmdlp::Parser parser(args);
            parser.addOption("-r", "--required", "A required option", true);
            parser.parseOptions();
        },
        "Cannot find required option: --required");

    // Test 2: Missing required positional option
    std::cout << "--- Test: Missing required positional option ---\n";
    all_tests_passed &= test_exception_thrown<cmdlp::ParsingError>(
        [] {
            std::vector<std::string> args = {"test_error_handling"};
            cmdlp::Parser parser(args);
            parser.addPositionalOption("-p", "--pos", "A required positional argument", true);
            parser.parseOptions();
        },
        "Missing required positional argument: A required positional argument");

    // Test 3: Missing required positional list
    std::cout << "--- Test: Missing required positional list ---\n";
    all_tests_passed &= test_exception_thrown<cmdlp::ParsingError>(
        [] {
            std::vector<std::string> args = {"test_error_handling"};
            cmdlp::Parser parser(args);
            parser.addPositionalList("-l", "--list", "A required positional list", true);
            parser.parseOptions();
        },
        "Missing required positional list argument: A required positional list");

    // Test 4: BadConversion for non-boolean type
    std::cout << "--- Test: BadConversion for non-boolean type ---\n";
    all_tests_passed &= test_exception_thrown<cmdlp::detail::BadConversion>(
        [] {
            std::vector<std::string> args = {"test_error_handling", "--int-val", "not_an_int"};
            cmdlp::Parser parser(args);
            parser.addOption("-i", "--int-val", "An integer value", false);
            parser.parseOptions();
            parser.getOption<int>("--int-val"); // This should trigger the BadConversion
        },
        "Failed to convert value 'not_an_int' to requested type.");

    // Test 5: BadConversion for boolean type (invalid string)
    std::cout << "--- Test: BadConversion for boolean type (invalid string) ---\n";
    all_tests_passed &= test_exception_thrown<cmdlp::detail::BadConversion>(
        [] {
            std::vector<std::string> args = {"test_error_handling", "--bool-val", "not_a_bool"};
            cmdlp::Parser parser(args);
            parser.addOption("-b", "--bool-val", "A boolean value", false);
            parser.parseOptions();
            parser.getOption<bool>("--bool-val"); // This should trigger the BadConversion
        },
        "Failed to convert value 'not_a_bool' to bool. Expected 'true' or 'false'.");

    // Test 6: Option not found (std::out_of_range)
    std::cout << "--- Test: Option not found ---\n";
    all_tests_passed &= test_exception_thrown<std::out_of_range>(
        [] {
            std::vector<std::string> args = {"test_error_handling"};
            cmdlp::Parser parser(args);
            parser.addOption("-o", "--optional", "An optional value", false);
            parser.parseOptions();
            parser.getOption<std::string>("--non-existent"); // This should trigger std::out_of_range
        },
        "Option '--non-existent' not found.");

    if (all_tests_passed) {
        std::cout << "\nAll error handling tests passed successfully!\n";
        return 0;
    } else {
        std::cerr << "\nSome error handling tests failed.\n";
        return 1;
    }
}
