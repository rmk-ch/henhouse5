#ifndef __PWM_H
#define __PWM_H

#include <zephyr/drivers/pwm.h>
#include "errorcode.h"

class Pwm{
    public:
        Pwm(const ErrorCode::Instance instance, const struct pwm_dt_spec pwm);
        virtual ~Pwm() {};

        const ErrorCode init();
        const ErrorCode setDutyCycle(const float zeroToOne);

    protected:
        const ErrorCode::Instance m_instance;
        const struct pwm_dt_spec m_pwm;

        const uint32_t m_period = PWM_USEC(40);
};

#endif