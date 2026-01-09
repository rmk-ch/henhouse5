#include "rtc.hpp"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Rtc, LOG_LEVEL_INF);

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/sys/util.h>

Rtc::Rtc(const ErrorCode::Instance instance) :
    Publisher(instance),
    m_rtc(DEVICE_DT_GET(DT_ALIAS(rtc)))
    {
    
}

const ErrorCode Rtc::init() {
    if (!device_is_ready(m_rtc)) {
        LOG_ERR("RTC is not ready\n");
		return ErrorCode(m_instance, ErrorCode::not_ready, 1);
	}
    return ErrorCode(m_instance, ErrorCode::success);
}

static void alarm_callback(const struct device *dev, uint16_t id, void *this_ptr) {
    LOG_INF("Alarm id %u", id);
    Rtc* rtc = static_cast<Rtc*>(this_ptr);
    rtc->_notify(id);
}



const ErrorCode Rtc::set_date_time(int tm_sec, int tm_min, int tm_hour, int tm_mday, int tm_mon, int tm_year) {
	int ret = 0;
    struct rtc_time tm = {
            .tm_sec = tm_sec,
            .tm_min = tm_min,
            .tm_hour = tm_hour,
            .tm_mday = tm_mday,
            .tm_mon = tm_mon-1,
            .tm_year = tm_year-1900,
        };

	ret = rtc_set_time(m_rtc, &tm);
	if (ret < 0) {
        LOG_ERR("Cannot write date time: %d\n", ret);
		return ErrorCode(m_instance, ErrorCode::runtime, 1);
	}
	LOG_INF("RTC set: %04d-%02d-%02d %02d:%02d:%02d", tm.tm_year+1900,
	       tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return ErrorCode(m_instance, ErrorCode::success);
}


const ErrorCode Rtc::get_time(int& tm_hour, int& tm_min, int& tm_sec) {
	int ret = 0;
	struct rtc_time tm;

	ret = rtc_get_time(m_rtc, &tm);
	if (ret < 0) {
        LOG_ERR("Cannot read date time: %d\n", ret);
		return ErrorCode(m_instance, ErrorCode::runtime, 2);
	}

    tm_hour = tm.tm_hour;
    tm_min = tm.tm_min;
    tm_sec = tm.tm_sec;
    return ErrorCode(m_instance, ErrorCode::success);
}


const ErrorCode Rtc::log_date_time() {
	int ret = 0;
	struct rtc_time tm;

	ret = rtc_get_time(m_rtc, &tm);
	if (ret < 0) {
        LOG_ERR("Cannot read date time: %d\n", ret);
		return ErrorCode(m_instance, ErrorCode::runtime, 2);
	}

	LOG_INF("It is now: %04d-%02d-%02d %02d:%02d:%02d", tm.tm_year+1900,
	       tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    return ErrorCode(m_instance, ErrorCode::success);
}


const ErrorCode Rtc::set_alarm(int tm_hour, int tm_min, int tm_sec) {
    int ret;

    // sanitize time!
    while (tm_sec >= 60) {
        tm_min += 1;
        tm_sec -= 60;
    }
    while (tm_min >= 60) {
        tm_hour += 1;
        tm_min -= 60;
    }
    while (tm_hour >= 24) {
        tm_hour -= 24;
    }


    LOG_INF("Setting alarm time to %02u:%02u:%02u!", tm_hour, tm_min, tm_sec);
    ret = rtc_alarm_set_callback(m_rtc, 0, alarm_callback, this);
    if (ret != 0) {
        LOG_ERR("Setting alarm callback failed: %i", ret);
        return ErrorCode(m_instance, ErrorCode::runtime, 1);
    }
    
    struct rtc_time alarm_time = {
        .tm_sec = tm_sec,
        .tm_min = tm_min,
        .tm_hour = tm_hour,
    };
    ret = rtc_alarm_set_time(m_rtc, 0, 
        RTC_ALARM_TIME_MASK_SECOND	| RTC_ALARM_TIME_MASK_MINUTE | RTC_ALARM_TIME_MASK_HOUR,
        &alarm_time);
    if (ret != 0) {
        LOG_ERR("Setting alarm time failed: %i", ret);
        return ErrorCode(m_instance, ErrorCode::runtime, 2);
    }
    
    return ErrorCode(m_instance, ErrorCode::success);
}