#include "doortriggerauto.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(DoorTriggerAuto, LOG_LEVEL_INF);

DoorTriggerAuto::DoorTriggerAuto(ErrorCode::Instance instance, Rtc& rtc, k_thread_stack_t * stack_area, const uint32_t stack_size, const int32_t priority) :
    Thread(instance, stack_area, stack_size, priority, "DoorTriggerAuto"),
	Publisher(instance),
	m_rtc(rtc)
	{
    
}

const ErrorCode DoorTriggerAuto::run_internal() {
    k_sem_init(&m_alarm_semaphore, 0, 1);
	
	auto_open_close();		
    while(true) {
		update_alarm();
		// wait half a day...
		k_sem_take(&m_alarm_semaphore, K_FOREVER);
        m_rtc.log_date_time();
		auto_open_close();		
    }

	return ErrorCode(Thread::m_instance, ErrorCode::unexpected_exit, 1);
}

const bool DoorTriggerAuto::should_now_be_open() {
	int tm_sec, tm_min, tm_hour;
	m_rtc.get_time(tm_hour, tm_min, tm_sec);
	return tm_hour >= m_open_time_hour && tm_min >= m_open_time_min && tm_hour <= m_close_time_hour && tm_min < m_close_time_min;
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
	return m_rtc.set_alarm(tm_hour, tm_min, tm_sec);
}

const ErrorCode DoorTriggerAuto::auto_open_close() {
	bool should_open = should_now_be_open();
	if (should_open) {
		LOG_INF("Open door!");
	}
	else {
		LOG_INF("Close door!");
	}

	_notify(should_open);
	return ErrorCode(Thread::m_instance, ErrorCode::success);
}


void DoorTriggerAuto::callback_rtc_alarm(uint16_t alarm_id) {
    k_sem_give(&m_alarm_semaphore);
}
