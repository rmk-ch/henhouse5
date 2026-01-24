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

        virtual const ErrorCode run();
        const ErrorCode autoOpenClose();

        void alarmCallback(uint16_t alarm_id);

    protected:
        const bool shouldNowBeOpen();
        const ErrorCode updateAlarm();
        const ErrorCode doOpenClose(bool do_open);
        
        
        Rtc& m_rtc;
        struct k_sem m_alarmSemaphore;
        bool m_nextDoOpen;

        const int m_openTimeHour = 8;
        const int m_openTimeMin = 30;
        
        const int m_closeTimeHour = 18;
        const int m_closeTimeMin = 30;
};
