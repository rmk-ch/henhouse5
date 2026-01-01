#include "doorcontrol.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(DoorControl, LOG_LEVEL_INF);

DoorControl::DoorControl(ErrorCode::Instance instance, Motor& motor, DoorState &doorState) :
    m_instance(instance),
    m_motor(motor),
    m_doorState(doorState),
    m_target_doorstate(DoorStateEnum::UNINITIALIZED) {
    
    }

const ErrorCode DoorControl::init() {
    k_sem_init(&m_semaphore_target_doorstate_reached, 0, 1);
    return ErrorCode(m_instance, ErrorCode::success);
}
const ErrorCode DoorControl::openClose(bool do_open) {
    DoorStateEnum expected_start_state;
    ErrorCode::Code errorcode_door_start_wrong;
    int8_t speed_percent = +100;
    ErrorCode ec;

    if (do_open) {
        LOG_INF("Opening door...");
        speed_percent *= +1;
        errorcode_door_start_wrong = ErrorCode::Code::door_start_opening_wrong;
        expected_start_state = DoorStateEnum::CLOSED;
        atomic_set(&m_target_doorstate, DoorStateEnum::OPEN);
    }
    else {
        LOG_INF("Closing door...");
        speed_percent *= -1;
        errorcode_door_start_wrong = ErrorCode::Code::door_start_closing_wrong;
        expected_start_state = DoorStateEnum::OPEN;
        atomic_set(&m_target_doorstate, DoorStateEnum::CLOSED);
    }
    
    if (m_doorState.get() != expected_start_state) {
        LOG_WRN("Start position wrong!");
        // not returning, we want to continue no matter the init state!
        ErrorCode(m_instance, errorcode_door_start_wrong, 1);
    }
    
    // now moving the motors...
    m_motor.setSpeed(speed_percent);
    
    // awaiting end switch with timeout
    int ret = k_sem_take(&m_semaphore_target_doorstate_reached, K_SECONDS(30));
    m_motor.setSpeed(0.0f);

    if (ret == EAGAIN) {
        LOG_WRN("Awaiting doorstate to reach target timed out!");
        return ErrorCode(m_instance, ErrorCode::Code::timeout, 1);
    }
    LOG_INF("Target doorstate reached!");
    return ErrorCode(m_instance, ErrorCode::Code::success);
}



void DoorControl::callback_doorstate(DoorStateEnum message) {
    // thread context: DoorState!
    // therefore, atomic!
    DoorStateEnum state_to_forward = static_cast<DoorStateEnum>(atomic_get(&m_target_doorstate));
    if (message == state_to_forward) {
        k_sem_give(&m_semaphore_target_doorstate_reached);
    }
    else {
        LOG_DBG("Not interested in doorstate=%u", message);
    }
}


void static_callback_doorstate(void* thisptr, DoorStateEnum message) {
	LOG_INF("Callback doorstate received");
    DoorControl* door_control = static_cast<DoorControl*>(thisptr);
    door_control->callback_doorstate(message);
}
