#include "motor.h"
#include <zephyr/shell/shell.h>
#include <cmath>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(Motor, LOG_LEVEL_DBG);


Motor* Motor::self = nullptr;

Motor::Motor(const ErrorCode::Instance instance, Pwm &pwm, OutputPin &brake_pin, OutputPin &dir_pin) :
    m_instance(instance),
    m_pwm(pwm),
    m_brake_pin(brake_pin),
    m_dir_pin(dir_pin)
{
    if (self != nullptr) {
        LOG_ERR("Motor can only exist once!");
    }
    self = this;

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
    ec = setSpeed(0);
    if (ec.hasFailed()) {
        return ec;
    }


    return ErrorCode(m_instance, ErrorCode::Code::success);
}


/////////////////////////////////////////////////////////////////////
// Shell commands
/////////////////////////////////////////////////////////////////////
static int shellCmd_testMotor(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	Motor::getInstance()->testMotor();

	return 0;
}
static int shellCmd_stop(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	Motor::getInstance()->setSpeed(0);

	return 0;
}


/* Creating subcommands (level 1 command) array for command "demo". */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_motor,
        SHELL_CMD(test, NULL, "Test (spin up and down in both directions)", shellCmd_testMotor),
        SHELL_CMD(stop, NULL, "Stop", shellCmd_stop),
        SHELL_SUBCMD_SET_END
);
/* Creating root (level 0) command "door" */
SHELL_CMD_REGISTER(motor, &sub_motor, "Motor", NULL);