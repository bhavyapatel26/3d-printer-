#ifndef _PLANNER_THREAD_H_
#define _PLANNER_THREAD_H_

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <queue>
#include <utility>
#include <cstring>

#include "GPSCommThread.h"
#include "ImageCaptureThread.h"
#include "ServerCommThread.h"

enum
{
    STOPPED = 0
};

class PlannerThread
{

    public:
        PlannerThread(int serialFD, ServerCommThread* serverCommThread);

        void start();
        int getState();

        void postMessage(std::string message);

    private:
        void run();

        bool isRunning;
        boost::mutex mtx;
        int state;

        std::queue<std::pair<t_fix, t_fix> > returnQ;
        std::queue<std::string> postbox;

        GPSCommThread gpsCommThread;
        ImageCaptureThread imageCaptureThread;
        ServerCommThread* serverCommThread;
};

#endif
