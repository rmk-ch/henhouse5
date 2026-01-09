#include "thread.h"
#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Thread, LOG_LEVEL_INF);

static void static_run(void * ptr1, void * ptr2, void * ptr3) {
	Thread* thread = static_cast<Thread*>(ptr1);
	LOG_DBG("Starting thread");
	thread->run_internal();
}


Thread::Thread(const ErrorCode::Instance instance, k_thread_stack_t * stack_area, const uint32_t stack_size, const int32_t priority, const std::string& name) :
    m_instance(instance),
	m_stack_area(stack_area), 
	m_stack_size(stack_size),
	m_priority(priority),
	m_name(name)
    {
    /*
    k_tid_t k_thread_create 	( 	struct k_thread * 	new_thread,
		k_thread_stack_t * 	stack,
		size_t 	stack_size,
		k_thread_entry_t 	entry,
		void * 	p1,
		void * 	p2,
		void * 	p3,
		int 	prio,
		uint32_t 	options,
		k_timeout_t 	delay )
    */
}

const ErrorCode Thread::init() {
	m_tid = k_thread_create(&m_thread_data, m_stack_area, m_stack_size, static_run, static_cast<void*>(this), nullptr, nullptr, m_priority, 0, K_NO_WAIT);
	k_thread_name_set(&m_thread_data, m_name.c_str());
	return ErrorCode(m_instance, ErrorCode::Code::success);
}


const ErrorCode Thread::run_internal() {
	return ErrorCode(m_instance, ErrorCode::Code::not_implemented, 2);
}