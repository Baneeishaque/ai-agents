cmake_minimum_required(VERSION 3.14)

# Find JNI
find_package(JNI REQUIRED)

# JNI library
add_library(git_reword_jni SHARED
    git_reword_jni.cpp
)

target_include_directories(git_reword_jni PRIVATE
    ${JNI_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

target_link_libraries(git_reword_jni PRIVATE
    git_reword_core
    ${JNI_LIBRARIES}
)

# Set output name
set_target_properties(git_reword_jni PROPERTIES
    OUTPUT_NAME "git_reword_jni"
    PREFIX ""
)

# Platform-specific settings
if(WIN32)
    set_target_properties(git_reword_jni PROPERTIES
        SUFFIX ".dll"
    )
elseif(APPLE)
    set_target_properties(git_reword_jni PROPERTIES
        SUFFIX ".dylib"
    )
else()
    set_target_properties(git_reword_jni PROPERTIES
        SUFFIX ".so"
    )
endif()

# Install
install(TARGETS git_reword_jni
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Copy Java source files
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/
    DESTINATION share/git-reword-jni/java
    FILES_MATCHING PATTERN "*.java" PATTERN "*.kt"
)
