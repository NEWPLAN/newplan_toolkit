

#ifndef NEWPLAN_TOOLKIT_CORE_PLATFORM_TYPES_H_
#define NEWPLAN_TOOLKIT_CORE_PLATFORM_TYPES_H_

#include <string>

#include "bfloat16.h"
#include "platform.h"
#include "tstring.h"

// Include appropriate platform-dependent implementations
#if defined(PLATFORM_POSIX) || defined(PLATFORM_POSIX_ANDROID) || defined(PLATFORM_GOOGLE_ANDROID) || defined(PLATFORM_POSIX_IOS) || defined(PLATFORM_GOOGLE_IOS) || defined(PLATFORM_WINDOWS)
#include "integral_types.h"
#else
#error Define the appropriate PLATFORM_<foo> macro for this platform
#endif

namespace newplan_toolkit
{
    // Alias newplan_toolkit::string to std::string.
    using std::string;

    static const uint8 kuint8max = static_cast<uint8>(0xFF);
    static const uint16 kuint16max = static_cast<uint16>(0xFFFF);
    static const uint32 kuint32max = static_cast<uint32>(0xFFFFFFFF);
    static const uint64 kuint64max = static_cast<uint64>(0xFFFFFFFFFFFFFFFFull);
    static const int8 kint8min = static_cast<int8>(~0x7F);
    static const int8 kint8max = static_cast<int8>(0x7F);
    static const int16 kint16min = static_cast<int16>(~0x7FFF);
    static const int16 kint16max = static_cast<int16>(0x7FFF);
    static const int32 kint32min = static_cast<int32>(~0x7FFFFFFF);
    static const int32 kint32max = static_cast<int32>(0x7FFFFFFF);
    static const int64 kint64min = static_cast<int64>(~0x7FFFFFFFFFFFFFFFll);
    static const int64 kint64max = static_cast<int64>(0x7FFFFFFFFFFFFFFFll);

    // A typedef for a uint64 used as a short fingerprint.
    typedef uint64 Fprint;

} // namespace newplan_toolkit

#if defined(PLATFORM_WINDOWS)
#include <cstddef>
typedef std::ptrdiff_t ssize_t;
#endif

#endif // NEWPLAN_TOOLKIT_CORE_PLATFORM_TYPES_H_
