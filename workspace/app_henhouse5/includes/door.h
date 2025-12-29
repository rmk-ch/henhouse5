#ifndef __DOOR_H
#define __DOOR_H

#include "errorcode.h"
#include "motor.h"
#include "inputpin.h"

class Door {
    public:
        Door(ErrorCode::Instance instance, Motor& motor, InputPin& endswitch_top, InputPin& endswitch_bottom);
        virtual ~Door() {};
        const ErrorCode init();
        const ErrorCode openClose(bool do_open);

    protected:
        const ErrorCode::Instance m_instance;
        Motor& m_motor;
        InputPin& m_endswitch_top;
        InputPin& m_endswitch_bottom;
};

#endif