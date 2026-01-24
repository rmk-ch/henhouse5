#pragma once

#include "errorcode.h"
#include "doorcontrol.h"


enum class DoorCommanderEnum : uint32_t {
    UNDEFINED = 0,
    AUTO = 1,
    MANUAL = 2,
    MQTT = 3,
    SHELL = 4,
};

class DoorControlThread : public Thread {
    public:
        // Singleton instance to allow access in shell commands
        static DoorControlThread* self;
        static DoorControlThread* getInstance() {
            return self;
        }

        
        DoorControlThread(ErrorCode::Instance instance, DoorControl & doorControl, k_thread_stack_t * stack, const uint32_t stackSize, const uint32_t priority);
        virtual ~DoorControlThread() {};
        virtual const ErrorCode run();

        void openClose(bool do_open, DoorCommanderEnum commander);



    protected:
        const ErrorCode::Instance m_instance;
        DoorControl & m_doorControl;
        DoorCommanderEnum m_lastCommander;

        static const uint32_t m_commandQueueMaxEntries = 4;
        char m_commandQueueBuffer[m_commandQueueMaxEntries * sizeof(bool)];
        struct k_msgq m_commandQueue;
};
