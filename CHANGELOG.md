# Changelog

## 1.5.1 (2025-08-25)

- **Refactor**:
  - Improve usage printing.

## 1.5.0 (Undated)

- **Chores**:
  - Bump version to 1.5.0.
- **Refactor**:
  - Improve help message formatting and add example.
  - Move BadConversion to root namespace and update usage.
  - Separate parsing from validation and update examples/tests.
  - Split test file and improve test structure.
  - Improve error handling and tokenizer flexibility.
- **Documentation**:
  - Update gh-pages.
- **CI/CD**:
  - Update workflow.

## 1.4.0 (Undated)

- **Chores**:
  - Update version.
- **Documentation**:
  - Update readme.
- **Tests**:
  - Improve tests.
- **Refactor**:
  - Improve tokenizer structure, and move positional options logic inside the parser.
  - Improve help printing.

## 1.3.0 (Undated)

- **Chores**:
  - Update version.
- **Features**:
  - Auto generate usage.
  - Add positional list of arguments.

## 1.2.0 (Undated)

- **Chores**:
  - Update version.
- **Refactor**:
  - Refine how positional options are accessed.

## 1.1.0 (Undated)

- **Chores**:
  - Update version and add missing comments.
  - Reformat code, and add clang-tidy.
  - Update license.
  - Make it c++11 compatible.
  - Remove prints.
  - Reorganize entire repository, and add example.
  - Use latest version of fetched content.
  - Change interface.
- **Refactor**:
  - Remove move.
- **Features**:
  - Add positional arguments.
- **Documentation**:
  - Simplify documentation list of file.
  - Update doxygen generation flow.
  - Update readme.
- **Build System**:
  - Update compilation flow.
- **Tests**:
  - Update test.
- **CI/CD**:
  - Update workflows.
  - Update workflow.
- **Bug Fixes**:
  - Fix wrong parsing of negative and double values.
- **Features**:
  - Add separators.
  - Add print of list on error.
  - Add a multi-option.

## 1.0.1 (Undated)

- **Refactor**:
  - Improve the tokenizer.
  - Simplify how short and long options are defined.
  - Remove help as default option.
  - Rename files.
- **Build System**:
  - Update cmake file.

## 1.0.0 (Undated)

- **Documentation**:
  - Update readme.
- **Bug Fixes**:
  - Minor fixes.
  - Fix reserved name of private function.
- **CI/CD**:
  - Enable github workflow.
- **Build System**:
  - Update compilation.
  - Move gcc-style compile options into the non-msvc branch.
  - Declare makefile check target. Exclude tests from all.
- **Refactor**:
  - Change type from unsigned to std::size_t for length/sizes.
  - Improve the overall structure.
  - Improve compatibility.
- **Tests**:
  - Don't use reserved names for local variables.
  - Update tests.
- **Features**:
  - Add copy constructor for the option list.
  - Add permissive option for MSVC.
