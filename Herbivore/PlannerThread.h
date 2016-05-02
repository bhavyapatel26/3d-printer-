#ifndef _PLANNER_THREAD_H_
#define _PLANNER_THREAD_H_

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include <vector>
#include <queue>
#include <utility>
#include <cstring>
#include <stack>

#include "GPSCommThread.h"
#include "ImageCaptureThread.h"
#include "ServerCommThread.h"

#include "MotorControllerInterface.h"
#include "SensorControllerInterface.h"

#include "PID.h"

#define BOT_WIDTH   10

enum BOT_STATE
{
	UNKNOWN = -1,
	COL_BTM = 0,
	COL_BTM_NO_STEP,
	COL_TOP,
	NEXT_COL,
	MAX_POS
};

enum MOVE_TYPE
{
    STOPPED = 0,
    STRAIGHT,
    TURNING
};

using namespace std;

class PlannerThread
{

    public:
        PlannerThread(int serialFD, ServerCommThread* serverCommThread);

        void start();
        int getState();

        void postMessage(std::string message);

    private:
        void run();
        float dist(pair<int, int> c1, pair<int, int> c2);

        pair<int, int> nextPoint(pair<int, int> curPos);

        int maxX, maxY;
        BOT_STATE botMovingTo, botAt;
        MOVE_TYPE botMoveType;

        pair<int, int> botPos;

        bool isRunning;
        boost::mutex mtx;
        int state;

        std::queue<std::pair<int, int> > returnQ;
        std::queue<std::string> postbox;

        float distances[NUM_SENSORS];

        GPSCommThread gpsCommThread;
        ImageCaptureThread imageCaptureThread;
        ServerCommThread* serverCommThread;

        double currentHeading;
        double desiredHeading;

        double motorPowerScalar;
        double leftDrivePower;
        double rightDrivePower;

        PID* pidDriveLeft;
        PID* pidDriveRight;

        stack<pair<int, int> > endPoints;
};

#endif
