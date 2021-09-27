# ThreadPool
A CPP-based ThreadPool for Linux-platform

## How to use

### example/thread_pool.cc
```cpp
#include "thread_pool.h"
#include <iostream>

int main(int argc, char const *argv[])
{
    newplan_toolkit::ThreadPool tp(10);
    for (int index = 0; index < 100; index++)
    {
        tp.runTask([]{ 
                       VLOG(0) << "Hello, World!";
                    });
    }
    tp.waitWorkComplete();
    return 0;
}
```

### compile:
```bash
g++ example/thread_pool.cc -o thread_pool -std=c++11 -lpthread -lglog
```

### run:
```bash
> ./thread_pool
WARNING: Logging before InitGoogleLogging() is written to STDERR
I0927 18:12:44.019248 21657 test.cc:11] Hello, World!
I0927 18:12:44.019246 21658 test.cc:11] Hello, World!
I0927 18:12:44.019246 21663 test.cc:11] Hello, World!
I0927 18:12:44.019248 21662 test.cc:11] Hello, World!
I0927 18:12:44.019249 21664 test.cc:11] Hello, World!
I0927 18:12:44.019248 21660 test.cc:11] Hello, World!
I0927 18:12:44.019568 21664 test.cc:11] Hello, World!
I0927 18:12:44.019299 21666 test.cc:11] Hello, World!
I0927 18:12:44.019297 21659 test.cc:11] Hello, World!
I0927 18:12:44.019335 21665 test.cc:11] Hello, World!
I0927 18:12:44.019661 21659 test.cc:11] Hello, World!
I0927 18:12:44.019676 21665 test.cc:11] Hello, World!
I0927 18:12:44.019527 21658 test.cc:11] Hello, World!
I0927 18:12:44.019539 21662 test.cc:11] Hello, World!
I0927 18:12:44.019598 21660 test.cc:11] Hello, World!
I0927 18:12:44.019603 21664 test.cc:11] Hello, World!
I0927 18:12:44.019495 21657 test.cc:11] Hello, World!
I0927 18:12:44.019645 21666 test.cc:11] Hello, World!
I0927 18:12:44.019356 21661 test.cc:11] Hello, World!
I0927 18:12:44.019520 21663 test.cc:11] Hello, World!
```

# ThreadPerf
a thread perf tool to show CPU usage
## How to use
### example/thread_perf.cc
```cpp
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
```
### compile 
```bash
g++ example/thread_perf.cc -o thread_perf -std=c++11 -lpthread 
```

### run:
```bash
>./thread_perf [pid] [tid]
pid is the process id and optional. By default, it refers itself
tid is the threads id and optional. For all threads, set tid=-1
pid=24453,tid=0
----------------------------
Cpu(s):  0.00%us,  0.02%sy,  0.00%ni,  99.98%id,  0.00%wa,  0.00%hi,  0.00%si,  0.00%st
Mem: 131904440k total,  73504124k used,  58400316k free,  128700272k buffer
Swap: 17352536k total,  17023812k used,  328724k  free,  2533908k cached
PID=24273  TID=0  0.00%CPU  0.00%MEM VIRT=24273KB RES=31018KB
```