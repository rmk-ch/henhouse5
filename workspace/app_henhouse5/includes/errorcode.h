
#ifndef __ERRORCODE_H
#define __ERRORCODE_H

#include <stdint.h>

class ErrorCode {
    public:
        enum Instance : uint8_t {
            INVALID = 0,
            main = 1,
            door = 2,
            motor = 3,
            motor_brake_pin = 4,
            motor_dir_pin = 5,
            motor_pwm = 6,
        };
        enum Code : uint8_t {
            success = 0,
            undefined = 1,
            timeout = 2,
            not_ready = 3,
            invalid_config = 4,
            invalid_argument = 5,
            runtime = 6,
        };


        ErrorCode(Instance instance = Instance::INVALID, Code code = Code::undefined, uint8_t number = 0);
        virtual ~ErrorCode() {};

        bool hasFailed();

        static void registerCallback(void*());

    protected:
        static void* callback;

        Instance m_instance;
        Code m_code;
        uint8_t m_number;
        uint32_t m_serialized;

};

#endif