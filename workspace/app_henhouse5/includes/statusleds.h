#pragma once

#include "thread.h"
#include "errorcode.h"
#include "outputpin.h"


class StatusLeds : public Thread {
    public:
        StatusLeds(ErrorCode::Instance instance, OutputPin& ledGreen, OutputPin& ledRed, k_thread_stack_t * stack_area, const uint32_t stack_size, const int32_t priority);
        virtual ~StatusLeds() {};
        virtual const ErrorCode run_internal();

    protected:
        OutputPin& m_ledGreen;
        OutputPin& m_ledRed;
};
