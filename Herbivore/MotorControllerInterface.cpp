#include "MotorControllerInterface.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>

void setMotorPower(int index, int power)
{
    if(power < 0)
    {
        setMotorDir(index, REVERSE);
    }
    else
    {
        setMotorDir(index, FORWARD);
    }

    unsigned char message[] = {SET_POWER, (unsigned char)index, (unsigned char)(power/100.0)*255.0};

    wiringPiSPIDataRW(MOTOR_CON_CS, message, 3);
}

void setMotorDir(int index, int dir)
{
    unsigned char message[] = {SET_DIR, (unsigned char)index, (unsigned char)dir};

    wiringPiSPIDataRW(MOTOR_CON_CS, message, 3);
}
