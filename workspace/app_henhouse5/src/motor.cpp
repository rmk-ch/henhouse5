#include "motor.h"
#include <zephyr/sys/printk.h>
#include <cmath>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Motor, LOG_LEVEL_WRN);

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

const ErrorCode Motor::setSpeed(const int8_t percent) {
    ErrorCode ret;
    
    const int8_t duty_cycle = std::abs(percent);
    const int8_t min_duty_cycle = 10; // torque is low, even without load
    const bool brake_state = duty_cycle <= min_duty_cycle;    
    const bool dir_state = percent >= 0;
    
    
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
            duty_cycle, brake_state, dir_state);
    }

    return ErrorCode(m_instance, ErrorCode::Code::success);;
}

const ErrorCode Motor::testMotor() {
    const int8_t min_speed = -100;
    const int8_t max_speed = 100;
    const int8_t zero_speed = 0;
    const int8_t step = 10;
    int8_t speed = zero_speed;
    ErrorCode ec;
    
    for (; speed < max_speed; speed += step) {
        ec = setSpeed(speed);
        if (ec.hasFailed()) {
            return ec;
        }
        k_sleep(K_MSEC(200));
    }
    for (; speed > min_speed; speed -= step) {
        ec = setSpeed(speed);
        if (ec.hasFailed()) {
            return ec;
        }
        k_sleep(K_MSEC(200));
    }

    return ErrorCode(m_instance, ErrorCode::Code::success);
}