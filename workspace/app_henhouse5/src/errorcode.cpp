#include "errorcode.h"
#include <zephyr/sys/printk.h>

ErrorCode::ErrorCode(Instance instance, Code code, uint8_t number) :
    m_instance(instance),
    m_code(code),
    m_number(number),
    m_serialized(0)
{
    if (m_instance != Instance::INVALID && m_code != Code::success) {
        m_serialized += ((m_code && 0xFF) << 0);
        m_serialized += ((m_number && 0xFF) << 8);
        m_serialized += ((m_instance && 0xFF) << 24);
        printk("ERROR: Instance=%u, Code=%u, Number=%u\n", m_instance, m_code, m_number);
    }
}

bool ErrorCode::hasFailed() {
    return m_code != Code::success;
}