#pragma once

#include <zephyr/drivers/gpio.h>
#include <zephyr/zbus/zbus.h>
#include "errorcode.h"

struct InputPin_PrivateCallbackContainer {
    /**
     * The callback container is used privately within the Inputpin class. 
     * It holds the GPIO callback data that is sent to the callback function
     * which uses CONTAINER_OF functionality to get the Inputpin class.
     * Yes, complicated... 
     */
    struct gpio_callback callback_data;
    ErrorCode::Instance m_instance;
    const struct zbus_channel* m_zbus_channel;
};

struct InputPin_zbus_message {
    uint32_t value;
};

class InputPin {
    public:
        InputPin(ErrorCode::Instance instance, const struct gpio_dt_spec pin, const uint32_t zbus_id);
        virtual ~InputPin() {};
        const ErrorCode init();
        
        const bool get();
        // const ErrorCode addObserver();
        const ErrorCode await(k_timeout_t timeout);
        
        
        protected:
        const ErrorCode::Instance m_instance;
        const struct gpio_dt_spec m_pin;
        bool m_value;
        const struct zbus_channel* m_zbus_channel;
        struct InputPin_PrivateCallbackContainer m_cb_container;

};

