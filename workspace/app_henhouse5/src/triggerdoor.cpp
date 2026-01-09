#include "triggerdoor.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(TriggerDoor, LOG_LEVEL_INF);

TriggerDoor::TriggerDoor(ErrorCode::Instance instance, Rtc& rtc, k_thread_stack_t * stack_area, const uint32_t stack_size, const int32_t priority) :
    Thread(instance, stack_area, stack_size, priority, "TriggerDoor"),
	Publisher(instance),
	m_rtc(rtc)
	{
    
}

const ErrorCode TriggerDoor::run_internal() {
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

const ErrorCode TriggerDoor::auto_open_close() {
	bool should_open = false;
	int tm_sec, tm_min, tm_hour;
	m_rtc.get_time(tm_hour, tm_min, tm_sec);
	if (tm_hour >= m_open_time_hour && tm_min >= m_open_time_min && tm_hour <= m_close_time_hour && tm_min < m_close_time_min) {
		should_open = true;
		LOG_INF("Open door!");
	}
	else {
		LOG_INF("Close door!");
	}

	_notify(should_open);
	return ErrorCode(Thread::m_instance, ErrorCode::success);
}


void TriggerDoor::callback_rtc_alarm(uint16_t alarm_id) {
    k_sem_give(&m_alarm_semaphore);
}


void static_callback_rtc_alarm(void* thisptr, uint16_t alarm_id) {
	LOG_DBG("Callback static received");
    TriggerDoor* instance = static_cast<TriggerDoor*>(thisptr);
    instance->callback_rtc_alarm(alarm_id);
}
