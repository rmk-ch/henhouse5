#pragma once
#include <zephyr/kernel.h>
#include "errorcode.h"
#include "thread.h"
#include "rtc.hpp"

class TriggerDoor : public Thread, public Publisher<bool> {
    public:
        TriggerDoor(ErrorCode::Instance instance, Rtc& rtc, k_thread_stack_t * stack_area, const uint32_t stack_size, const int32_t priority);
        virtual ~TriggerDoor() {};

        virtual const ErrorCode run_internal();
        const ErrorCode auto_open_close();

        void callback_rtc_alarm(uint16_t alarm_id);

    protected:
        Rtc& m_rtc;
        struct k_sem m_alarm_semaphore;

        const int m_open_time_hour = 8;
        const int m_open_time_min = 30;
        
        const int m_close_time_hour = 18;
        const int m_close_time_min = 30;
};

void static_callback_rtc_alarm(void* thisptr, uint16_t alarm_id);
