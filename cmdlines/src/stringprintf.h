

// Printf variants that place their output in a C++ string.
//
// Usage:
//      string result = strings::Printf("%d %s\n", 10, "hello");
//      strings::Appendf(&result, "%d %s\n", 20, "there");

#ifndef NEWPLAN_TOOLKIT_CORE_PLATFORM_STRINGPRINTF_H_
#define NEWPLAN_TOOLKIT_CORE_PLATFORM_STRINGPRINTF_H_

#include <stdarg.h>

#include <string>

#include "macros.h"
#include "types.h"

namespace newplan_toolkit
{
    namespace strings
    {
        // Return a C++ string
        extern std::string Printf(const char *format, ...)
            // Tell the compiler to do printf format string checking.
            TF_PRINTF_ATTRIBUTE(1, 2);

        // Append result to a supplied string
        extern void Appendf(std::string *dst, const char *format, ...)
            // Tell the compiler to do printf format string checking.
            TF_PRINTF_ATTRIBUTE(2, 3);

        // Lower-level routine that takes a va_list and appends to a specified
        // string.  All other routines are just convenience wrappers around it.
        extern void Appendv(std::string *dst, const char *format, va_list ap);

    }; // namespace strings
};     // namespace newplan_toolkit

#endif // NEWPLAN_TOOLKIT_CORE_PLATFORM_STRINGPRINTF_H_
