#pragma once

#include "errorcode.h"
#include "thread.h"
#include "rtc.hpp"

class TriggerDoor : public Thread {
    public:
        TriggerDoor(ErrorCode::Instance instance, Rtc& rtc, k_thread_stack_t * stack_area, const uint32_t stack_size, const int32_t priority);
        virtual ~TriggerDoor() {};

        virtual const ErrorCode run_internal();

    protected:
        Rtc& m_rtc;
};
