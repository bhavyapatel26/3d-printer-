#ifndef _MOTOR_CON_INTERFACE_H_
#define _MOTOR_CON_INTERFACE_H_

#define MOTOR_CON_CS    0

enum
{
    LEFT_DRIVE = 0,
    RIGHT_DRIVE,
    DRILL_DRIVE
};

enum
{
    NO_COMMAND = 0,
    SET_POWER,
    SET_DIR,
    GET_POWER,
    GET_DIR
};

enum
{
    FORWARD = 0,
    REVERSE
};

void setMotorPower(int index, int power);
void setMotorDir(int index, int dir);

#endif
