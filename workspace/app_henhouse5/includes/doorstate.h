#pragma once

#include "thread.h"
#include "pubsub.h"
#include "errorcode.h"
#include "inputpin.h"

enum DoorStateEnum : uint32_t {
    UNINITIALIZED = 0,
    OPEN = 1,
    CLOSED = 2,
    INVALID = 3,
    OPENING = 4,
    CLOSING = 5,
    UNDEFINED = 6,
};

class DoorState : public Thread, public Publisher<DoorStateEnum> {
    public:

        DoorState(const ErrorCode::Instance instance, InputPin& endswitchTop, InputPin& endswitchBottom, k_thread_stack_t * stack, const uint32_t stack_size, const uint32_t priority);
        virtual ~DoorState() {};
        virtual const ErrorCode run_internal();
        const DoorStateEnum get();

        void callback_endswitches(uint32_t message);

    protected:
        DoorStateEnum m_state;
        InputPin& m_endswitch_top;
        InputPin& m_endswitch_bottom;
        static const uint32_t m_n_endswitches_queue_max_entries = 4;
        char m_endswitches_queue_buffer[m_n_endswitches_queue_max_entries * sizeof(uint32_t)];
        struct k_msgq m_endswitches_queue;

        void update_state();
};

void static_callback_endswitches(void* thisptr, uint32_t message);
