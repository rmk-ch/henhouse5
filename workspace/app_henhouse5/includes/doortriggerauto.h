#pragma once
#include <zephyr/kernel.h>
#include "errorcode.h"
#include "thread.h"
#include "pubsub.h"
#include "rtc.hpp"

class DoorTriggerAuto : public Thread, public Publisher<2,bool> {
    public:
        DoorTriggerAuto(ErrorCode::Instance instance, Rtc& rtc, k_thread_stack_t * stack_area, const uint32_t stack_size, const int32_t priority);
        virtual ~DoorTriggerAuto() {};

        virtual const ErrorCode run_internal();
        const ErrorCode auto_open_close();

        void callback_rtc_alarm(uint16_t alarm_id);

    protected:
        const bool should_now_be_open();
        const ErrorCode update_alarm();
        const ErrorCode do_openclose(bool do_open);
        
        
        Rtc& m_rtc;
        struct k_sem m_alarm_semaphore;
        bool m_next_do_open;

        const int m_open_time_hour = 8;
        const int m_open_time_min = 30;
        
        const int m_close_time_hour = 18;
        const int m_close_time_min = 30;
};
