
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/gpio.h>


class Motor {
    public:
        Motor();
    	virtual ~Motor() {};
        virtual int init();

        virtual int set_speed_percent(float speed);

    protected:
        const struct pwm_dt_spec m_pwm;
        const struct gpio_dt_spec m_brake_pin;
        const struct gpio_dt_spec m_dir_pin;

        uint32_t m_brake_state;
        uint32_t m_dir_state;

        uint32_t m_period = PWM_USEC(40);
};
