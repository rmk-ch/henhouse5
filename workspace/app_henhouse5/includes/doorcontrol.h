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

        void callback_doorstate(DoorStateEnum state);

    protected:
        const ErrorCode::Instance m_instance;
        Motor& m_motor;
        DoorState& m_doorState;
        atomic_t m_target_doorstate;
        struct k_sem m_semaphore_target_doorstate_reached;


        static const uint32_t m_n_endswitches_queue_max_entries = 4;
        char m_endswitches_queue_buffer[m_n_endswitches_queue_max_entries * sizeof(uint32_t)];
        struct k_msgq m_endswitches_queue;
};
