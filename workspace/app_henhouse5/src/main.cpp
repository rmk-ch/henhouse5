#include <zephyr/logging/log.h>
#include "errorcode.h"
#include "inputpin.h"
#include "outputpin.h"
#include "motor.h"
#include "doorstate.h"
#include "doorcontrol.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);


K_THREAD_STACK_DEFINE(stack_door_state, 4096);
const int32_t prio_door_state = K_PRIO_COOP(1);

int main(void)
{
    ErrorCode ec;
    LOG_INF("Hello to Henhouse5!");
    
    InputPin button_open(ErrorCode::Instance::button_open, GPIO_DT_SPEC_GET(DT_ALIAS(button_open), gpios));
    InputPin endswitch_bottom(ErrorCode::Instance::endswitch_bottom, GPIO_DT_SPEC_GET(DT_NODELABEL(endswitchbottom), gpios), GPIO_INT_EDGE_BOTH);
    InputPin endswitch_top(ErrorCode::Instance::endswitch_top, GPIO_DT_SPEC_GET(DT_NODELABEL(endswitchtop), gpios), GPIO_INT_EDGE_BOTH);
    Pwm pwm(ErrorCode::Instance::motor_pwm, PWM_DT_SPEC_GET(DT_NODELABEL(green_pwm_led)));
    OutputPin brake_pin(ErrorCode::Instance::motor_brake_pin, GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_brake), gpios));
    OutputPin dir_pin(ErrorCode::Instance::motor_dir_pin, GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_dir), gpios));
    DoorState door_state(ErrorCode::Instance::door_state, endswitch_bottom, endswitch_top, stack_door_state, K_THREAD_STACK_SIZEOF(stack_door_state), prio_door_state);
    Motor motor = Motor(ErrorCode::Instance::motor, pwm, brake_pin, dir_pin);
    DoorControl door_control(ErrorCode::Instance::doorcontrol, motor, door_state);

    endswitch_bottom.registerCallback(static_cast<static_fcn_uint32*>(&static_callback_endswitches), static_cast<void*>(&door_state));
    endswitch_top.registerCallback(static_cast<static_fcn_uint32*>(&static_callback_endswitches), static_cast<void*>(&door_state));

    endswitch_bottom.init();
    endswitch_top.init();
    button_open.init();

    door_state.init();

    // actors
    motor.init();

    door_control.init();

    motor.testMotor();

    k_sleep(K_SECONDS(5));
    while (true) {
        door_control.openClose(true);
        k_sleep(K_SECONDS(3));
        door_control.openClose(false);
        k_sleep(K_SECONDS(3));
    }

	return 0;
}
