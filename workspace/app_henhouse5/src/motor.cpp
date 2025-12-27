#include <cmath>

#include "motor.h"


Motor::Motor() :
    m_pwm(PWM_DT_SPEC_GET(DT_NODELABEL(green_pwm_led))),
    m_brake_pin(GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_brake), gpios)),
    m_dir_pin(GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_dir), gpios)),
    m_brake_state(1),
    m_dir_state(0)
{

}

int Motor::init() {
    uint32_t ret;
    // init gpios
	if (!gpio_is_ready_dt(&m_brake_pin)) {
		printk("Brake pin not ready\n");
		return 0;
	}
	if (!gpio_is_ready_dt(&m_dir_pin)) {
		printk("Dir pin not ready\n");
		return 0;
	}

	ret = gpio_pin_configure_dt(&m_brake_pin, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Brake pin not configured\n");
		return 0;
	}
	ret = gpio_pin_configure_dt(&m_dir_pin, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Dir pin not configured\n");
		return 0;
	}
	
	ret = gpio_pin_set_dt(&m_brake_pin, m_brake_state);
	if (ret < 0) {
		printk("Brake pin not set\n");
		return ret;
	}
	ret = gpio_pin_set_dt(&m_dir_pin, m_dir_state);
	if (ret < 0) {
		printk("Dir pin not set\n");
		return ret;
	}

    // PWM
	if (!pwm_is_ready_dt(&m_pwm)) {
		printk("Error: PWM device %s is not ready\n", m_pwm.dev->name);
		return -1;
	}
	
	// MOE must be enabled!
    uint32_t * puint32;
	puint32 = (uint32_t*) 0x40012c44;
	*puint32 |= 1<<15;

    set_speed_percent(0.0);

    return 0;
}

int Motor::set_speed_percent(float speed) {
    const float speed_tolerance = 0.0001;
    uint32_t ret;
    if (speed >= -speed_tolerance && speed <= speed_tolerance) {
        printk("Motor stopping\n");
        speed = 0;
        m_brake_state = 1;
    }
    else {
        m_brake_state = 0;
    }
    
    float abs_speed;
    if (speed < 0) {
        abs_speed = speed+1.0;
        m_dir_state = 1;
    }
    else {
        abs_speed = speed;
        m_dir_state = 0;
    }

    ret = gpio_pin_set_dt(&m_brake_pin, m_brake_state);
    if (ret < 0) {
        printk("Brake pin not toggled\n");
        return ret;
    }
    ret = gpio_pin_set_dt(&m_dir_pin, m_dir_state);
    if (ret < 0) {
        printk("Dir pin not toggled\n");
        return ret;
    }

    uint32_t pulse_width = static_cast<uint32_t>(std::lround(abs_speed * static_cast<float>(m_period)));

    ret = pwm_set_dt(&m_pwm, m_period, pulse_width);
    if (ret) {
        printk("Error %d: failed to set pulse width\n", ret);
    }

    printk("Motor: PWM %u/%u, brake = %u, dir = %u\n",
            pulse_width, m_period, m_brake_state, m_dir_state);


    return 0;
}