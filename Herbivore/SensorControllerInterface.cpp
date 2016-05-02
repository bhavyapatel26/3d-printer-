#include "SensorControllerInterface.h"

#include <cstdlib>
#include <stdint.h>

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

double getMRes(MScale scale)
{
	switch(scale)
	{
		case MFS_14BITS:
		return 10.0*4912.0/8190.0;
		break;

		case MFS_16BITS:
		return 10.0*4912.0/32760.0;
		break;
	}

	return 0;
}

double getGRes(GScale scale)
{
	switch(scale)
	{
		case GFS_250DPS:
		return 250.0/32768.0;
		break;

		case GFS_500DPS:
		return 500.0/32768.0;
		break;

		case GFS_1000DPS:
		return 1000.0/32768.0;
		break;

		case GFS_2000DPS:
		return 2000.0/32768.0;
		break;
	}

	return 0;
}

double getARes(AScale scale)
{
	switch(scale)
	{
		case AFS_2G:
		return 2.0/32768.0;
		break;

		case AFS_4G:
		return 4.0/32768.0;
		break;

		case AFS_8G:
		return 8.0/32768.0;
		break;

		case AFS_16G:
		return 16.0/32768.0;
		break;
	}

	return 0;
}

double getAccel(int fd, Axis axis, AScale scale)
{
    int reg = 0;
	switch(axis)
	{
		case X:
		reg = ACCEL_XOUT_H;
		break;

		case Y:
		reg = ACCEL_YOUT_H;
		break;

		case Z:
		reg = ACCEL_ZOUT_H;
		break;

		default:
		return -1;
		break;
	}

	return ((int16_t)wiringPiI2CReadReg16(fd, reg))*getARes(scale);
}

double getRot(int fd, Axis axis, GScale scale)
{
    int reg = 0;
	switch(axis)
	{
        case X:
        reg = GYRO_XOUT_H;
        break;

        case Y:
        reg = GYRO_YOUT_H;
        break;

        case Z:
        reg = GYRO_ZOUT_H;
        break;

        default:
        return -1;
        break;
	}

	return ((int16_t)wiringPiI2CReadReg16(fd, reg))*getGRes(scale);
}

double getMag(int fd, Axis axis, MScale scale)
{
    int reg = 0;
	switch(axis)
	{
        case X:
        reg = AK8963_XOUT_H;
        break;

        case Y:
        reg = AK8963_YOUT_H;
        break;

        case Z:
        reg = AK8963_ZOUT_H;
        break;

        default:
        return -1;
        break;
	}

	return ((int16_t)wiringPiI2CReadReg16(fd, reg))*getMRes(scale);
}
