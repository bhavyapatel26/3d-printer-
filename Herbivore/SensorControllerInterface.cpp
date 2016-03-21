#include "SensorControllerInterface.h"

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringPiI2C.h>

float getDistance(int index)
{
    unsigned char message[] = {(unsigned char)index, 0, 0};

    wiringPiSPIDataRW(SENSOR_CON_CS, message, 3);

    int time = message[2];
    time = (time << 8) | message[1];

    return (((float)time)/1000000.0)*SPEED_OF_SOUND;
}
