#include "doortriggerauto.h"

#include <zephyr/sys/timeutil.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(DoorTriggerAuto, LOG_LEVEL_INF);

DoorTriggerAuto::DoorTriggerAuto(ErrorCode::Instance instance, Rtc& rtc, k_thread_stack_t * stack_area, const uint32_t stack_size, const int32_t priority) :
    Thread(instance, stack_area, stack_size, priority, "DoorTriggerAuto"),
	Publisher(instance),
	m_rtc(rtc),
	m_next_do_open(false)
	{
	}


#if 0
// @todo move test function to integration test...
const ErrorCode DoorTriggerAuto::run_internal() {
    k_sem_init(&m_alarm_semaphore, 0, 1);
	
    while(true) {
		auto_open_close();
		int tm_sec, tm_min, tm_hour;
		m_rtc.get_time(tm_hour, tm_min, tm_sec);
		tm_sec += 10;
		m_rtc.set_alarm(tm_hour, tm_min, tm_sec);
		
		k_sem_take(&m_alarm_semaphore, K_FOREVER);
        m_rtc.log_date_time();
    }

	return ErrorCode(Thread::m_instance, ErrorCode::runtime, 255);
}
#endif


const ErrorCode DoorTriggerAuto::run_internal() {
    k_sem_init(&m_alarm_semaphore, 0, 1);
	auto_open_close();
    while(true) {
		update_alarm();

		// wait around half a day...
		k_sem_take(&m_alarm_semaphore, K_FOREVER);
        m_rtc.log_date_time();
		do_openclose(m_next_do_open);
    }

	return ErrorCode(Thread::m_instance, ErrorCode::unexpected_exit, 1);
}

const bool DoorTriggerAuto::should_now_be_open() {
	int tm_sec, tm_min, tm_hour;
	m_rtc.get_time(tm_hour, tm_min, tm_sec);

	const int32_t time_now   = tm_sec +           tm_min*60 +           tm_hour*3600;
	const int32_t time_open  =      0 +  m_open_time_min*60 +  m_open_time_hour*3600;
	const int32_t time_close =      0 + m_close_time_min*60 + m_close_time_hour*3600;

	if (time_now > time_close || time_now < time_open) {
		return false;
	}
	else {
		return true;
	}
}

const ErrorCode DoorTriggerAuto::update_alarm() {
	int tm_sec, tm_min, tm_hour;
	bool next_close = should_now_be_open();
	
	if (next_close) {
		tm_hour = m_close_time_hour;
		tm_min = m_close_time_min;
		tm_sec = 0;
		LOG_INF("Next: close door at %02u:%02u!", tm_hour, tm_min);
	}
	else {
		tm_hour = m_open_time_hour;
		tm_min = m_open_time_min;
		tm_sec = 0;
		LOG_INF("Next: open door at %02u:%02u!", tm_hour, tm_min);
	}
	m_next_do_open = !next_close;
	return m_rtc.set_alarm(tm_hour, tm_min, tm_sec);
}

const ErrorCode DoorTriggerAuto::do_openclose(bool do_open) {
	return _notify(do_open);
}

const ErrorCode DoorTriggerAuto::auto_open_close() {
	bool do_open_now = should_now_be_open();
	if (do_open_now) {
		LOG_INF("Open door!");
	}
	else {
		LOG_INF("Close door!");
	}

	return do_openclose(do_open_now);
}


void DoorTriggerAuto::callback_rtc_alarm(uint16_t alarm_id) {
    k_sem_give(&m_alarm_semaphore);
}
