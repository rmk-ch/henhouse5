#include <zephyr/logging/log.h>
#include "errorcode.h"
#include "inputpin.h"
#include "outputpin.h"
#include "statusleds.h"
#include "motor.h"
#include "doorstate.h"
#include "doorcontrol.h"
#include "rtc.hpp"
#include "doorcontrolthread.h"
#include "doortriggerauto.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);



///////////////////////////////////////////////////////////////////////////////
// Threads
///////////////////////////////////////////////////////////////////////////////
K_THREAD_STACK_DEFINE(stack_doorState, 4096);
const int32_t prio_doorState = K_PRIO_COOP(1); // higher number = lower prio

K_THREAD_STACK_DEFINE(stack_statusLeds, 1024);
const int32_t prio_statusLeds = K_PRIO_COOP(10); // higher number = lower prio

K_THREAD_STACK_DEFINE(stack_doorTriggerAuto, 1024);
const int32_t prio_doorTriggerAuto = K_PRIO_COOP(2); // higher number = lower prio

K_THREAD_STACK_DEFINE(stack_doorControlThread, 1024);
const int32_t prio_doorControlThread = K_PRIO_COOP(3); // higher number = lower prio

int main(void)
{
    ErrorCode ec;
    k_sleep(K_SECONDS(1)); // awaiting shell thread to start such that logs are shown
    LOG_INF("Hello to Henhouse5!");
    
    OutputPin ledGreen(ErrorCode::Instance::ledGreen, GPIO_DT_SPEC_GET(DT_NODELABEL(green_led_2), gpios));
    ledGreen.init(false, false);
    //OutputPin led_blue(ErrorCode::Instance::led_blue, GPIO_DT_SPEC_GET(DT_NODELABEL(blue_led_1), gpios)); // blue is communication
    // led_blue.init(false, true);
    OutputPin ledRed(ErrorCode::Instance::ledRed, GPIO_DT_SPEC_GET(DT_NODELABEL(red_led_3), gpios));
    ledRed.init(false, true);
    StatusLeds statusLeds(ErrorCode::Instance::statusLeds, ledGreen, ledRed, stack_statusLeds, K_THREAD_STACK_SIZEOF(stack_statusLeds), prio_statusLeds);
    statusLeds.start();

    // inputs
    Rtc rtc(ErrorCode::Instance::rtc);
    DoorTriggerAuto doorTriggerAuto(ErrorCode::Instance::doorTriggerAuto, rtc, stack_doorTriggerAuto, K_THREAD_STACK_SIZEOF(stack_doorTriggerAuto), prio_doorTriggerAuto);
    rtc.registerCallback([&doorTriggerAuto](uint16_t alarmId) -> void {return doorTriggerAuto.alarmCallback(alarmId);});
    rtc.init();
	rtc.set_date_time(50,29,8,4,5,2020);
    
    InputPin buttonOpen(ErrorCode::Instance::buttonOpen, GPIO_DT_SPEC_GET(DT_ALIAS(button_open), gpios), GPIO_INT_EDGE_FALLING);
    InputPin buttonClose(ErrorCode::Instance::buttonClose, GPIO_DT_SPEC_GET(DT_ALIAS(button_close), gpios), GPIO_INT_EDGE_FALLING);
    InputPin endswitchBottom(ErrorCode::Instance::endswitchBottom, GPIO_DT_SPEC_GET(DT_NODELABEL(endswitchbottom), gpios), GPIO_INT_EDGE_BOTH);
    InputPin endswitchTop(ErrorCode::Instance::endswitchTop, GPIO_DT_SPEC_GET(DT_NODELABEL(endswitchtop), gpios), GPIO_INT_EDGE_BOTH);
    Pwm pwm(ErrorCode::Instance::motorPwm, PWM_DT_SPEC_GET(DT_NODELABEL(green_pwm_led)));
    OutputPin brakePin(ErrorCode::Instance::motorBrakePin, GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_brake), gpios));
    OutputPin dirPin(ErrorCode::Instance::motorDirPin, GPIO_DT_SPEC_GET(DT_ALIAS(motor_a_dir), gpios));
    DoorState doorState(ErrorCode::Instance::doorState, endswitchBottom, endswitchTop, stack_doorState, K_THREAD_STACK_SIZEOF(stack_doorState), prio_doorState);
    Motor motor = Motor(ErrorCode::Instance::motor, pwm, brakePin, dirPin);
    DoorControl doorControl(ErrorCode::Instance::doorControl, motor, doorState);
    DoorControlThread doorControlThread(ErrorCode::Instance::doorControlThread, doorControl, stack_doorControlThread, K_THREAD_STACK_SIZEOF(stack_doorControlThread), prio_doorControlThread);

    endswitchBottom.registerCallback([&doorState](uint32_t pinInstanceId) -> void {return doorState.endswitchChanged(pinInstanceId);});
    endswitchTop.registerCallback([&doorState](uint32_t pinInstanceId) -> void {return doorState.endswitchChanged(pinInstanceId);});
    buttonOpen.registerCallback([&doorControlThread](uint32_t pinInstanceId) -> void {doorControlThread.openClose(true, DoorCommanderEnum::MANUAL);});
    buttonClose.registerCallback([&doorControlThread](uint32_t pinInstanceId) -> void {doorControlThread.openClose(false, DoorCommanderEnum::MANUAL);});
    doorState.registerCallback([&doorControl](DoorStateEnum state) -> void {return doorControl.doorstateChanged(state);});
    
    endswitchBottom.init();
    endswitchTop.init();
    buttonOpen.init();
    buttonClose.init();
    
    doorControl.init();
    doorState.start();
    doorControlThread.start();
    
    doorTriggerAuto.registerCallback([&doorControlThread](bool doOpen) -> void {doorControlThread.openClose(doOpen, DoorCommanderEnum::AUTO);});
    doorTriggerAuto.start();

    // actors
    motor.init();

    while (true) {
        k_sleep(K_HOURS(1));
    }

	return 0;
}
    