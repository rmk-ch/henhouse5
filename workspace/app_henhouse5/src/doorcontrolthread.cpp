#include "doorcontrolthread.h"
#include <zephyr/shell/shell.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(DoorControlThread, LOG_LEVEL_INF);

DoorControlThread* DoorControlThread::self = nullptr;

DoorControlThread::DoorControlThread(ErrorCode::Instance instance, DoorControl & doorControl, k_thread_stack_t * stack, const uint32_t stack_size, const uint32_t priority) :
    Thread(instance, stack, stack_size, priority, "DoorControlThread"),
    m_instance(instance),
    m_doorControl(doorControl),
    m_lastCommander(DoorCommanderEnum::UNDEFINED)
{    
    if (self != nullptr) {
        LOG_ERR("DoorControlThread can only exist once!");
    }
    self = this;
}

const ErrorCode DoorControlThread::run() {
	LOG_DBG("DoorControlThread started");
    k_msgq_init(&m_commandQueue, m_commandQueueBuffer, sizeof(bool), m_commandQueueMaxEntries);

    while(true) {
        bool do_open;
        k_msgq_get(&m_commandQueue, &do_open, K_FOREVER);
        m_doorControl.openClose(do_open);
    }

    return ErrorCode(Thread::m_instance, ErrorCode::Code::unexpected_exit, 1);
}

void DoorControlThread::openClose(bool do_open, DoorCommanderEnum commander) {
	LOG_INF("Received openclose (do_open=%u) command from %u", do_open, static_cast<uint32_t>(commander));
    k_msgq_put(&m_commandQueue, &do_open, K_NO_WAIT);
}



/////////////////////////////////////////////////////////////////////
// Shell commands
/////////////////////////////////////////////////////////////////////
static int shellCmd_openDoor(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	DoorControlThread::getInstance()->openClose(true, DoorCommanderEnum::SHELL);

	return 0;
}

static int shellCmd_closeDoor(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	DoorControlThread::getInstance()->openClose(false, DoorCommanderEnum::SHELL);

	return 0;
}

static int shellCmd_cycleOpenCloseDoor(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	// int nCycles = std::stoi(argv[0]); // current leading to HARD FAULT
	int nCycles = 1;
    LOG_INF("Cycle door open/close %u times", nCycles);

    for (int i = 0; i < nCycles; i++) {
        DoorControlThread::getInstance()->openClose(true, DoorCommanderEnum::SHELL);
        k_sleep(K_SECONDS(1));
        DoorControlThread::getInstance()->openClose(false, DoorCommanderEnum::SHELL);
        k_sleep(K_SECONDS(1));
    }

	return 0;
}


/* Creating subcommands (level 1 command) array for command "demo". */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_door,
        SHELL_CMD(open, NULL, "Open door", shellCmd_openDoor),
        SHELL_CMD(close, NULL, "Close door", shellCmd_closeDoor),
        SHELL_CMD(cycle, NULL, "Cycle openclose", shellCmd_cycleOpenCloseDoor),
        SHELL_SUBCMD_SET_END
);
/* Creating root (level 0) command "door" */
SHELL_CMD_REGISTER(door, &sub_door, "Door open/close", NULL);