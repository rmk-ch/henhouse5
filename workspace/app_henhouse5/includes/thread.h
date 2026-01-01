#pragma once

#include <zephyr/kernel/thread.h>
#include <zephyr/kernel/thread_stack.h>

#include "errorcode.h"

class Thread {
    public:
        Thread(const ErrorCode::Instance instance, k_thread_stack_t * stack_area, const uint32_t stack_size, const int32_t priority);
        virtual ~Thread() {};
        virtual const ErrorCode init();
        virtual const ErrorCode run();

    protected:
        const ErrorCode::Instance m_instance;
        k_thread_stack_t * m_stack_area;
        const uint32_t m_stack_size;
        const int32_t m_priority;
        k_tid_t m_tid;
        struct k_thread m_thread_data;
};

