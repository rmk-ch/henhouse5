#ifndef __BUTTONPIN_H
#define __BUTTONPIN_H

#include <zephyr/drivers/gpio.h>
#include "errorcode.h"

struct callback_container {
    /**
     * The callback container is used privately within the Button class. 
     * It holds the GPIO callback data that is sent to the callback function
     * which uses CONTAINER_OF functionality to get the button class.
     * Yes, complicated... 
     */
    struct gpio_callback callback_data;
    const class Button * button;
};

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

        struct callback_container m_cb_container;
};

#endif