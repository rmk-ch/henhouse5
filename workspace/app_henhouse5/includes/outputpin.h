#ifndef __OUTPUTPIN_H
#define __OUTPUTPIN_H

#include <zephyr/drivers/gpio.h>
#include "errorcode.h"

class OutputPin {
    public:
        OutputPin(ErrorCode::Instance instance, const struct gpio_dt_spec pin);
        virtual ~OutputPin() {};
        virtual const ErrorCode init(const bool is_active_high, const bool initial_value = false);

        virtual const ErrorCode set(const bool value);
        virtual const bool get();
    protected:
        const ErrorCode::Instance m_instance;
        const struct gpio_dt_spec m_pin;
        bool m_value;
};

#endif