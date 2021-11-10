## INTRODUCTION:
backtrace is a fashion implementation for C++ programmer to show the runtime_backtraces.

## REQUIREMENT
- [libbfd](http://www.gnu.org/software/binutils/): from GNU/binutils, you can install it by ```apt-get install binutils-dev```, and link it with ```-lbfd -ldl ```

- Compile option: better work under debug mode (```-g -O0```) for print pretty stack.




## COMPILE:
```bash
make  #compile_option: -g -O0, link_option: -lbfd -ldl
```

## USAGE:
### example
```cpp
#include "../backtrace_service.h"
#include <iostream>
namespace
{
    void show_stack_snapshot_at_runtime()
    {
        std::cout << "=========== show_stack_snapshot_at_runtime===========" << std::endl;
        newplan_toolkit::BackTraceService::show_stack_snapshot();
    }
    void show_exception()
    {
        std::cout << "===========show_exception===========" << std::endl;
        newplan_toolkit::execption_test();
    }
}; // namespace
int main(void)
{
    show_stack_snapshot_at_runtime();
    show_exception();
    return 0;
}
```

### result
```bash
Registering backtrace service successful
=========== show_stack_snapshot_at_runtime===========
Stack trace (most recent call last):
#7    Object "", at 0xffffffffffffffff, in
#6    Object "backtrace_benchmark", at 0x5568e64a38d9, in _start
#5    Source "/build/glibc-S9d2JN/glibc-2.27/csu/../csu/libc-start.c", line 310, in __libc_start_main
#4    Source "example/backward_test.cc", line 18, in main
         15: }; // namespace
         16: int main(void)
         17: {
      >  18:     show_stack_snapshot_at_runtime();
         19:     show_exception();
         20:     return 0;
         21: }
#3    Source "example/backward_test.cc", line 8, in (anonymous namespace)::show_stack_snapshot_at_runtime()
          5:     void show_stack_snapshot_at_runtime()
          6:     {
          7:         std::cout << "=========== show_stack_snapshot_at_runtime===========" << std::endl;
      >   8:         newplan_toolkit::BackTraceService::show_stack_snapshot();
          9:     }
         10:     void show_exception()
         11:     {
#2    Source "example/../backtrace_service.h", line 23, in newplan_toolkit::BackTraceService::show_stack_snapshot()
         20:         static void show_stack_snapshot()
         21:         {
         22:             backward::StackTrace st;
      >  23:             st.load_here(128);
         24:             backward::Printer p;
         25:             p.print(st);
         26:         }
#1    Source "example/../3rd_party/backward/backward.hpp", line 869, in backward::StackTraceImpl<backward::system_tag::linux_tag>::load_here(unsigned long, void*, void*)
        866:       return 0;
        867:     }
        868:     _stacktrace.resize(depth);
      > 869:     size_t trace_cnt = details::unwind(callback(*this), depth);
        870:     _stacktrace.resize(trace_cnt);
        871:     skip_n_firsts(0);
        872:     return size();
#0    Source "example/../3rd_party/backward/backward.hpp", line 851, in unsigned long backward::details::unwind<backward::StackTraceImpl<backward::system_tag::linux_tag>::callback>(backward::StackTraceImpl<backward::system_tag::linux_tag>::callback, unsigned long)
        849: template <typename F> size_t unwind(F f, size_t depth) {
        850:   Unwinder<F> unwinder;
      > 851:   return unwinder(f, depth);
        852: }
        853:
        854: } // namespace details
===========show_exception===========
terminate called after throwing an instance of 'char const*'
Stack trace (most recent call last):
#11   Object "", at 0xffffffffffffffff, in
#10   Object "backtrace_benchmark", at 0x5568e64a38d9, in _start
#9    Source "/build/glibc-S9d2JN/glibc-2.27/csu/../csu/libc-start.c", line 310, in __libc_start_main [0x7ff391249bf6]
#8    Source "example/backward_test.cc", line 19, in main [0x5568e64a3a7b]
         16: int main(void)
         17: {
         18:     show_stack_snapshot_at_runtime();
      >  19:     show_exception();
         20:     return 0;
         21: }
#7    Source "example/backward_test.cc", line 13, in (anonymous namespace)::show_exception() [0x5568e64a3a6a]
         10:     void show_exception()
         11:     {
         12:         std::cout << "===========show_exception===========" << std::endl;
      >  13:         newplan_toolkit::execption_test();
         14:     }
         15: }; // namespace
         16: int main(void)
#6    Source "example/../backtrace_service.h", line 73, in newplan_toolkit::execption_test(int, int) [0x5568e64a39f5]
         70:     {
         71:         if (b == 0)
         72:         {
      >  73:             throw "Division by zero condition!";
         74:         }
         75:         while (1)
         76:             a = a + 2; // normally, the process would stuck here until a signal ctrl+c
#5    Object "/usr/lib/x86_64-linux-gnu/libstdc++.so.6", at 0x7ff3918c3d53, in __cxa_throw
#4    Object "/usr/lib/x86_64-linux-gnu/libstdc++.so.6", at 0x7ff3918c3b20, in std::terminate()
#3    Object "/usr/lib/x86_64-linux-gnu/libstdc++.so.6", at 0x7ff3918c3ae5, in std::rethrow_exception(std::__exception_ptr::exception_ptr)
#2    Object "/usr/lib/x86_64-linux-gnu/libstdc++.so.6", at 0x7ff3918bd956, in
#1    Source "/build/glibc-S9d2JN/glibc-2.27/stdlib/abort.c", line 79, in __GI_abort [0x7ff391268920]
#0    Source "/build/glibc-S9d2JN/glibc-2.27/signal/../sysdeps/unix/sysv/linux/raise.c", line 51, in __GI_raise [0x7ff391266fb7]
Aborted (Signal sent by tkill() 32001 1001)
fish: Job 1, './backtrace_benchmark' terminated by signal SIGABRT (Abort)
```