#include "motor.h"
#include <zephyr/sys/printk.h>
#include <cmath>


Motor::Motor(const ErrorCode::Instance instance, Pwm &pwm, OutputPin &brake_pin, OutputPin &dir_pin) :
    m_instance(instance),
    m_pwm(pwm),
    m_brake_pin(brake_pin),
    m_dir_pin(dir_pin)
{

}

const ErrorCode Motor::init() {
    ErrorCode ret = ErrorCode();
    
    ret = m_brake_pin.init(true, 1);
    if (ret.hasFailed()) {
        return ret;
    }
    ret = m_dir_pin.init(true, 0);
    if (ret.hasFailed()) {
        return ret;
    }
    
    ret = m_pwm.init();
    if (ret.hasFailed())
        return ret;

    ret = setSpeed(0.0);
    if (ret.hasFailed())
        return ret;

    return ErrorCode(m_instance, ErrorCode::Code::success);
}

const ErrorCode Motor::setSpeed(const float speed) {
    ErrorCode ret;
    
    const float duty_cycle = std::fabs(speed);
    const float min_duty_cycle = 0.1f; // torque is low, even without load
    const bool brake_state = duty_cycle <= min_duty_cycle;
    if (brake_state) {
        printk("Motor stopping\n");
    }
    
    const bool dir_state = speed >= 0;
    
    ret = m_pwm.setDutyCycle(duty_cycle);
    if (ret.hasFailed()) {
        return ret;
    }
            
    ret = m_brake_pin.set(brake_state);
    if (ret.hasFailed()) {
        return ret;
    }
    ret = m_dir_pin.set(dir_state);
    if (ret.hasFailed()) {
        return ret;
    }


    
    printk("Motor: PWM %u%%, brake = %u, dir = %u\n",
            static_cast<uint32_t>(duty_cycle * 100.0f), brake_state, dir_state);


    return ErrorCode(m_instance, ErrorCode::Code::success);;
}