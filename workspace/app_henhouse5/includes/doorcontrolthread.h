#pragma once

#include "errorcode.h"
#include "doorcontrol.h"


enum class DoorCommanderEnum : uint32_t {
    UNDEFINED = 0,
    AUTO = 1,
    MANUAL = 2,
    MQTT = 3,
};

class DoorControlThread : public Thread {
    public:
        DoorControlThread(ErrorCode::Instance instance, DoorControl & doorControl, k_thread_stack_t * stack, const uint32_t stack_size, const uint32_t priority);
        virtual ~DoorControlThread() {};
        virtual const ErrorCode run_internal();

        void openClose(bool do_open, DoorCommanderEnum commander);

    protected:
        const ErrorCode::Instance m_instance;
        DoorControl & m_doorControl;
        DoorCommanderEnum m_lastCommander;

        static const uint32_t m_command_queue_max_entries = 4;
        char m_command_queue_buffer[m_command_queue_max_entries * sizeof(bool)];
        struct k_msgq m_command_queue;
};
