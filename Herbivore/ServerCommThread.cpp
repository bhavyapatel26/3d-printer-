#include "ServerCommThread.h"

ServerCommThread::ServerCommThread(boost::asio::io_service& io_service, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
: io_service_(io_service), socket(io_service)
{
    boost::asio::async_connect(socket, endpoint_iterator, boost::bind(&ServerCommThread::handle_connect, this, boost::asio::placeholders::error));
}

void ServerCommThread::handle_connect(const boost::system::error_code& error)
{
    if(!error)
    {
        boost::asio::async_read(socket, boost::asio::buffer(readBuffer, 1024), boost::bind(&ServerCommThread::handle_read, this, boost::asio::placeholders::error));
    }
}

void ServerCommThread::handle_read(const boost::system::error_code& error)
{
    if(!error)
    {
        boost::asio::async_read(socket, boost::asio::buffer(readBuffer, 1024), boost::bind(&ServerCommThread::handle_read, this, boost::asio::placeholders::error));
    }
    else
    {
        do_close();
    }
}

void ServerCommThread::sendImage(cv::Mat& image)
{
    io_service_.post(boost::bind(&ServerCommThread::do_write, this, image));
}

void ServerCommThread::do_write(cv::Mat& image)
{
    boost::asio::write(socket, boost::asio::buffer(&image, sizeof(image)), boost::asio::transfer_all());
}

void ServerCommThread::do_close()
{
    socket.close();
}
