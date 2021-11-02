#include <iostream>
#include <sstream>
#include "../time_event.h"

namespace
{

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
};


int main(int argc, char **argv)
{
    time_event_test();
    return 0;
}