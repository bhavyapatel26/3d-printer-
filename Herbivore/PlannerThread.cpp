#include "PlannerThread.h"

#include <boost/bind.hpp>

#include "MotorControllerInterface.h"
#include "SensorControllerInterface.h"

PlannerThread::PlannerThread(int serialFD, ServerCommThread* serverCommThread)
{
    isRunning = false;

    gpsCommThread.setSerialFD(serialFD);

    this->serverCommThread = serverCommThread;
    imageCaptureThread.setServerComm(serverCommThread);

    state = STOPPED;
}

void PlannerThread::postMessage(std::string message)
{
    mtx.lock();

    postbox.push(message);

    mtx.unlock();
}

void PlannerThread::start()
{
    isRunning = true;

    boost::thread t(boost::bind(&GPSCommThread::start, &gpsCommThread));
    run();
}

int PlannerThread::getState()
{
    return state;
}

void PlannerThread::run()
{
    t_fix lat, lon;

    while(isRunning)
    {
        if(gpsCommThread.dataReady(&lat, &lon))
        {
            boost::thread t(boost::bind(&ImageCaptureThread::start, &imageCaptureThread));

            t.join();
        }
        //Get gyro data
        //Get distance data

        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
}
