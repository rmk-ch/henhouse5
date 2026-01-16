#pragma once

#include <zephyr/sys/atomic.h>
#include "errorcode.h"
#include "motor.h"
#include "doorstate.h"

class DoorControl {
    public:
        DoorControl(ErrorCode::Instance instance, Motor& motor, DoorState &doorState);
        virtual ~DoorControl() {};
        const ErrorCode init();
        const ErrorCode openClose(bool do_open);

        void callback_doorstate(DoorStateEnum message);

    protected:
        const ErrorCode::Instance m_instance;
        Motor& m_motor;
        DoorState& m_doorState;
        atomic_t m_target_doorstate;
        struct k_sem m_semaphore_target_doorstate_reached;
};

void static_callback_doorstate(void* thisptr, DoorStateEnum message);
void static_callback_openclosedoor(void* thisptr, bool do_open);
