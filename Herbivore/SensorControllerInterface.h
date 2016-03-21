#ifndef _SENSOR_CON_INTERFACE_H_
#define _SENSOR_CON_INTERFACE_H_

#define SENSOR_CON_CS   1

#define SPEED_OF_SOUND  340

enum
{
    FRONT_LEFT_SEN = 0,
    FRONT_RIGHT_SEN,
    LEFT_LEFT_SEN,
    LEFT_RIGHT_SEN,
    RIGHT_LEFT_SEN,
    RIGHT_RIGHT_SEN
};

float getDistance(int index);


#endif
