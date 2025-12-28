#ifndef __BUTTONPIN_H
#define __BUTTONPIN_H

#include <zephyr/drivers/gpio.h>
#include "errorcode.h"

class Button {
    public:
        Button(ErrorCode::Instance instance, const struct gpio_dt_spec pin);
        virtual ~Button() {};
        virtual const ErrorCode init();

        // virtual const ErrorCode register_callback();
        virtual const bool get();
    protected:
        const ErrorCode::Instance m_instance;
        const struct gpio_dt_spec m_pin;
        bool m_value;
        struct gpio_callback m_pin_cb_data;
};

#endif