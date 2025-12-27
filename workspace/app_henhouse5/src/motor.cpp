#include "motor.h"
#include <zephyr/sys/printk.h>
#include <cmath>


Motor::Motor(const ErrorCode::Instance instance) :
    m_instance(instance),
    m_pwm(ErrorCode::Instance::motor_pwm, PWM_DT_SPEC_GET(DT_NODELABEL(green_pwm_led))),
    m_brake_pin(ErrorCode::Instance::motor_brake_pin, GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_brake), gpios)),
    m_dir_pin(ErrorCode::Instance::motor_dir_pin, GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_dir), gpios))
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

    ret = setSpeedPercent(0.0);
    if (ret.hasFailed())
        return ret;

    return ErrorCode(m_instance, ErrorCode::Code::success);
}

const ErrorCode Motor::setSpeedPercent(float speed) {
    ErrorCode ret;
    
    const float speed_tolerance = 0.0001;
    bool brake_state = (speed >= -speed_tolerance && speed <= speed_tolerance);
    if (brake_state) {
        printk("Motor stopping\n");
    }
    
    bool dir_state = speed >= 0;
    float duty_cycle = std::fabs(speed);
    
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