#include "pwm.h"
#include <cmath>
// #include <zephyr/sys/printk.h>


Pwm::Pwm(const ErrorCode::Instance instance, const struct pwm_dt_spec pwm) :
    m_instance(instance),
    m_pwm(pwm)
    {
}

const ErrorCode Pwm::init() {
    
	if (!pwm_is_ready_dt(&m_pwm)) {
        return ErrorCode(m_instance, ErrorCode::not_ready, 1);
	}
	
	// MOE must be enabled!
    uint32_t * puint32;
	puint32 = (uint32_t*) 0x40012c44;
	*puint32 |= 1<<15;

    return ErrorCode(m_instance, ErrorCode::Code::success);
}

const ErrorCode Pwm::setDutyCycle(float zeroToOne) {
    const float min_duty_cycle = static_cast<float>(0.0);
    const float max_duty_cycle = static_cast<float>(1.0);
    if (zeroToOne > max_duty_cycle || zeroToOne < min_duty_cycle) {
        return ErrorCode(m_instance, ErrorCode::Code::invalid_argument, 1);
    }
    uint32_t pulse_width = static_cast<uint32_t>(std::lround(zeroToOne * static_cast<float>(m_period)));

    int32_t ret = pwm_set_dt(&m_pwm, m_period, pulse_width);
    if (ret) {
        return ErrorCode(m_instance, ErrorCode::Code::runtime, 2);
    }

    return ErrorCode(m_instance, ErrorCode::Code::success);
}