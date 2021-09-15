

#ifndef NEWPLAN_TOOLKIT_CORE_UTIL_COMMAND_LINE_FLAGS_H
#define NEWPLAN_TOOLKIT_CORE_UTIL_COMMAND_LINE_FLAGS_H

#include "types.h"
#include <functional>
#include <string>
#include <vector>

namespace newplan_toolkit
{
    // N.B. This library is for INTERNAL use only.
    //
    // This is a simple command-line argument parsing module to help us handle
    // parameters for C++ binaries. The recommended way of using it is with local
    // variables and an initializer list of Flag objects, for example:
    //
    // int some_int = 10;
    // bool some_switch = false;
    // string some_name = "something";
    // std::vector<newplan_toolkit::Flag> flag_list = {
    //   Flag("some_int", &some_int, "an integer that affects X"),
    //   Flag("some_switch", &some_switch, "a bool that affects Y"),
    //   Flag("some_name", &some_name, "a string that affects Z")
    // };
    // // Get usage message before ParseFlags() to capture default values.
    // string usage = Flag::Usage(argv[0], flag_list);
    // bool parsed_values_ok = Flags::Parse(&argc, argv, flag_list);
    //
    // newplan_toolkit::port::InitMain(usage.c_str(), &argc, &argv);
    // if (argc != 1 || !parsed_values_ok) {
    //    ...output usage and error message...
    // }
    //
    // The argc and argv values are adjusted by the Parse function so all that
    // remains is the program name (at argv[0]) and any unknown arguments fill the
    // rest of the array. This means you can check for flags that weren't understood
    // by seeing if argv is greater than 1.
    // The result indicates if there were any errors parsing the values that were
    // passed to the command-line switches. For example, --some_int=foo would return
    // false because the argument is expected to be an integer.
    //
    // NOTE: Unlike gflags-style libraries, this library is intended to be
    // used in the `main()` function of your binary. It does not handle
    // flag definitions that are scattered around the source code.

    // A description of a single command line flag, holding its name, type, usage
    // text, and a pointer to the corresponding variable.
    class Flag
    {
    public:
        Flag(const char *name, int32 *dst, const string &usage_text,
             bool *dst_updated = nullptr);
        Flag(const char *name, int64 *dst, const string &usage_text,
             bool *dst_updated = nullptr);
        Flag(const char *name, bool *dst, const string &usage_text,
             bool *dst_updated = nullptr);
        Flag(const char *name, string *dst, const string &usage_text,
             bool *dst_updated = nullptr);
        Flag(const char *name, float *dst, const string &usage_text,
             bool *dst_updated = nullptr);

        // These constructors invoke a hook on a match instead of writing to a
        // specific memory location.  The hook may return false to signal a malformed
        // or illegal value, which will then fail the command line parse.
        //
        // "default_value_for_display" is shown as the default value of this flag in
        // Flags::Usage().
        Flag(const char *name, std::function<bool(int32)> int32_hook,
             int32 default_value_for_display, const string &usage_text);
        Flag(const char *name, std::function<bool(int64)> int64_hook,
             int64 default_value_for_display, const string &usage_text);
        Flag(const char *name, std::function<bool(float)> float_hook,
             float default_value_for_display, const string &usage_text);
        Flag(const char *name, std::function<bool(bool)> bool_hook,
             bool default_value_for_display, const string &usage_text);
        Flag(const char *name, std::function<bool(string)> string_hook,
             string default_value_for_display, const string &usage_text);

        bool is_default_initialized() const
        {
            return default_initialized_;
        }

    private:
        friend class Flags;

        bool Parse(string arg, bool *value_parsing_ok) const;

        string name_;
        enum
        {
            TYPE_INT32 = 1,
            TYPE_INT64 = 2,
            TYPE_BOOL = 3,
            TYPE_STRING = 4,
            TYPE_FLOAT = 5,
        } type_;

        std::function<bool(int32)> int32_hook_;
        int32 int32_default_for_display_;

        std::function<bool(int64)> int64_hook_;
        int64 int64_default_for_display_;

        std::function<bool(float)> float_hook_;
        float float_default_for_display_;

        std::function<bool(bool)> bool_hook_;
        bool bool_default_for_display_;

        std::function<bool(string)> string_hook_;
        string string_default_for_display_;

        string usage_text_;
        bool default_initialized_ = true;
    };

    class Flags
    {
    public:
        // Parse the command line represented by argv[0, ..., (*argc)-1] to find flag
        // instances matching flags in flaglist[].  Update the variables associated
        // with matching flags, and remove the matching arguments from (*argc, argv).
        // Return true iff all recognized flag values were parsed correctly, and the
        // first remaining argument is not "--help".
        static bool Parse(int *argc, char **argv, const std::vector<Flag> &flag_list);

        // Return a usage message with command line cmdline, and the
        // usage_text strings in flag_list[].
        static string Usage(const string &cmdline,
                            const std::vector<Flag> &flag_list);
    };

} // namespace newplan_toolkit

#endif // NEWPLAN_TOOLKIT_CORE_UTIL_COMMAND_LINE_FLAGS_H
