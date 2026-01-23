#include "doorstate.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(DoorState, LOG_LEVEL_INF);

DoorState::DoorState(const ErrorCode::Instance instance, InputPin& endswitchTop, InputPin& endswitchBottom, k_thread_stack_t * stack, const uint32_t stack_size, const uint32_t priority) :
    Thread(instance, stack, stack_size, priority, "DoorState"),
    Publisher(instance),
    m_state(DoorStateEnum::UNINITIALIZED),
    m_endswitch_top(endswitchTop),
    m_endswitch_bottom(endswitchBottom){
}


const ErrorCode DoorState::run_internal() {
	LOG_INF("Doorstate started");
    update_state();
    k_msgq_init(&m_endswitches_queue, m_endswitches_queue_buffer, sizeof(ErrorCode::Instance), m_n_endswitches_queue_max_entries);

    while(true) {
        uint32_t pin_instance_id;
        k_msgq_get(&m_endswitches_queue, &pin_instance_id, K_FOREVER);
        update_state();
    }

    return ErrorCode(Thread::m_instance, ErrorCode::Code::success);
}

void DoorState::update_state() {
    bool state_top = m_endswitch_top.get();
    bool state_bottom = m_endswitch_bottom.get();

    DoorStateEnum prev_state = m_state;
    if (state_top && !state_bottom) {
        m_state = DoorStateEnum::OPEN;
        LOG_INF("Door is open");
    }
    else if(!state_top && state_bottom) {
        m_state = DoorStateEnum::CLOSED;
        LOG_INF("Door is closed");
    }
    else if(!state_top && !state_bottom) {
        if (prev_state == DoorStateEnum::CLOSED) {
            m_state = DoorStateEnum::OPENING;
            LOG_INF("Door is opening");
        }
        else if (prev_state == DoorStateEnum::OPEN) {
            m_state = DoorStateEnum::CLOSING;
            LOG_INF("Door is closing");
        }
        else {
            m_state = DoorStateEnum::UNDEFINED;
            LOG_INF("Door is in undefined state");            
        }
    }
    else {
        m_state = DoorStateEnum::INVALID;
        LOG_ERR("Door state is invalid - both endswitches are set!");
    }

    if (m_state != prev_state) {
        _notify(m_state);
    }
}

const DoorStateEnum DoorState::get() {
    return m_state;
}

void DoorState::callback_endswitches(uint32_t pin_instance_id) {
    k_msgq_put(&m_endswitches_queue, &pin_instance_id, K_NO_WAIT);
}
