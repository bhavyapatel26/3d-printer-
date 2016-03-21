#ifndef _IMAGE_CAP_THREAD_H_
#define _IMAGE_CAP_THREAD_H_

#include <opencv2/highgui/highgui.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

#include "ServerCommThread.h"

#define ZERO_IMAGE_SIGNAL_PIN      2

typedef struct ImageData
{
    cv::Mat image;
    float lat, lon;
};

class ImageCaptureThread
{

    public:
        ImageCaptureThread();

        void setServerComm(ServerCommThread* serverCommThread);

        void start();

        void getRecentImage(cv::Mat* image);

    private:
        void run();

        cv::VideoCapture webcam;
        cv::Mat recentImage;
        boost::mutex mtx;

        ServerCommThread *serverCommThread;
};

#endif
