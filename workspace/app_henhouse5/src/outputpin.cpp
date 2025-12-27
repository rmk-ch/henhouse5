#include "outputpin.h"

OutputPin::OutputPin(ErrorCode::Instance instance, const struct gpio_dt_spec pin) :
    m_instance(instance),
    m_pin(pin),
    m_value(0)
    {
}

const ErrorCode OutputPin::init(const bool is_active, const bool initial_value) {
    int retint;
    ErrorCode retEC;
	if (!gpio_is_ready_dt(&m_pin)) {
		printk("Pin not ready\n");
		return ErrorCode(m_instance, ErrorCode::not_ready, 1);
	}


	retint = gpio_pin_configure_dt(&m_pin, is_active ? GPIO_OUTPUT_ACTIVE : GPIO_OUTPUT_INACTIVE);
	if (retint < 0) {
        printk("Pin not configured\n");
        return ErrorCode(m_instance, ErrorCode::invalid_config, 1);
	}

    retEC = set(initial_value);
    if (retEC.hasFailed()) {
        return retEC;
    }

    return ErrorCode(m_instance, ErrorCode::success);
}

const ErrorCode OutputPin::set(const bool value) {
    int ret;
	ret = gpio_pin_set_dt(&m_pin, value ? 1 : 0);
	if (ret < 0) {
        printk("Pin not set\n");
        return ErrorCode(m_instance, ErrorCode::runtime, 1);
	}
    m_value = value;
    return ErrorCode(m_instance, ErrorCode::success);
}

const bool OutputPin::get() {
    return m_value;
}