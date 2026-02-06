/**
 * @file export.hpp
 * @brief Export macros for shared library builds
 */

#ifndef GIT_REWORD_EXPORT_HPP
#define GIT_REWORD_EXPORT_HPP

#ifdef GIT_REWORD_SHARED
    #ifdef _WIN32
        #ifdef GIT_REWORD_EXPORTS
            #define GIT_REWORD_API __declspec(dllexport)
        #else
            #define GIT_REWORD_API __declspec(dllimport)
        #endif
    #else
        #ifdef GIT_REWORD_EXPORTS
            #define GIT_REWORD_API __attribute__((visibility("default")))
        #else
            #define GIT_REWORD_API
        #endif
    #endif
#else
    #define GIT_REWORD_API
#endif

#endif // GIT_REWORD_EXPORT_HPP
