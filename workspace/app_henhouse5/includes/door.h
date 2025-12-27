#ifndef __DOOR_H
#define __DOOR_H

#include "errorcode.h"
#include "motor.h"

class Door {
    public:
        Door(ErrorCode::Instance instance);
        virtual ~Door();
        virtual const ErrorCode openClose(bool do_open);

    protected:
        const ErrorCode::Instance m_instance;
        Motor m_motor;
        // m_upperEndSwitch;
        // m_lowerEndSwitch;

};

#endif