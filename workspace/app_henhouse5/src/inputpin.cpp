#include "inputpin.h"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(InputPin, LOG_LEVEL_DBG);

struct msg {
    uint32_t value;
};

static const uint32_t EVENT_MSG = 0x1;
static void inputpin_isr(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    // why so complicated to get InputPin class? Ask Zephyr devs... Details in description of InputPin_PrivateCallbackContainer struct.
    struct InputPin_PrivateCallbackContainer* cb_container = CONTAINER_OF(cb, struct InputPin_PrivateCallbackContainer, callback_data);
	LOG_DBG("InputPin ISR id%u", cb_container->m_instance);
	zbus_chan_notify(cb_container->m_zbus_channel, K_NO_WAIT);
};

InputPin::InputPin(ErrorCode::Instance instance, const struct gpio_dt_spec pin, const uint32_t zbus_id) :
    m_instance(instance),
    m_pin(pin),
    m_value(0),
	m_zbus_channel(zbus_chan_from_id(zbus_id))
    {
        m_cb_container.m_instance = m_instance;
		m_cb_container.m_zbus_channel = m_zbus_channel;
}

const ErrorCode InputPin::init() {
    int ret;


	if (!gpio_is_ready_dt(&m_pin)) {
		return ErrorCode(m_instance, ErrorCode::not_ready, 1);
	}

	ret = gpio_pin_configure_dt(&m_pin, GPIO_INPUT);
	if (ret != 0) {
        return ErrorCode(m_instance, ErrorCode::invalid_config, 1);
	}

	ret = gpio_pin_interrupt_configure_dt(&m_pin,
					      GPIO_INT_EDGE_TO_ACTIVE);
	if (ret != 0) {
        return ErrorCode(m_instance, ErrorCode::invalid_config, 2);
	}


	gpio_init_callback(&m_cb_container.callback_data, inputpin_isr, BIT(m_pin.pin));
	gpio_add_callback(m_pin.port, &m_cb_container.callback_data);

    return ErrorCode(m_instance, ErrorCode::success);
}

const bool InputPin::get() {
    m_value = gpio_pin_get(m_pin.port, m_pin.pin);
    return m_value;
}


const ErrorCode InputPin::await(const k_timeout_t  timeout) {
	InputPin_zbus_message msg;
	LOG_DBG("Awaiting input pin...");
	int ret;
	ret = zbus_chan_read(m_zbus_channel, &msg, timeout);
	LOG_DBG("Input pin received...");
    if (ret == EAGAIN) {
        return ErrorCode(m_instance, ErrorCode::Code::timeout, 1);
	}
	else if (ret != 0) {
		return ErrorCode(m_instance, ErrorCode::Code::runtime, 1);
	}
    
	return ErrorCode(m_instance, ErrorCode::Code::success);
}