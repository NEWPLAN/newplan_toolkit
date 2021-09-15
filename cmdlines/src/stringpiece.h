

// StringPiece is a simple structure containing a pointer into some external
// storage and a size.  The user of a StringPiece must ensure that the slice
// is not used after the corresponding external storage has been
// deallocated.
//
// Multiple threads can invoke const methods on a StringPiece without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same StringPiece must use
// external synchronization.

#ifndef NEWPLAN_TOOLKIT_CORE_PLATFORM_STRINGPIECE_H_
#define NEWPLAN_TOOLKIT_CORE_PLATFORM_STRINGPIECE_H_

#include "absl/strings/string_view.h" // IWYU pragma: export

namespace newplan_toolkit
{
    using StringPiece = absl::string_view;

} // namespace newplan_toolkit

#endif // NEWPLAN_TOOLKIT_CORE_PLATFORM_STRINGPIECE_H_
