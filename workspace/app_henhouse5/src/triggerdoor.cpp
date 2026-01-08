#include "triggerdoor.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(TriggerDoor, LOG_LEVEL_INF);

TriggerDoor::TriggerDoor(ErrorCode::Instance instance, Rtc& rtc, k_thread_stack_t * stack_area, const uint32_t stack_size, const int32_t priority) :
    Thread(instance, stack_area, stack_size, priority),
	m_rtc(rtc)
	{
    
}

const ErrorCode TriggerDoor::run_internal() {
    while(true) {
		int tm_sec, tm_min, tm_hour;
		m_rtc.get_time(tm_hour, tm_min, tm_sec);
		tm_sec += 2;
		m_rtc.set_alarm(tm_hour, tm_min, tm_sec);

        k_sleep(K_SECONDS(10));
        m_rtc.log_date_time();
    }

	return ErrorCode(m_instance, ErrorCode::runtime, 255);
}


