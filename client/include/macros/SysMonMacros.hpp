#pragma once

/**
 * @file SysMonMacros.hpp
 * @brief Declares small utility macros shared across the client's classes.
 */

/**
 * @brief Adds a static ClassName() accessor returning the class name as a string literal.
 * @param cls Name of the class to generate the accessor for.
 */
#define SYSMON_CLASS(cls) \
    public: \
        static constexpr const char* ClassName() { return #cls; }
