#include "pwm.h"
#include <cmath>
#include <algorithm>


Pwm::Pwm(const ErrorCode::Instance instance, const struct pwm_dt_spec pwm) :
    m_instance(instance),
    m_pwm(pwm)
    {
}

const ErrorCode Pwm::init() {
    
	if (!pwm_is_ready_dt(&m_pwm)) {
        return ErrorCode(m_instance, ErrorCode::not_ready, 1);
	}
	
    return ErrorCode(m_instance, ErrorCode::Code::success);
}

const ErrorCode Pwm::setDutyCycle(const uint8_t percent) {
    const uint8_t min_duty_cycle = 0;
    const uint8_t max_duty_cycle = 101; // add safety margin to allow later saturation without throwing too many errors due to rounding/floating point
    if (percent > max_duty_cycle || percent < min_duty_cycle) {
        return ErrorCode(m_instance, ErrorCode::Code::invalid_argument, 1);
    }

    uint32_t pulse_width = (static_cast<uint32_t>(percent) * m_period) /100;
    pulse_width = std::min(pulse_width, m_period); // saturate to maximum!

    int32_t ret = pwm_set_dt(&m_pwm, m_period, pulse_width);
    if (ret) {
        return ErrorCode(m_instance, ErrorCode::Code::runtime, 2);
    }

    return ErrorCode(m_instance, ErrorCode::Code::success);
}