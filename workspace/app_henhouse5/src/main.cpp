#include <zephyr/logging/log.h>
#include "errorcode.h"
#include "inputpin.h"
#include "outputpin.h"
#include "statusleds.h"
#include "motor.h"
#include "doorstate.h"
#include "doorcontrol.h"
#include "rtc.hpp"
#include "triggerdoor.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);


K_THREAD_STACK_DEFINE(stack_door_state, 4096);
const int32_t prio_door_state = K_PRIO_COOP(1); // higher number = lower prio

K_THREAD_STACK_DEFINE(stack_status_leds, 1024);
const int32_t prio_status_leds = K_PRIO_COOP(10); // higher number = lower prio

K_THREAD_STACK_DEFINE(stack_triggerdoor, 1024);
const int32_t prio_triggerdoor = K_PRIO_COOP(2); // higher number = lower prio

int main(void)
{
    ErrorCode ec;
    LOG_INF("Hello to Henhouse5!");
    
    OutputPin led_green(ErrorCode::Instance::led_green, GPIO_DT_SPEC_GET(DT_NODELABEL(green_led_2), gpios));
    //OutputPin led_blue(ErrorCode::Instance::led_blue, GPIO_DT_SPEC_GET(DT_NODELABEL(blue_led_1), gpios)); // blue is communication
    OutputPin led_red(ErrorCode::Instance::led_red, GPIO_DT_SPEC_GET(DT_NODELABEL(red_led_3), gpios));
    led_green.init(false, false);
    // led_blue.init(false, true);
    led_red.init(false, true);
    StatusLeds status_leds(ErrorCode::Instance::status_leds, led_green, led_red, stack_status_leds, K_THREAD_STACK_SIZEOF(stack_status_leds), prio_status_leds);
    status_leds.init();

    // inputs
    Rtc rtc(ErrorCode::Instance::rtc);
    TriggerDoor trigger_door(ErrorCode::Instance::trigger_door, rtc, stack_triggerdoor, K_THREAD_STACK_SIZEOF(stack_triggerdoor), prio_triggerdoor);
    rtc.registerCallback(static_callback_rtc_alarm, &trigger_door);
    rtc.init();
	rtc.set_date_time(3,2,1,4,5,2020);
    trigger_door.init();
    
    InputPin button_open(ErrorCode::Instance::button_open, GPIO_DT_SPEC_GET(DT_ALIAS(button_open), gpios));
    InputPin endswitch_bottom(ErrorCode::Instance::endswitch_bottom, GPIO_DT_SPEC_GET(DT_NODELABEL(endswitchbottom), gpios), GPIO_INT_EDGE_BOTH);
    InputPin endswitch_top(ErrorCode::Instance::endswitch_top, GPIO_DT_SPEC_GET(DT_NODELABEL(endswitchtop), gpios), GPIO_INT_EDGE_BOTH);
    Pwm pwm(ErrorCode::Instance::motor_pwm, PWM_DT_SPEC_GET(DT_NODELABEL(green_pwm_led)));
    OutputPin brake_pin(ErrorCode::Instance::motor_brake_pin, GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_brake), gpios));
    OutputPin dir_pin(ErrorCode::Instance::motor_dir_pin, GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_dir), gpios));
    DoorState door_state(ErrorCode::Instance::door_state, endswitch_bottom, endswitch_top, stack_door_state, K_THREAD_STACK_SIZEOF(stack_door_state), prio_door_state);
    Motor motor = Motor(ErrorCode::Instance::motor, pwm, brake_pin, dir_pin);
    DoorControl door_control(ErrorCode::Instance::doorcontrol, motor, door_state);

    endswitch_bottom.registerCallback(&static_callback_endswitches, &door_state);
    endswitch_top.registerCallback(&static_callback_endswitches, &door_state);
    door_state.registerCallback(&static_callback_doorstate, &door_control);

    endswitch_bottom.init();
    endswitch_top.init();
    button_open.init();

    door_control.init();
    door_state.init();

    // actors
    motor.init();

    // motor.testMotor();

    k_sleep(K_SECONDS(5));
    while (true) {
        door_control.openClose(true);
        k_sleep(K_SECONDS(10));
        door_control.openClose(false);
        k_sleep(K_SECONDS(10));
    }

    while (true) {
        k_sleep(K_HOURS(1));
    }

	return 0;
}
