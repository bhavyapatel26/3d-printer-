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

bool GPSCommThread::dataReady(pair<int, int> coor)
{
    if(isDataReady)
    {
        mtx.lock();

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
            isDataReady = true;
        }
    }
}
