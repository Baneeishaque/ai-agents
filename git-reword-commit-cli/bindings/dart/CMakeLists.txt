cmake_minimum_required(VERSION 3.14)

# Dart FFI library
add_library(git_reword_dart SHARED
    git_reword_dart.cpp
)

target_include_directories(git_reword_dart PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(git_reword_dart PRIVATE
    git_reword_core
)

# Set output name
set_target_properties(git_reword_dart PROPERTIES
    OUTPUT_NAME "git_reword_dart"
    PREFIX ""
)

# Platform-specific settings
if(WIN32)
    set_target_properties(git_reword_dart PROPERTIES
        SUFFIX ".dll"
    )
elseif(APPLE)
    set_target_properties(git_reword_dart PROPERTIES
        SUFFIX ".dylib"
    )
else()
    set_target_properties(git_reword_dart PROPERTIES
        SUFFIX ".so"
    )
endif()

# Install
install(TARGETS git_reword_dart
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
