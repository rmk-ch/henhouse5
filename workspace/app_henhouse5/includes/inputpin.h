#pragma once

#include <ecpp/static_vector.hpp>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include "pubsub.h"
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
    ecpp::static_vector<callback_entry, 4> m_callbacks;
};
class InputPin {
    public:
        InputPin(ErrorCode::Instance instance, const struct gpio_dt_spec pin, const uint32_t callback_event = GPIO_INT_EDGE_TO_ACTIVE);
        virtual ~InputPin() {};

        const ErrorCode init();
        
        const bool get();

        const ErrorCode registerCallback(static_fcn_uint32* function, void * instance);
        
    protected:
        const ErrorCode::Instance m_instance;
        const struct gpio_dt_spec m_pin;
        struct InputPin_PrivateCallbackContainer m_cb_container;
        const uint32_t m_callback_event;

};
