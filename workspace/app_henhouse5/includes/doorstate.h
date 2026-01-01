#pragma once

#include "thread.h"
#include "errorcode.h"
#include "inputpin.h"


class DoorState : public Thread {
    public:
        enum STATE {
            UNDEFINED = 0,
            OPEN = 1,
            CLOSED = 2,
            INVALID = 3,
        };

        DoorState(const ErrorCode::Instance instance, InputPin& endswitchTop, InputPin& endswitchBottom, k_thread_stack_t * stack, const uint32_t stack_size, const uint32_t priority);
        virtual ~DoorState() {};
        virtual const ErrorCode run();
        const ErrorCode await(k_timeout_t timeout);
        const STATE get();

        void callback_endswitches(uint32_t message);

    protected:
        InputPin& m_endswitch_top;
        InputPin& m_endswitch_bottom;
        static const uint32_t m_n_endswitches_queue_max_entries = 4;
        char m_endswitches_queue_buffer[m_n_endswitches_queue_max_entries * sizeof(uint32_t)];
        struct k_msgq m_endswitches_queue;
};

void static_callback_endswitches(void* thisptr, uint32_t message);
