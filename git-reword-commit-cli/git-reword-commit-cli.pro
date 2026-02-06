# Qt Creator project file for git-reword-commit-cli
# This allows opening the project in Qt Creator even without CMake integration

TEMPLATE = app
TARGET = git-reword-commit

CONFIG += c++17
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

# Sources
SOURCES += \
    src/main.cpp \
    src/platform.cpp \
    src/utils.cpp \
    src/git_operations.cpp \
    src/reword_engine.cpp

# Headers
HEADERS += \
    include/git_reword/export.hpp \
    include/git_reword/platform.hpp \
    include/git_reword/utils.hpp \
    include/git_reword/git_operations.hpp \
    include/git_reword/reword_engine.hpp

# Include paths
INCLUDEPATH += include

# Platform-specific settings
win32 {
    LIBS += -lshlwapi
    DEFINES += _CRT_SECURE_NO_WARNINGS
}

# Output directory
DESTDIR = $$OUT_PWD/bin

# Compiler flags
unix:!macx {
    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
}

macx {
    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
}

win32-msvc* {
    QMAKE_CXXFLAGS += /W4
}

win32-g++ {
    QMAKE_CXXFLAGS += -Wall -Wextra -Wpedantic
}

# Android configuration
android {
    TARGET = git-reword-commit
}

# iOS configuration  
ios {
    TARGET = git-reword-commit
}
