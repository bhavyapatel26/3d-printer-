#ifndef _GPS_COMM_THREAD_H_
#define _GPS_COMM_THREAD_H_

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

typedef struct t_time
{
    int hours, minutes;
    float seconds;
};

typedef struct t_fix
{
    int degrees;
    double minutes;
};

enum
{
    GGA = 0,
    GLL,
    GSA,
    GSV,
    RMC,
    VTG,
    ZDA,
    STI005,
    STI030
};

class GPSCommThread
{

    public:
        GPSCommThread();

        void start();
        bool dataReady(t_fix* lat, t_fix* lon);
        void setSerialFD(int serialFD);

    private:
        void run();
        void parse(char* buf, int len);
        void timeParse(char* buf, t_time* time);
        void lonParse(char* buf, t_fix* lon);
        void latParse(char* buf, t_fix* lat);

        int fd;

        t_fix lat, lon;
        t_time curTime;

        bool isDataReady, isRunning;

        boost::mutex mtx;
};

#endif
