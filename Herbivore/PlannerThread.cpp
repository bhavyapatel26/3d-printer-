#include "PlannerThread.h"

#include <boost/bind.hpp>

#include <cmath>

PlannerThread::PlannerThread(int serialFD, ServerCommThread* serverCommThread)
{
    isRunning = false;

    gpsCommThread.setSerialFD(serialFD);

    this->serverCommThread = serverCommThread;
    imageCaptureThread.setServerComm(serverCommThread);

    pidDriveLeft = new PID(&currentHeading, &leftDrivePower, &desiredHeading, 1, 1, 1, DIRECT);
    pidDriveRight = new PID(&currentHeading, &rightDrivePower, &desiredHeading, 1, 1, 1, DIRECT);

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

    //boost::thread t(boost::bind(&GPSCommThread::start, &gpsCommThread));
    run();
}

int PlannerThread::getState()
{
    return state;
}

void PlannerThread::run()
{
    pair<int, int> goTo, oldGoTo;

    MOVE_TYPE moving = STRAIGHT;

    while(isRunning)
    {
        if(gpsCommThread.dataReady(&lat, &lon))
        {
            boost::thread t(boost::bind(&ImageCaptureThread::start, &imageCaptureThread));

            t.join();
        }

        getDistance(NUM_SENSORS);
        for(int i = 0; i < NUM_SENSORS; i++)
        {
            distances[i] = getDistance(i);
        }

        oldGoTo = goTo;
        goTo = nextPoint(botPos);

        currentHeading += getRot(0, X, GFS_1000DPS)*(0.001);
        desiredHeading = atan2(goTo.second - botPos.second, goTo.first - botPos.first);

        if(moving == STRAIGHT)
        {
            if(oldGoTo != goTo) //Make turn
            {
                moving = TURNING;

                motorPowerScalar = 0.5;

                if(currentHeading > desiredHeading)
                {
                    leftDrivePower = 1;
                    rightDrivePower = -1;
                }
                else
                {
                    leftDrivePower = -1;
                    rightDrivePower = 1;
                }
            }
            else
            {
                pidDriveLeft->Compute();
                pidDriveRight->Compute();

                double diff = dist(botPos, goTo)/1000; //Convert to meters

                if(diff > 1)
                    motorPowerScalar = 1;
                else if(diff > 0.5)
                    motorPowerScalar = 0.75;
                else
                    motorPowerScalar = 0.5;
            }
        }
        else if(moving == TURNING)
        {
            double diff = abs(currentHeading - desiredHeading);

            if(diff > 90)
                motorPowerScalar = 0.5;
            else if(diff > 45)
                motorPowerScalar = 0.25;

            if(diff < 5)
                moving = STRAIGHT;
        }

        leftDrivePower *= motorPowerScalar;
        rightDrivePower *= motorPowerScalar;

        setMotorPower(LEFT_DRIVE, (int)leftDrivePower);
        setMotorPower(RIGHT_DRIVE, (int)rightDrivePower);

        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }
}

float PlannerThread::dist(pair<int, int> c1, pair<int, int> c2)
{
	return sqrt(pow(c1.first - c2.first, 2) + pow(c1.second - c2.second, 2));
}

pair<int, int> PlannerThread::nextPoint(pair<int, int> curPoint)
{
	if(!endPoints.empty())
	{
		if(dist(endPoints.top(), curPoint) == 0)
		{
			endPoints.pop();

			if(endPoints.empty())
			{
				//Calculate new endpoint
				int columnStep = BOT_WIDTH;

				if(curPoint.first + columnStep > maxX)
					columnStep = maxX - curPoint.first;

				switch(botMovingTo)
				{
					case COL_BTM:
					if(curPoint.first != maxX)
					{
						botMovingTo = NEXT_COL;
						botAt = COL_BTM;

						endPoints.push(pair<int, int>(curPoint.first + columnStep, curPoint.second));
					}
					else
					{
						botMovingTo = UNKNOWN;
						botAt = MAX_POS;

						endPoints.push(curPoint);
					}
					break;

					case COL_BTM_NO_STEP:
					botMovingTo = COL_TOP;
					botAt = COL_BTM;

					endPoints.push(pair<int, int>(curPoint.first, maxY));
					break;

					case COL_TOP:
					if(curPoint.first != maxX)
					{
						botMovingTo = NEXT_COL;
						botAt = COL_TOP;

						endPoints.push(pair<int, int>(curPoint.first + columnStep, curPoint.second));
					}
					else
					{
						botMovingTo = UNKNOWN;
						botAt = MAX_POS;

						endPoints.push(curPoint);
					}
					break;

					case NEXT_COL:
					switch(botAt)
					{
						case COL_BTM:
						botMovingTo = COL_TOP;

						endPoints.push(pair<int, int>(curPoint.first, maxY));
						break;

						case COL_TOP:
						botMovingTo = COL_BTM;

						endPoints.push(pair<int, int>(curPoint.first, 0));
						break;
					}
					botAt = UNKNOWN;

					break;
				}
			}
		}
	}
	else //Fresh start
        endPoints.push(pair<int, int>(0,0));

	return endPoints.top();
}
