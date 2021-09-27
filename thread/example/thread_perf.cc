#include "../thread_perf.h"

using namespace newplan_toolkit;

int main(int argc, char **argv)
{
    int pid = 0;
    int tid = 0;
    string cpu, mem;
    usage(argv);
    if (argc > 1)
        pid = atoi(argv[1]);
    else
    {
        pid = getpid();
    }
    if (argc > 2)
        tid = atoi(argv[2]);

    printf("pid=%d,tid=%d\n", pid, tid);
    if (pid == 0 && tid == 0)
        return -1;

    while (1)
    {
        string s = GetCpuMem(pid, cpu, mem, tid);
        system("clear");
        cout << "----------------------------" << endl;
        cout << s << endl;
    }
    return 0;
}