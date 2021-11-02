## Introduction:
`timer` is a collective tools related to time, including time_record, time_event, and time_line. They are described as follows.

### Time Record
time record is presented in ```time_record.h```. It provides a      ```Timer```, a high-resolution (nanosecond-level) time record to measure the eslaped time cost between two time point. It works with a flag: ```c++11``` when compiling
```cpp
void time_record_test()
{
    newplan_toolkit::Timer cpu_timer;
    int count = 100;
    do
    {
        cpu_timer.Start();
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        cpu_timer.Stop();
        std::cout << "[" << count << "] CPU time cost for: " 
        << cpu_timer.MilliSeconds() << " ms" << std::endl;
    } while (count-- > 0);
}
``` 

### Time Event
time event is presented in ```time_event.h```. Time event is a high-resolution event manager that handles time-clock events during runtime. It works with a flag: ```-std=c++17``` during compiling

```cpp
void time_event_test()
{
    using namespace std::chrono;
    auto start = std::chrono::steady_clock::now();

    auto duration = [start]() {
        auto now = std::chrono::steady_clock::now();
        auto msecs = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
        std::stringstream ss;
        ss << msecs / 1000.0;
        std::cout << "elapsed " << ss.str() << "s\t: ";
    };

    std::cout << "start" << std::endl;
    newplan_toolkit::TimeEvent t(1ms);
    auto e1 = t.set_timeout(3s, [&]() { duration(); std::cout << "timeout 3s" << std::endl; });
    auto e2 = t.set_interval(100ms, [&]() { duration(); std::cout << "interval 100ms" << std::endl; });
    auto e3 = t.set_timeout(4s, [&]() { duration(); std::cout << "timeout 4s" << std::endl; });
    auto e4 = t.set_interval(2s, [&]() { duration(); std::cout << "interval 2s" << std::endl; });
    auto e5 = t.set_timeout(5s, [&]() { duration(); std::cout << "timeout that never happens" << std::endl; });
    e5->cancel(); // cancel this timeout
    std::this_thread::sleep_for(5s);
    e4->cancel(); // cancel this interval
    std::cout << "cancel interval 2" << std::endl;
    std::this_thread::sleep_for(5s);
    std::cout << "end" << std::endl;
}
```

### Time Line
time line is presented in ```timeline.h```. Time line is a tool to profile event execution as long as time goes away. An example is shown as follow:
```cpp
{
    newplan_toolkit::Instrumentor::Get().BeginSession("profile");
    {
        newplan_toolkit::PROFILE_FUNCTION();
        std::thread a([]() {Function1(); });
        std::thread b([]() {Function2(10); });
 
        a.join();
        b.join();
    }
    newplan_toolkit::Instrumentor::Get().EndSession();
}
```
Then, you will get a json file that records the function start and stop time.
and tab ```chrome://tracing``` in to the search line to reload the json file to visualize the profile result in detail.
```json
{
    "otherData": {}, 
    "traceEvents": 
        [
            {
                "cat":"function",
                "dur":2154,
                "name":"void Function2(int)",
                "ph":"X",
                "pid": 0,
                "tid": "4173821382",
                "ts": 395597191197
            },
            {
                "cat":"function",
                "dur":2619,
                "name":"void Function1()",
                "ph":"X",
                "pid": 0,
                "tid": "271697159",
                "ts": 395597191167
            },
            {
                "cat":"function",
                "dur":2716,
                "name":"void benchmark::RunApp()",
                "ph":"X",
                "pid": 0,
                "tid": "40448362",
                "ts": 395597191136
            }
        ]
}
```

## Compile and Example:
```bash
cd example & make # and run the corresonding execcutable
```

