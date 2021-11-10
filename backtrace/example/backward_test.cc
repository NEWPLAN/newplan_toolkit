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