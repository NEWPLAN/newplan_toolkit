
   
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include "../time_record.h"

void time_record_test()
{
    newplan_toolkit::Timer cpu_timer;
    int count = 100;
    do
    {
        cpu_timer.Start();
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        cpu_timer.Stop();
        std::cout << "[" << count << "] CPU time cost for: " << cpu_timer.MilliSeconds() << " ms" << std::endl;
    } while (count-- > 0);
}


int main(int argc, char **argv)
{
    time_record_test();
    return 0;
}