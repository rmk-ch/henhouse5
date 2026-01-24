#include "doortriggerauto.h"

#include <zephyr/sys/timeutil.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(DoorTriggerAuto, LOG_LEVEL_INF);

DoorTriggerAuto::DoorTriggerAuto(ErrorCode::Instance instance, Rtc& rtc, k_thread_stack_t * stackArea, const uint32_t stackSize, const int32_t priority) :
    Thread(instance, stackArea, stackSize, priority, "DoorTriggerAuto"),
	Publisher(instance),
	m_rtc(rtc),
	m_nextDoOpen(false)
	{
	}


#if 0
// @todo move test function to integration test...
const ErrorCode DoorTriggerAuto::run() {
    k_sem_init(&m_alarm_semaphore, 0, 1);
	
    while(true) {
		autoOpenClose();
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


const ErrorCode DoorTriggerAuto::run() {
    k_sem_init(&m_alarmSemaphore, 0, 1);
	autoOpenClose();
    while(true) {
		updateAlarm();

		// wait around half a day...
		k_sem_take(&m_alarmSemaphore, K_FOREVER);
        m_rtc.log_date_time();
		doOpenClose(m_nextDoOpen);
    }

	return ErrorCode(Thread::m_instance, ErrorCode::unexpected_exit, 1);
}

const bool DoorTriggerAuto::shouldNowBeOpen() {
	int tm_sec, tm_min, tm_hour;
	m_rtc.get_time(tm_hour, tm_min, tm_sec);

	const int32_t timeNow   = tm_sec +           tm_min*60 +           tm_hour*3600;
	const int32_t timeOpen  =      0 +  m_openTimeMin*60 +  m_openTimeHour*3600;
	const int32_t timeClose =      0 + m_closeTimeMin*60 + m_closeTimeHour*3600;

	if (timeNow > timeClose || timeNow < timeOpen) {
		return false;
	}
	else {
		return true;
	}
}

const ErrorCode DoorTriggerAuto::updateAlarm() {
	int tm_sec, tm_min, tm_hour;
	bool next_close = shouldNowBeOpen();
	
	if (next_close) {
		tm_hour = m_closeTimeHour;
		tm_min = m_closeTimeMin;
		tm_sec = 0;
		LOG_INF("Next: close door at %02u:%02u!", tm_hour, tm_min);
	}
	else {
		tm_hour = m_openTimeHour;
		tm_min = m_openTimeMin;
		tm_sec = 0;
		LOG_INF("Next: open door at %02u:%02u!", tm_hour, tm_min);
	}
	m_nextDoOpen = !next_close;
	return m_rtc.set_alarm(tm_hour, tm_min, tm_sec);
}

const ErrorCode DoorTriggerAuto::doOpenClose(bool doOpen) {
	return _notify(doOpen);
}

const ErrorCode DoorTriggerAuto::autoOpenClose() {
	bool doOpenNow = shouldNowBeOpen();
	if (doOpenNow) {
		LOG_INF("Open door!");
	}
	else {
		LOG_INF("Close door!");
	}

	return doOpenClose(doOpenNow);
}


void DoorTriggerAuto::alarmCallback(uint16_t alarmId) {
    k_sem_give(&m_alarmSemaphore);
}
