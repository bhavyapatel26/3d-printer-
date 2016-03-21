#include "GPSCommThread.h"

#include <stdio.h>

GPSCommThread::GPSCommThread()
{
    this->isRunning = false;
}

void GPSCommThread::setSerialFD(int serialFD)
{
    this->fd = serialFD;
}

void GPSCommThread::start()
{
    this->isRunning = true;

    run();
}

bool GPSCommThread::dataReady(t_fix* lat, t_fix* lon)
{
    if(isDataReady)
    {
        mtx.lock();

        *lat = this->lat;
        *lon = this->lon;

        isDataReady = false;

        mtx.unlock();

        return true;
    }

    return false;
}

void GPSCommThread::run()
{
    char buf[256];

    while(isRunning)
    {
        int bytesRead;
        if((bytesRead = read(fd, buf, sizeof(buf))) > 0)
        {
            parse(buf, bytesRead);
            isDataReady = true;
        }
    }
}

void GPSCommThread::parse(char* buf, int len)
{
    char* token = strtok(buf, ",");
    int state = 0;

    //Meant to parse GGA structured messages
    while(token != NULL)
    {

        switch(state)
        {
            case 0: //$GGA
            break;

            case 1: //Time
            timeParse(token, &curTime);
            break;

            case 2:
            break;
        }

        token = strtok(NULL, ",");
        state++;
    }
}

void GPSCommThread::timeParse(char* buf, t_time* time)
{
    char hours[3], minutes[3], seconds[7];

    memset(hours, '\0', 3);
    memset(minutes, '\0', 3);
    memset(seconds, '\0', 7);

    hours[0] = buf[0];
    hours[1] = buf[1];

    minutes[0] = buf[2];
    minutes[1] = buf[3];

    seconds[0] = buf[4];
    seconds[1] = buf[5];
    seconds[2] = buf[6];
    seconds[3] = buf[7];
    seconds[4] = buf[8];
    seconds[5] = buf[9];

    time->hours = atoi(hours);
    time->minutes = atoi(minutes);
    time->seconds = atof(seconds);
}

void GPSCommThread::latParse(char* buf, t_fix* lat)
{
    char degrees[3], minutes[11];

    memset(degrees, '\0', 3);
    memset(minutes, '\0', 11);

    degrees[0] = buf[0];
    degrees[1] = buf[1];

    minutes[0] = buf[2];
    minutes[1] = buf[3];
    minutes[2] = buf[4];
    minutes[3] = buf[5];
    minutes[4] = buf[6];
    minutes[5] = buf[7];
    minutes[6] = buf[8];
    minutes[7] = buf[9];
    minutes[8] = buf[10];
    minutes[9] = buf[11];

    lat->degrees = atoi(degrees);
    lat->minutes = atof(minutes);
}

void GPSCommThread::lonParse(char* buf, t_fix* lon)
{
    char degrees[4], minutes[11];

    memset(degrees, '\0', 4);
    memset(minutes, '\0', 11);

    degrees[0] = buf[0];
    degrees[1] = buf[1];
    degrees[2] = buf[2];

    minutes[0] = buf[3];
    minutes[1] = buf[4];
    minutes[2] = buf[5];
    minutes[3] = buf[6];
    minutes[4] = buf[7];
    minutes[5] = buf[8];
    minutes[6] = buf[9];
    minutes[7] = buf[10];
    minutes[8] = buf[11];
    minutes[9] = buf[12];

    lon->degrees = atoi(degrees);
    lon->minutes = atof(minutes);
}
