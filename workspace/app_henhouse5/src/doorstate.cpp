#include "doorstate.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(DoorState, LOG_LEVEL_INF);

DoorState::DoorState(const ErrorCode::Instance instance, InputPin& endswitchTop, InputPin& endswitchBottom, k_thread_stack_t * stack, const uint32_t stack_size, const uint32_t priority) :
    Thread(instance, stack, stack_size, priority),
    m_endswitch_top(endswitchTop),
    m_endswitch_bottom(endswitchBottom){
}


const ErrorCode DoorState::run() {
	LOG_INF("Doorstate started");
    k_msgq_init(&m_endswitches_queue, m_endswitches_queue_buffer, sizeof(ErrorCode::Instance), m_n_endswitches_queue_max_entries);

    while(true) {
        uint32_t message;
        k_msgq_get(&m_endswitches_queue, &message, K_FOREVER);
	    LOG_INF("Message %u received", message);
    }

    return ErrorCode(m_instance, ErrorCode::Code::not_implemented, 1);
}

void DoorState::callback_endswitches(uint32_t message) {
    k_msgq_put(&m_endswitches_queue, &message, K_NO_WAIT);
}


void static_callback_endswitches(void* thisptr, uint32_t message) {
	LOG_INF("Callback static received");
    DoorState* door_state = static_cast<DoorState*>(thisptr);
    door_state->callback_endswitches(message);
}
