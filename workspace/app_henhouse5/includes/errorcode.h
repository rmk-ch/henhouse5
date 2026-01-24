#pragma once

#include <stdint.h>

class ErrorCode {
    public:
        enum Instance : uint8_t {
            INVALID = 0,
            main = 1,
            doorControl = 2,
            motor = 3,
            motorBrakePin = 4,
            motorDirPin = 5,
            motorPwm = 6,
            buttonOpen = 7,
            buttonClose = 8,
            endswitchTop = 9,
            endswitchBottom = 10,
            motorPoweronPin = 11,
            doorState = 12,
            ledGreen = 13,
            ledBlue = 14,
            ledRed = 15,
            statusLeds = 16,
            doorTriggerAuto = 17,
            rtc = 18,
            doorControlThread = 18,
        };
        enum Code : uint8_t {
            success = 0,
            undefined = 1,
            timeout = 2,
            notReady = 3,
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
