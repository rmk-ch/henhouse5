#pragma once

#include "errorcode.h"
#include "motor.h"
#include "doorstate.h"

class DoorControl {
    public:
        DoorControl(ErrorCode::Instance instance, Motor& motor, DoorState &doorState);
        virtual ~DoorControl() {};
        const ErrorCode init();
        const ErrorCode openClose(bool do_open);

    protected:
        const ErrorCode::Instance m_instance;
        Motor& m_motor;
        DoorState& m_doorState;
};
