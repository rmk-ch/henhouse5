/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2020 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file Sample app to demonstrate PWM-based LED fade
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>

#define NUM_STEPS	5U


int main(void)
{
	uint32_t pulse_width_ns;
	uint32_t step_ns;
	uint8_t dir;
	uint32_t period_ns;
	int ret;
	const struct pwm_dt_spec pwm_led = PWM_DT_SPEC_GET(DT_NODELABEL(green_pwm_led));
	uint32_t *puint32;
	const struct gpio_dt_spec brake_pin = GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_brake), gpios);
	const struct gpio_dt_spec dir_pin = GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_dir), gpios);


	printk("PWM-based LED fade for %s, channel=%u\n", pwm_led.dev->name, pwm_led.channel);
	
	
	if (!gpio_is_ready_dt(&brake_pin)) {
		printk("Brake pin not ready\n");
		return 0;
	}
	if (!gpio_is_ready_dt(&dir_pin)) {
		printk("Dir pin not ready\n");
		return 0;
	}
	
	ret = gpio_pin_configure_dt(&brake_pin, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Brake pin not configured\n");
		return 0;
	}
	ret = gpio_pin_configure_dt(&dir_pin, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		printk("Dir pin not configured\n");
		return 0;
	}
	
	ret = gpio_pin_set_dt(&brake_pin, 1);
	if (ret < 0) {
		printk("Brake pin not set\n");
		return ret;
	}
	ret = gpio_pin_set_dt(&dir_pin, 0);
	if (ret < 0) {
		printk("Dir pin not set\n");
		return ret;
	}

	
	period_ns = PWM_USEC(40); //pwm_led.period;
	pulse_width_ns = period_ns/4;
	step_ns = period_ns / NUM_STEPS;
	dir = 1U;
	if (!pwm_is_ready_dt(&pwm_led)) {
		printk("Error: PWM device %s is not ready\n", pwm_led.dev->name);
		return 0;
	}
	printk("PWM: period_ns=%uns=%ums, step_ns=%uns=%ums, \n", period_ns, period_ns/1000/1000, step_ns, step_ns/1000/1000);
	
	// MOE must be enabled!
	puint32 = (uint32_t*) 0x40012c44;
	*puint32 |= 1<<15;

	
	
	while (1) {
		k_sleep(K_MSEC(500));
		
		ret = pwm_set_dt(&pwm_led, period_ns, pulse_width_ns);
		if (ret) {
			printk("Error %d: failed to set pulse width for LED\n", ret);
		}
		ret = gpio_pin_toggle_dt(&brake_pin);
		if (ret < 0) {
			printk("Brake pin not toggled\n");
			return ret;
		}
		ret = gpio_pin_toggle_dt(&dir_pin);
		if (ret < 0) {
			printk("Dir pin not toggled\n");
			return ret;
		}

		printk("LED: Using pulse width %d%% (%uns=%ums)\n",
				100 * pulse_width_ns / period_ns, pulse_width_ns, pulse_width_ns/1000/1000);

		if (dir == 1) {
			if (pulse_width_ns + step_ns >= period_ns) {
				pulse_width_ns = period_ns;
				dir = 0U;
			} else {
				pulse_width_ns += step_ns;
			}
		} else {
			if (pulse_width_ns <= step_ns) {
				pulse_width_ns = 0;
				dir = 1U;
			} else {
				pulse_width_ns -= step_ns;
			}
		}

	}
	return 0;
}
