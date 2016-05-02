#ifndef _GPS_COMM_THREAD_H_
#define _GPS_COMM_THREAD_H_

#include <utility>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace std;

struct GPSInfo
{
	double m_latitude;
	double m_longitude;
    double m_altitude;
	unsigned long m_nSentences;
	unsigned int m_signalQuality;
	unsigned int m_satelitesInUse;
};

class GPSCommThread
{

    public:
        GPSCommThread();

        void start();
        bool dataReady(pair<int, int> coor);
        void setSerialFD(int serialFD);

    private:
        void run();

    	void ParseRecursive(const char ch);
        void ParseNMEASentence(const char *addressField, const char *buf, const unsigned int bufSize);
        void ProcessGPGGA(const char *buf, const unsigned int bufSize);
        void ProcessGPGSA(const char *buf, const unsigned int bufSize);
        void ProcessGPGSV(const char *buf, const unsigned int bufSize);
        void ProcessGPRMB(const char *buf, const unsigned int bufSize);
        void ProcessGPRMC(const char *buf, const unsigned int bufSize);
        void ProcessGPZDA(const char *buf, const unsigned int bufSize);

        int fd;

        bool isDataReady, isRunning;

        boost::mutex mtx;
};

#endif
