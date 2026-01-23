#include "doorcontrolthread.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(DoorControlThread, LOG_LEVEL_INF);

DoorControlThread::DoorControlThread(ErrorCode::Instance instance, DoorControl & doorControl, k_thread_stack_t * stack, const uint32_t stack_size, const uint32_t priority) :
    Thread(instance, stack, stack_size, priority, "DoorControlThread"),
    m_instance(instance),
    m_doorControl(doorControl),
    m_lastCommander(DoorCommanderEnum::UNDEFINED)
{    
}

const ErrorCode DoorControlThread::run_internal() {
	LOG_INF("DoorControlThread started");
    k_msgq_init(&m_command_queue, m_command_queue_buffer, sizeof(bool), m_command_queue_max_entries);

    while(true) {
        bool do_open;
        k_msgq_get(&m_command_queue, &do_open, K_FOREVER);
        m_doorControl.openClose(do_open);
    }

    return ErrorCode(Thread::m_instance, ErrorCode::Code::unexpected_exit, 1);
}

void DoorControlThread::openClose(bool do_open, DoorCommanderEnum commander) {
	LOG_INF("Received openclose command from %u", static_cast<uint32_t>(commander));
    k_msgq_put(&m_command_queue, &do_open, K_NO_WAIT);
}