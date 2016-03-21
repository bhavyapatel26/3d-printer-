#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#include <wiringSerial.h>

#include "PlannerThread.h"
#include "ServerCommThread.h"

#include "MotorControllerInterface.h"

#define CODE_VERSION        0.1
#define HARDWARE_VERSION    0.1

#define GPS_BAUD            57600

using namespace std;

struct termios orig_termios;

void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode()
{
    struct termios new_termios;

    /* take two copies - one for now, one for later */
    tcgetattr(0, &orig_termios);
    memcpy(&new_termios, &orig_termios, sizeof(new_termios));

    /* register cleanup handler, and set the new terminal mode */
    atexit(reset_terminal_mode);
    cfmakeraw(&new_termios);
    tcsetattr(0, TCSANOW, &new_termios);
}

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int getch()
{
    int r;
    unsigned char c;
    if ((r = read(0, &c, sizeof(c))) < 0) {
        return r;
    } else {
        return c;
    }
}

int main()
{
    set_conio_terminal_mode();

    cout << "Senior Design 2016" << endl;
    cout << "Group P1: The Herbivore" << endl;
    cout << endl << "Hardware Version: " << HARDWARE_VERSION << endl;
    cout << endl << "Code Version: " << CODE_VERSION << endl;

    #ifdef DEBUG
    cout << "Running under DEBUG build" << endl;
    #endif

    cout << endl << "Setting up WiringPi" << endl;

    cout << "Running base setup...";
    try
    {
        wiringPiSetup();
    }
    catch(exception& e)
    {
        cout << "Failed" << endl;
        cout << e.what() << endl << endl;
        cout << "Ending" << endl;

        return -1;
    }
    cout << "Success" << endl << endl;

    cout << "Running SPI setup...";
    if(system("gpio load spi") < 0)
    {
        cout << "Failed" << endl;
        cout << "Ending" << endl;

        return -1;
    }
    cout << "Success" << endl << endl;


    cout << "Running I2C setup...";
    if(system("gpio load i2c") < 0)
    {
        cout << "Failed" << endl;
        cout << "Ending" << endl;

        return -1;
    }
    cout << "Success" << endl;
    system("i2cdetect -y 1");
    cout << endl << endl;

    cout << "Running serial setup...";
    int serialFD = serialOpen("/dev/ttyAMA", GPS_BAUD);
    if(serialFD < 0)
    {
        cout << "Failed" << endl;
        cout << "Ending" << endl;

        return -1;
    }
    cout << "Success" << endl << endl;

    cout <<"Starting server communication thread" << endl << endl;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::asio::ip::tcp::resolver::query query("127.0.0.1", "8080");
    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

    ServerCommThread serverCommThread(io_service, iterator);
    boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

    cout << "Starting planner thread" << endl << endl;
    PlannerThread planner(serialFD, &serverCommThread);

    bool kill = false;
    while(!kill)
    {
        if(kbhit())
        {
            char c = getch();

            switch(c)
            {
                case 'x':
                case 'X':
                kill = true;
                break;
            }
        }
    }

    setMotorPower(LEFT_DRIVE, 0);
    setMotorPower(RIGHT_DRIVE, 0);
    setMotorPower(DRILL_DRIVE, 0);

    return 0;
}
