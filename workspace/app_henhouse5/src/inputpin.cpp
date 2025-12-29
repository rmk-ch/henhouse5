#include "inputpin.h"


static const uint32_t EVENT_MSG = 0x1;
static void inputpin_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    // why so complicated to get InputPin class? Ask Zephyr devs... Details in description of InputPin_PrivateCallbackContainer struct.
    struct InputPin_PrivateCallbackContainer* cb_container = CONTAINER_OF(cb, struct InputPin_PrivateCallbackContainer, callback_data);
    InputPin* inputpin = cb_container->inputpin;
	printk("This: %p InputPin %p / %u pressed at %" PRIu32 "\n", inputpin, dev, pins, k_cycle_get_32());
	k_event_post(&inputpin->m_event, EVENT_MSG);
};

InputPin::InputPin(ErrorCode::Instance instance, const struct gpio_dt_spec pin) :
    m_instance(instance),
    m_pin(pin),
    m_value(0)
    {
        m_cb_container.inputpin = this;
}

const ErrorCode InputPin::init() {
    int ret;
	k_event_init(&m_event);

	if (!gpio_is_ready_dt(&m_pin)) {
		printk("Error: m_pin device %s is not ready\n",
		       m_pin.port->name);
		return ErrorCode(m_instance, ErrorCode::not_ready, 1);
	}

	ret = gpio_pin_configure_dt(&m_pin, GPIO_INPUT);
	if (ret != 0) {
		printk("Error %d: failed to configure %s pin %d\n",
		       ret, m_pin.port->name, m_pin.pin);
        return ErrorCode(m_instance, ErrorCode::invalid_config, 1);
	}

	ret = gpio_pin_interrupt_configure_dt(&m_pin,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
		printk("Error %d: failed to configure interrupt on %s pin %d\n",
			ret, m_pin.port->name, m_pin.pin);
        return ErrorCode(m_instance, ErrorCode::invalid_config, 2);
	}


	gpio_init_callback(&m_cb_container.callback_data, inputpin_pressed, BIT(m_pin.pin));
	gpio_add_callback(m_pin.port, &m_cb_container.callback_data);

    return ErrorCode(m_instance, ErrorCode::success);
}

const bool InputPin::get() {
    m_value = gpio_pin_get(m_pin.port, m_pin.pin);
    return m_value;
}


const ErrorCode InputPin::await(const k_timeout_t  timeout) {
	uint32_t events = k_event_wait(&m_event, EVENT_MSG, false, timeout);
    if (events == 0) {
        return ErrorCode(m_instance, ErrorCode::Code::timeout, 1);
	}
    
	return ErrorCode(m_instance, ErrorCode::Code::success);
}