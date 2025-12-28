#include <zephyr/sys/printk.h>
#include "errorcode.h"
#include "button.h"
#include "outputpin.h"
#include "motor.h"


int main(void)
{
    ErrorCode ec;
    const float min_speed = -1.0;
    const float max_speed = 1.0;
    const float zero_speed = 0.0;
    const float step = 0.1;
    printk("Hello to Henhouse5!\n");
    
    Button button_open = Button(ErrorCode::Instance::button_open, GPIO_DT_SPEC_GET(DT_ALIAS(button_open), gpios));
    button_open.init();
    Motor motor = Motor(ErrorCode::Instance::motor);
    motor.init();

    float speed = zero_speed;
    while (true)
    {
        for (; speed <= max_speed; speed += step) {
            motor.setSpeed(speed);
            k_sleep(K_MSEC(1000));
        }
        for (; speed >= min_speed; speed -= step) {
            motor.setSpeed(speed);
            k_sleep(K_MSEC(1000));
        }
    }    

	return 0;
}
