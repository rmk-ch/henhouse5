#include "button.h"


static void button_pressed(const struct device *dev, struct gpio_callback *cb,
		    uint32_t pins)
{
    // why so complicated to get button class? Ask Zephyr devs... Details in description of callback_container struct.
    const struct callback_container* cb_container = CONTAINER_OF(cb, struct callback_container, callback_data);
    const Button* button = cb_container->button;
	printk("This: %p Button %p / %u pressed at %" PRIu32 "\n", button, dev, pins, k_cycle_get_32());
};

Button::Button(ErrorCode::Instance instance, const struct gpio_dt_spec pin) :
    m_instance(instance),
    m_pin(pin),
    m_value(0)
    {
        m_cb_container.button = this;
}

const ErrorCode Button::init() {
    int ret;
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


	gpio_init_callback(&m_cb_container.callback_data, button_pressed, BIT(m_pin.pin));
	gpio_add_callback(m_pin.port, &m_cb_container.callback_data);
	printk("Set up m_pin at %s pin %d\n", m_pin.port->name, m_pin.pin);

    return ErrorCode(m_instance, ErrorCode::success);
}

const bool Button::get() {
    m_value = gpio_pin_get(m_pin.port, m_pin.pin);
    return m_value;
}