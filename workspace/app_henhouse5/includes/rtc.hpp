#pragma once

#include <zephyr/sys/timeutil.h>
#include "errorcode.h"
#include "pubsub.h"

class Rtc : public Publisher<2, uint16_t> {
    public:
        Rtc(const ErrorCode::Instance instance);
        virtual ~Rtc() {};

        virtual const ErrorCode init();
        
        const ErrorCode set_date_time(
            int tm_sec,	/**< Seconds [0, 59] */
            int tm_min,	/**< Minutes [0, 59] */
            int tm_hour,/**< Hours [0, 23] */
            int tm_mday,/**< Day of the month [1, 31] */
            int tm_mon,	/**< Month [1, 12] */
            int tm_year);/**< Year */
            
        const ErrorCode log_date_time();
        const ErrorCode get_time(int& tm_hour, int& tm_min, int& tm_sec);
        const ErrorCode set_alarm(int tm_hour, int tm_min, int tm_sec);
        // const ErrorCode log_alarm_time();

    protected:
        const struct device * const m_rtc;
};
