

#ifndef __SUPERDP_BACKTRACE_H__
#define __SUPERDP_BACKTRACE_H__

#define BACKWARD_HAS_BFD 1
#include "backward.h"

namespace newplan_toolkit
{
    class BackTraceService
    {
    public:
        static const BackTraceService *register_backtrace_service()
        {
            static BackTraceService bt_servcie = BackTraceService();
            return &bt_servcie;
        }

        static void show_stack_snapshot()
        {
            backward::StackTrace st;
            st.load_here(128);
            backward::Printer p;
            p.print(st);
        }

        virtual ~BackTraceService() {}

        explicit BackTraceService()
        {
            // TRACING("Tracing register_backtrace_service function call");
            std::vector<int> signals;

            // signals.push_back(SIGINT); //signal from system interrept

            signals.push_back(SIGABRT); // Abort signal from abort(3)
            signals.push_back(SIGBUS);  // Bus error (bad memory access)
            signals.push_back(SIGFPE);  // Floating point exception
            signals.push_back(SIGILL);  // Illegal Instruction
            signals.push_back(SIGIOT);  // IOT trap. A synonym for SIGABRT
            signals.push_back(SIGQUIT); // Quit from keyboard
            signals.push_back(SIGSEGV); // Invalid memory reference
            signals.push_back(SIGSYS);  // Bad argument to routine (SVr4)
            signals.push_back(SIGTRAP); // Trace/breakpoint trap
            signals.push_back(SIGXCPU); // CPU time limit exceeded (4.2BSD)
            signals.push_back(SIGXFSZ); // File size limit exceeded (4.2BSD)

            static backward::SignalHandling sh(signals);
            // static backward::SignalHandling sh;
            if (sh.loaded())
                std::cout << "Registering backtrace service successful" << std::endl;
            else
                std::cout << "Registering backtrace service failed" << std::endl;
        }
    };

#define NEW_CONNECTION(text1, text2) text1##text2
#define NEW_CONNECT(text1, text2) NEW_CONNECTION(text1, text2)

#define __REGISTER_SERVICE(file)                 \
    static auto NEW_CONNECT(bt_servcie_, file) = \
        BackTraceService::register_backtrace_service()

#define REGISTER_SERVICE() __REGISTER_SERVICE(__COUNTER__)

    __REGISTER_SERVICE(__COUNTER__);

    static __attribute__((unused)) double execption_test(int a = 1, int b = 0)
    {
        if (b == 0)
        {
            throw "Division by zero condition!";
        }
        while (1)
            a = a + 2; // normally, the process would stuck here until a signal ctrl+c

        return a / b;
    }
}; // namespace newplan_toolkit

#endif //__SUPERDP_BACKTRACE_H__