// #include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include "errorcode.h"
#include "inputpin.h"
#include "outputpin.h"
#include "motor.h"
#include "door.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);


K_THREAD_STACK_DEFINE(thread_door_stack, 4096);

const uint32_t ZBUS_CHANNEL_BUTTON_OPEN = ErrorCode::Instance::button_open;
ZBUS_CHAN_DEFINE_WITH_ID(
    zbus_channel_inputpin,    /* Name */
    ZBUS_CHANNEL_BUTTON_OPEN,              /* Unique channel identifier */
    InputPin_zbus_message,      /* Message type */
    NULL,       /* Validator */
    NULL,          /* User data */
    ZBUS_OBSERVERS_EMPTY,    /* observers */
    ZBUS_MSG_INIT(0) /* Initial value */
);

const uint32_t ZBUS_CHANNEL_ENDSWITCH_BOTTOM = ErrorCode::Instance::endswitch_bottom;
ZBUS_CHAN_DEFINE_WITH_ID(
    zbus_channel_endswitch_bottom,    /* Name */
    ZBUS_CHANNEL_ENDSWITCH_BOTTOM,              /* Unique channel identifier */
    InputPin_zbus_message,      /* Message type */
    NULL,       /* Validator */
    NULL,          /* User data */
    ZBUS_OBSERVERS_EMPTY,    /* observers */
    ZBUS_MSG_INIT(0) /* Initial value */
);
const uint32_t ZBUS_CHANNEL_ENDSWITCH_TOP = ErrorCode::Instance::endswitch_top;
ZBUS_CHAN_DEFINE_WITH_ID(
    zbus_channel_endswitch_top,    /* Name */
    ZBUS_CHANNEL_ENDSWITCH_TOP,              /* Unique channel identifier */
    InputPin_zbus_message,      /* Message type */
    NULL,       /* Validator */
    NULL,          /* User data */
    ZBUS_OBSERVERS_EMPTY,    /* observers */
    ZBUS_MSG_INIT(0) /* Initial value */
);

int main(void)
{
    ErrorCode ec;
    LOG_INF("Hello to Henhouse5!");
    
    InputPin button_open(ErrorCode::Instance::button_open, GPIO_DT_SPEC_GET(DT_ALIAS(button_open), gpios), ZBUS_CHANNEL_BUTTON_OPEN);
    button_open.init();
    InputPin endswitch_bottom(ErrorCode::Instance::endswitch_bottom, GPIO_DT_SPEC_GET(DT_NODELABEL(endswitchbottom), gpios), ZBUS_CHANNEL_ENDSWITCH_BOTTOM);
    endswitch_bottom.init();
    InputPin endswitch_top(ErrorCode::Instance::endswitch_top, GPIO_DT_SPEC_GET(DT_NODELABEL(endswitchtop), gpios), ZBUS_CHANNEL_ENDSWITCH_TOP);
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
        door.openClose(true);
        k_sleep(K_SECONDS(3));
        door.openClose(false);
        k_sleep(K_SECONDS(3));
    }

	return 0;
}
