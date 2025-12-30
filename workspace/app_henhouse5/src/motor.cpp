#include "motor.h"
#include <zephyr/sys/printk.h>
#include <cmath>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Motor, LOG_LEVEL_INF);

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
    const bool dir_state = speed >= 0;
    
    
    ret = m_brake_pin.set(brake_state);
    if (ret.hasFailed()) {
        return ret;
    }
    ret = m_dir_pin.set(dir_state);
    if (ret.hasFailed()) {
        return ret;
    }
    ret = m_pwm.setDutyCycle(duty_cycle);
    if (ret.hasFailed()) {
        return ret;
    }
    
    if (brake_state) {
        LOG_INF("Motor stopped");
    }
    else {
        LOG_INF("PWM %u%%, brake = %u, dir = %u",
            static_cast<uint32_t>(duty_cycle * 100.0f), brake_state, dir_state);
    }

    return ErrorCode(m_instance, ErrorCode::Code::success);;
}

const ErrorCode Motor::testMotor() {
    const float min_speed = -1.0;
    const float max_speed = 1.0;
    const float zero_speed = 0.0;
    const float step = 0.1;
    float speed = zero_speed;
    ErrorCode ec;
    
    for (; speed < max_speed; speed += step) {
        ec = setSpeed(speed);
        if (ec.hasFailed()) {
            return ec;
        }
        k_sleep(K_MSEC(1000));
    }
    for (; speed > min_speed; speed -= step) {
        ec = setSpeed(speed);
        if (ec.hasFailed()) {
            return ec;
        }
        k_sleep(K_MSEC(1000));
    }

    return ErrorCode(m_instance, ErrorCode::Code::success);
}