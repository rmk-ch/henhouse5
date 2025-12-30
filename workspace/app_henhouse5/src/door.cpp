#include "door.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Door, LOG_LEVEL_INF);

Door::Door(ErrorCode::Instance instance, Motor& motor, InputPin& endswitch_top, InputPin& endswitch_bottom) :
m_instance(instance),
m_motor(motor),
m_endswitch_top(endswitch_top),
m_endswitch_bottom(endswitch_bottom) {
    
    }

const ErrorCode Door::init() {
    return ErrorCode(m_instance, ErrorCode::success);
}
const ErrorCode Door::openClose(bool do_open) {
    InputPin* startPos = nullptr;
    InputPin* endPos = nullptr;
    ErrorCode::Code errorcode_door_start_wrong;
    ErrorCode::Code errorcode_door_end_wrong;
    float speed;
    ErrorCode ec;

    if (do_open) {
        LOG_INF("Opening door...");
        startPos = &m_endswitch_top;
        endPos = &m_endswitch_bottom;
        errorcode_door_start_wrong = ErrorCode::Code::door_start_opening_wrong;
        errorcode_door_end_wrong = ErrorCode::Code::door_end_opening_wrong;
        speed = +1.0f;
    }
    else {
        LOG_INF("Closing door...");
        startPos = &m_endswitch_bottom;
        endPos = &m_endswitch_top;
        errorcode_door_start_wrong = ErrorCode::Code::door_start_closing_wrong;
        errorcode_door_end_wrong = ErrorCode::Code::door_end_closing_wrong;
        speed = -1.0f;
    }
    
    if (!startPos->get()) {
        LOG_INF("Start position wrong!");
        // not returning, we want to continue no matter the init state!
        ErrorCode(m_instance, errorcode_door_start_wrong, 1);
    }
    
    // now moving the motors...
    m_motor.setSpeed(speed);
    
    // awaiting end switch with timeout
    LOG_INF("awaiting end switch with timeout");
    ec = endPos->await(K_SECONDS(10));
    // Not returning yet, we want to stop motors first...
    if (ec.m_code == ErrorCode::Code::timeout) {
        LOG_WRN("Awaiting end position timed out!");
    }
    m_motor.setSpeed(0.0f);
    
    return ec; // we are mainly interested if the target was reached within time        
}