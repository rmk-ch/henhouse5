#include "inputpin.h"
#include <functional>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(InputPin, LOG_LEVEL_DBG);


void inputpin_isr(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    // why so complicated to get InputPin class? Ask Zephyr devs... Details in description of InputPin_PrivateCallbackContainer struct.
    struct InputPin_PrivateCallbackContainer* cb_container = CONTAINER_OF(cb, struct InputPin_PrivateCallbackContainer, callback_data);
	LOG_DBG("InputPin ISR instance %u", cb_container->m_instance);
	InputPin* this_ptr = cb_container->this_ptr;
	this_ptr->_notify(cb_container->m_instance);
};

InputPin::InputPin(ErrorCode::Instance instance, const struct gpio_dt_spec pin, const uint32_t gpio_event) :
	Publisher(instance),
    m_instance(instance),
    m_pin(pin),
	m_gpio_event(gpio_event)
    {
        m_cb_container.m_instance = m_instance;
		m_cb_container.this_ptr = this;
}

const ErrorCode InputPin::init() {
    int ret;

	if (!gpio_is_ready_dt(&m_pin)) {
		return ErrorCode(m_instance, ErrorCode::not_ready, 1);
	}

	ret = gpio_pin_configure_dt(&m_pin, GPIO_INPUT);
	if (ret != 0) {
        return ErrorCode(m_instance, ErrorCode::invalid_config, 2);
	}

	ret = gpio_pin_interrupt_configure_dt(&m_pin, m_gpio_event);
	if (ret != 0) {
        return ErrorCode(m_instance, ErrorCode::invalid_config, 3);
	}


	gpio_init_callback(&m_cb_container.callback_data, inputpin_isr, BIT(m_pin.pin));

	ret = gpio_add_callback(m_pin.port, &m_cb_container.callback_data);
	if (ret != 0) {
		return ErrorCode(m_instance, ErrorCode::invalid_config, 4);
	}

    return ErrorCode(m_instance, ErrorCode::success);
}

const bool InputPin::get() {
    bool value = gpio_pin_get(m_pin.port, m_pin.pin);
    return value;
}
