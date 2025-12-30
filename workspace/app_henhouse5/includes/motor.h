#ifndef __MOTOR_H
#define __MOTOR_H

#include "outputpin.h"
#include "pwm.h"
#include "errorcode.h"


class Motor {
    public:
        Motor(const ErrorCode::Instance instance, Pwm &pwm, OutputPin &brake_pin, OutputPin &dir_pin);
    	virtual ~Motor() {};
        const ErrorCode init();

        const ErrorCode setSpeed(const float speed);
        const ErrorCode testMotor();

    protected:
        const ErrorCode::Instance m_instance;
        Pwm& m_pwm;
        OutputPin& m_brake_pin;
        OutputPin& m_dir_pin;
};

#endif