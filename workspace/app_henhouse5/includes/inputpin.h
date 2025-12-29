#ifndef __INPUTPIN_H
#define __INPUTPIN_H

#include <zephyr/drivers/gpio.h>
#include "errorcode.h"

struct InputPin_PrivateCallbackContainer {
    /**
     * The callback container is used privately within the Inputpin class. 
     * It holds the GPIO callback data that is sent to the callback function
     * which uses CONTAINER_OF functionality to get the Inputpin class.
     * Yes, complicated... 
     */
    struct gpio_callback callback_data;
    class InputPin * inputpin;
};

class InputPin {
    public:
        InputPin(ErrorCode::Instance instance, const struct gpio_dt_spec pin);
        virtual ~InputPin() {};
        const ErrorCode init();
        
        // virtual const ErrorCode register_callback();
        const bool get();
        const ErrorCode await(const k_timeout_t timeout); // Use K_FOREVER or K_MSEC/K_SECONDS/...
        struct k_event m_event;
        protected:
        const ErrorCode::Instance m_instance;
        const struct gpio_dt_spec m_pin;
        bool m_value;
        struct InputPin_PrivateCallbackContainer m_cb_container;

};

#endif