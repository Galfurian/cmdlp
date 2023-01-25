# -----------------------------------------------------------------------------
# @brief  : Root cmake file.
# @author : Enrico Fraccaroli
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Set the minimum CMake version, the project name and default build type.
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8...3.22)

# Set the project name.
project(cmdlp CXX)

# Set the default build type to Debug.
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
endif()

# -----------------------------------------------------------------------------
# OPTIONS
# -----------------------------------------------------------------------------

option(CMDLP_BUILD_TESTS "Build tests" OFF)
option(CMDLP_STRICT_WARNINGS "Enable strict compiler warnings" ON)
option(CMDLP_WARNINGS_AS_ERRORS "Treat all warnings as errors" OFF)

# -----------------------------------------------------------------------------
# COMPILATION FLAGS
# -----------------------------------------------------------------------------

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Disable warnings that suggest using MSVC-specific safe functions
    set(CMDLP_COMPILE_OPTIONS ${CMDLP_COMPILE_OPTIONS} -D_CRT_SECURE_NO_WARNINGS)

    if(CMDLP_WARNINGS_AS_ERRORS)
        set(CMDLP_COMPILE_OPTIONS ${CMDLP_COMPILE_OPTIONS} /WX)
    endif(CMDLP_WARNINGS_AS_ERRORS)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(CMDLP_WARNINGS_AS_ERRORS)
        set(CMDLP_COMPILE_OPTIONS ${CMDLP_COMPILE_OPTIONS} -Werror)
    endif(CMDLP_WARNINGS_AS_ERRORS)
endif()

if(CMDLP_STRICT_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # Mark system headers as external for MSVC explicitly
        # https://devblogs.microsoft.com/cppblog/broken-warnings-theory
        set(CMDLP_COMPILE_OPTIONS ${CMDLP_COMPILE_OPTIONS} /experimental:external)
        set(CMDLP_COMPILE_OPTIONS ${CMDLP_COMPILE_OPTIONS} /external:I ${CMAKE_BINARY_DIR})
        set(CMDLP_COMPILE_OPTIONS ${CMDLP_COMPILE_OPTIONS} /external:anglebrackets)
        set(CMDLP_COMPILE_OPTIONS ${CMDLP_COMPILE_OPTIONS} /external:W0)
        set(CMDLP_COMPILE_OPTIONS ${CMDLP_COMPILE_OPTIONS} /W4)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMDLP_COMPILE_OPTIONS ${CMDLP_COMPILE_OPTIONS} -Wall -Wextra -Wconversion -pedantic)
    endif()
endif(CMDLP_STRICT_WARNINGS)

# -----------------------------------------------------------------------------
# LIBRARY
# -----------------------------------------------------------------------------

# Add the C++ Library.
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
# Inlcude header directories and set the library.
target_include_directories(${PROJECT_NAME} INTERFACE ${PROJECT_SOURCE_DIR}/include)
# Set compilation flags.
target_compile_options(${PROJECT_NAME} INTERFACE ${CMDLP_COMPILE_OPTIONS})

# -----------------------------------------------------------------------------
# TESTS
# -----------------------------------------------------------------------------

if(CMDLP_BUILD_TESTS)

    # CMake has support for adding tests to a project.
    enable_testing()

    # -------------------------------------
    # TEST
    # -------------------------------------
    # Add the test.
    add_executable(${PROJECT_NAME}_test_cmdlp ${PROJECT_SOURCE_DIR}/tests/test_cmdlp.cpp)
    # Set compilation flags.
    target_compile_options(${PROJECT_NAME}_test_cmdlp PUBLIC ${CMDLP_COMPILE_OPTIONS})
    # Inlcude header directories.
    target_include_directories(${PROJECT_NAME}_test_cmdlp PUBLIC ${PROJECT_SOURCE_DIR}/include)
    # Liking for the test.
    target_link_libraries(${PROJECT_NAME}_test_cmdlp ${PROJECT_NAME})
    # Add the test.
    add_test(${PROJECT_NAME}_test_cmdlp_run ${PROJECT_NAME}_test_cmdlp)

endif()

# -----------------------------------------------------------------------------
# DOCUMENTATION
# -----------------------------------------------------------------------------

find_package(Doxygen)

if(DOXYGEN_FOUND)
    
    message(STATUS "Retrieving `doxygen-awesome-css`...")

    # = RETIVAL ===============================================================
    # Include fetch content.
    include(FetchContent)
    # Record the options that describe how to populate the specified content.
    FetchContent_Declare(
        doxygenawesome
        GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css
        GIT_TAG 4cd62308d825fe0396d2f66ffbab45d0e247724c # 2.0.3
    )
    # Retrieve the properties related to the content.
    FetchContent_GetProperties(doxygenawesome)
    # If not populated, make the content available.
    if(NOT doxygenawesome_POPULATED)
        # Ensures the named dependencies have been populated.
        FetchContent_MakeAvailable(doxygenawesome)
        # Hide fetchcontent variables, otherwise with ccmake it's a mess.
        mark_as_advanced(FORCE
            FETCHCONTENT_QUIET FETCHCONTENT_BASE_DIR FETCHCONTENT_FULLY_DISCONNECTED FETCHCONTENT_UPDATES_DISCONNECTED
            FETCHCONTENT_UPDATES_DISCONNECTED_DOXYGENAWESOME FETCHCONTENT_SOURCE_DIR_DOXYGENAWESOME
        )
    endif()

    # = CUSTOMIZATION =========================================================
    set(DOXYGEN_PROJECT_NAME "Command Line Parser (CMDLP)")
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
    set(DOXYGEN_SHOW_INCLUDE_FILES NO)
    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_HTML_HEADER ${doxygenawesome_SOURCE_DIR}/doxygen-custom/header.html)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET ${doxygenawesome_SOURCE_DIR}/doxygen-awesome.css)
    set(DOXYGEN_HTML_EXTRA_FILES
        ${doxygenawesome_SOURCE_DIR}/doxygen-awesome-fragment-copy-button.js
        ${doxygenawesome_SOURCE_DIR}/doxygen-awesome-paragraph-link.js
        ${doxygenawesome_SOURCE_DIR}/doxygen-awesome-darkmode-toggle.js
    )
    doxygen_add_docs(
        ${PROJECT_NAME}_documentation
        ${PROJECT_SOURCE_DIR}/README.md
        ${PROJECT_SOURCE_DIR}/include/cmdlp/cmdlp.hpp
        ${PROJECT_SOURCE_DIR}/include/cmdlp/line_parser.hpp
        ${PROJECT_SOURCE_DIR}/include/cmdlp/option_list.hpp
        ${PROJECT_SOURCE_DIR}/include/cmdlp/option.hpp
    )
endif()
