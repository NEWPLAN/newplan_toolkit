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
#include <thread>
#include <vector>
#include <memory>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
namespace
{
    void visit_all_threads()
    {
        std::vector<int> tids;
        DIR *proc_dir;
        {
            char dirname[100] = "/proc/self/task";
            // snprintf(dirname, sizeof dirname, "/proc/%d/task", getpid());
            proc_dir = opendir(dirname);
        }

        if (proc_dir)
        {
            /* /proc available, iterate through tasks... */
            struct dirent *entry;
            while ((entry = readdir(proc_dir)) != NULL)
            {
                if (entry->d_name[0] == '.')
                    continue;

                int tid = atoi(entry->d_name);
                tids.push_back(tid);

                /* ... (do stuff with tid) ... */
            }

            closedir(proc_dir);
        }
        else
        {
            /* /proc not available, act accordingly */
        }
        for (auto tid : tids)
        {
            std::cout << "tid: "
                      << tid << std::endl;
        }
    }
    void thread_function(int t_id)
    {
        pid_t pid = getpid();
        std::cout << "Creating thread: " << t_id
                  << " in pid: " << pid << std::endl;
        int left_cycle = 10;
        while (true)
        {
            std::cout << "Running in thread: " << t_id << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            left_cycle -= 1;
            if (left_cycle == t_id ||
                left_cycle < 0) // crashed here
                show_exception();
        }
    }
    void test_multiple_threads(int n = 3)
    {
        std::vector<std::shared_ptr<std::thread>> thread_group;
        for (int index = 0; index < n; index++)
        {

            std::shared_ptr<std::thread> p_thread =
                std::make_shared<std::thread>([index]()
                                              { int t_id = index;
                                              thread_function(t_id); });
            thread_group.push_back(std::move(p_thread));
        }
        visit_all_threads();
        for (auto &p_thread : thread_group)
        {
            p_thread->join();
        }
    }
};
int main(void)
{
    show_stack_snapshot_at_runtime();
    test_multiple_threads();
    show_exception();
    return 0;
}