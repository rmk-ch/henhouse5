#include <zephyr/sys/printk.h>
#include "errorcode.h"
#include "inputpin.h"
#include "outputpin.h"
#include "motor.h"
#include "door.h"


int main(void)
{
    ErrorCode ec;
    printk("Hello to Henhouse5!\n");
    
    InputPin button_open(ErrorCode::Instance::button_open, GPIO_DT_SPEC_GET(DT_ALIAS(button_open), gpios));
    button_open.init();
    InputPin endswitch_bottom(ErrorCode::Instance::endswitch_bottom, GPIO_DT_SPEC_GET(DT_NODELABEL(endswitchbottom), gpios));
    endswitch_bottom.init();
    InputPin endswitch_top(ErrorCode::Instance::endswitch_top, GPIO_DT_SPEC_GET(DT_NODELABEL(endswitchtop), gpios));
    endswitch_top.init();

    // actors
    Pwm pwm(ErrorCode::Instance::motor_pwm, PWM_DT_SPEC_GET(DT_NODELABEL(green_pwm_led)));
    OutputPin brake_pin(ErrorCode::Instance::motor_brake_pin, GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_brake), gpios));
    OutputPin dir_pin(ErrorCode::Instance::motor_dir_pin, GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_dir), gpios));

    Motor motor = Motor(ErrorCode::Instance::motor, pwm, brake_pin, dir_pin);
    motor.init();

    Door door(ErrorCode::Instance::door, motor, endswitch_top, endswitch_bottom);
    door.init();

    k_sleep(K_SECONDS(5));
    while (true) {
        printk("Opening door...");
        door.openClose(true);
        printk("Closing door...");
        door.openClose(false);
    }

    const float min_speed = -1.0;
    const float max_speed = 1.0;
    const float zero_speed = 0.0;
    const float step = 0.1;
    float speed = zero_speed;
    while (true)
    {
        for (; speed < max_speed; speed += step) {
            motor.setSpeed(speed);
            k_sleep(K_MSEC(1000));
        }
        for (; speed > min_speed; speed -= step) {
            motor.setSpeed(speed);
            k_sleep(K_MSEC(1000));
        }
    }

	return 0;
}
