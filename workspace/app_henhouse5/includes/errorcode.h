#pragma once

#include <stdint.h>

class ErrorCode {
    public:
        enum Instance : uint8_t {
            INVALID = 0,
            main = 1,
            doorcontrol = 2,
            motor = 3,
            motor_brake_pin = 4,
            motor_dir_pin = 5,
            motor_pwm = 6,
            button_open = 7,
            button_close = 8,
            endswitch_top = 9,
            endswitch_bottom = 10,
            motor_poweron_pin = 11,
            door_state = 12,
            led_green = 13,
            led_blue = 14,
            led_red = 15,
            status_leds = 16,
            door_trigger_auto = 17,
            rtc = 18,
            doorcontrolthread = 18,
        };
        enum Code : uint8_t {
            success = 0,
            undefined = 1,
            timeout = 2,
            not_ready = 3,
            invalid_config = 4,
            invalid_argument = 5,
            runtime = 6,
            not_implemented = 7,
            container_full = 8,
            unexpected_exit = 9,
            door_start_closing_wrong = 64,
            door_start_opening_wrong = 65,
            door_end_closing_wrong = 66,
            door_end_opening_wrong = 67,
        };


        ErrorCode(Instance instance = Instance::INVALID, Code code = Code::undefined, uint8_t number = 0);
        virtual ~ErrorCode() {};

        bool hasFailed();
        const uint32_t serialize();

        static void registerCallback(void*());

        Instance m_instance;
        Code m_code;
        uint8_t m_number;

    protected:
        static void* callback;


};
