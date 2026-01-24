#include "statusleds.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(StatusLeds, LOG_LEVEL_INF);

StatusLeds::StatusLeds(ErrorCode::Instance instance, OutputPin& ledGreen, OutputPin& ledRed, k_thread_stack_t * stack_area, const uint32_t stack_size, const int32_t priority) :
    Thread(instance, stack_area, stack_size, priority, "StatusLeds"),
    m_ledGreen(ledGreen),
    m_ledRed(ledRed) {
    
}


const ErrorCode StatusLeds::run() {
	LOG_INF("LED Thread started");
    m_ledGreen.set(true);
    m_ledRed.set(false);
    while(true) {
        m_ledGreen.set(true);
        k_sleep(K_MSEC(30));
        m_ledGreen.set(false);
        k_sleep(K_SECONDS(5));
    }

    return ErrorCode(Thread::m_instance, ErrorCode::Code::success);
}

