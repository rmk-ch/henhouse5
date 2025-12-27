#include <zephyr/sys/printk.h>
#include "errorcode.h"
#include "motor.h"


int main(void)
{
    ErrorCode ec;
    const float min_speed = -1.0;
    const float max_speed = 1.0;
    const float step = 0.1;
    printk("Hello to Henhouse5!\n");
    
    Motor motor = Motor(ErrorCode::Instance::motor);
    motor.init();


    while (true)
    {
        for (float speed = min_speed; speed <= max_speed; speed += step) {
            motor.setSpeedPercent(speed);
            k_sleep(K_MSEC(1000));
        }
        for (float speed = max_speed; speed >= min_speed; speed -= step) {
            motor.setSpeedPercent(speed);
            k_sleep(K_MSEC(1000));
        }
    }
    

	return 0;
}
