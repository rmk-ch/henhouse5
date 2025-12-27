#include <zephyr/sys/printk.h>
#include "motor.h"


int main(void)
{
    const float min_speed = -1.0;
    const float max_speed = 1.0;
    const float step = 0.1;
    printk("Hello to HenHouse5!\n");
    
    Motor motor = Motor();
    motor.init();


    while (true)
    {
        for (float speed = min_speed; speed <= max_speed; speed += step) {
            motor.set_speed_percent(speed);
            k_sleep(K_MSEC(1000));
        }
    }
    

	return 0;
}
