#ifndef _SERVER_COMM_THREAD_H_
#define _SERVER_COMM_THREAD_H_

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include <opencv2/highgui/highgui.hpp>

#include <queue>
#include <cstring>

class ServerCommThread
{

    public:
        ServerCommThread(boost::asio::io_service& io_service, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);

        void sendImage(cv::Mat& image);
        void close();

    private:
        void handle_connect(const boost::system::error_code& error);
        void handle_read(const boost::system::error_code& error);
        void handle_write(const boost::system::error_code& error);

        void do_write(cv::Mat& image);
        void do_close();

        char readBuffer[1024];

        boost::asio::io_service& io_service_;
        boost::asio::ip::tcp::socket socket;

        std::queue<std::string> postbox;
};

#endif
