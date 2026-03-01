cmake_minimum_required(VERSION 3.14)

# Find Python
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

# Check for pybind11
find_package(pybind11 QUIET)

if(NOT pybind11_FOUND)
    # Fetch pybind11 if not found
    include(FetchContent)
    FetchContent_Declare(
        pybind11
        GIT_REPOSITORY https://github.com/pybind/pybind11.git
        GIT_TAG v2.11.1
    )
    FetchContent_MakeAvailable(pybind11)
endif()

# Create Python module
pybind11_add_module(git_reword_py
    bindings.cpp
)

target_link_libraries(git_reword_py PRIVATE git_reword_core)
target_include_directories(git_reword_py PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# Set output name
set_target_properties(git_reword_py PROPERTIES
    OUTPUT_NAME "git_reword"
)

# Install Python module
install(TARGETS git_reword_py
    LIBRARY DESTINATION ${Python3_SITELIB}
)
