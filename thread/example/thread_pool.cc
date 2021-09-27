#include "../thread_pool.h"
#include <iostream>
#include <glog/logging.h>

int main(int argc, char const *argv[])
{
    newplan_toolkit::ThreadPool tp(10);
    for (int index = 0; index < 20; index++)
    {
        tp.runTask([]
                   { VLOG(0) << "Hello, World!"; });
    }
    tp.waitWorkComplete();
    return 0;
}