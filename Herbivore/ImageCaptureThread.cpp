#include "ImageCaptureThread.h"

#include <iostream>

#include <wiringPi.h>

ImageCaptureThread::ImageCaptureThread()
{
    webcam = cv::VideoCapture(0);

    pinMode(ZERO_IMAGE_SIGNAL_PIN, OUTPUT);
    digitalWrite(ZERO_IMAGE_SIGNAL_PIN, HIGH);

    if(!webcam.isOpened())
    {
        std::cout << "Error: Camera couldn't be opened" << std::endl;
    }
}

void ImageCaptureThread::setServerComm(ServerCommThread* serverCommThread)
{
    this->serverCommThread = serverCommThread;
}

void ImageCaptureThread::start()
{
    run();
}

void ImageCaptureThread::run()
{
    mtx.lock();

    digitalWrite(ZERO_IMAGE_SIGNAL_PIN, LOW);
    webcam.read(recentImage);
    digitalWrite(ZERO_IMAGE_SIGNAL_PIN, HIGH);

    mtx.unlock();

    serverCommThread->sendImage(recentImage);
}

void ImageCaptureThread::getRecentImage(cv::Mat* image)
{
    mtx.lock();
    recentImage.copyTo(*image);
    mtx.unlock();
}
