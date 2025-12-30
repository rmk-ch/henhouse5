#include "errorcode.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ErrorCode, LOG_LEVEL_INF);

ErrorCode::ErrorCode(Instance instance, Code code, uint8_t number) :
    m_instance(instance),
    m_code(code),
    m_number(number)
{
    if (m_instance != Instance::INVALID && m_code != Code::success) {
        LOG_ERR("Instance=%u, Code=%u, Number=%u", m_instance, m_code, m_number);
    }
}

const uint32_t ErrorCode::serialize() {
    uint32_t serialized = 0;
    serialized += ((m_code && 0xFF) << 0);
    serialized += ((m_number && 0xFF) << 8);
    serialized += ((m_instance && 0xFF) << 24);
    return serialized;
}

bool ErrorCode::hasFailed() {
    return m_code != Code::success;
}