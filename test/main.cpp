#define USER
//#define SENDER
//#define CLIENT

#ifdef SENDER

#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

const short multicast_port = 30001;
const int max_message_count = 10;

class sender
{
public:
  sender(boost::asio::io_service& io_service,
      const boost::asio::ip::address& multicast_address)
    : endpoint_(multicast_address, multicast_port),
      socket_(io_service, endpoint_.protocol()),
      timer_(io_service),
      message_count_(0)
  {
    std::ostringstream os;
    os << "Message " << message_count_++;
    message_ = os.str();

    socket_.async_send_to(
        boost::asio::buffer(message_), endpoint_,
        boost::bind(&sender::handle_send_to, this,
          boost::asio::placeholders::error));
  }

  void handle_send_to(const boost::system::error_code& error)
  {
    if (!error && message_count_ < max_message_count)
    {
      timer_.expires_from_now(boost::posix_time::seconds(1));
      timer_.async_wait(
          boost::bind(&sender::handle_timeout, this,
            boost::asio::placeholders::error));
    }
  }

  void handle_timeout(const boost::system::error_code& error)
  {
    if (!error)
    {
      std::ostringstream os;
      os << "Message " << message_count_++;
      message_ = os.str();

      std::cout << message_ << " sended" << std::endl;

      socket_.async_send_to(
          boost::asio::buffer(message_), endpoint_,
          boost::bind(&sender::handle_send_to, this,
            boost::asio::placeholders::error));
    }
  }

private:
  boost::asio::ip::udp::endpoint endpoint_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::deadline_timer timer_;
  int message_count_;
  std::string message_;
};

#endif

#ifdef CLIENT
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"

const short multicast_port = 30001;

class receiver
{
public:
  receiver(boost::asio::io_service& io_service,
      const boost::asio::ip::address& listen_address,
      const boost::asio::ip::address& multicast_address)
    : socket_(io_service)
  {
    // Create the socket so that multiple may be bound to the same address.
    boost::asio::ip::udp::endpoint listen_endpoint(
        listen_address, multicast_port);
    socket_.open(listen_endpoint.protocol());
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket_.bind(listen_endpoint);

    // Join the multicast group.
    socket_.set_option(
        boost::asio::ip::multicast::join_group(multicast_address));

    std::cout << "Start receive" << std::endl;

    socket_.async_receive_from(
        boost::asio::buffer(data_, max_length), sender_endpoint_,
        boost::bind(&receiver::handle_receive_from, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive_from(const boost::system::error_code& error,
      size_t bytes_recvd)
  {
    if (!error)
    {
      std::cout.write(data_, bytes_recvd);
      std::cout << std::endl;

      socket_.async_receive_from(
          boost::asio::buffer(data_, max_length), sender_endpoint_,
          boost::bind(&receiver::handle_receive_from, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
  }

private:
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint sender_endpoint_;
  enum { max_length = 1024 };
  char data_[max_length];
};


#endif

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
#ifdef USER
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
#endif

#ifdef SENDER
    try
    {
      boost::asio::io_service io_service;
      sender s(io_service, boost::asio::ip::address::from_string("239.255.0.1"));
      io_service.run();
    }
    catch (std::exception& e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }

    int n;
    std::cin >> n;

    return 0;
#endif

#ifdef CLIENT
    try
    {
      boost::asio::io_service io_service;
      receiver r(io_service,
          boost::asio::ip::address::from_string("0.0.0.0"),
          boost::asio::ip::address::from_string("239.255.0.1"));
      io_service.run();
    }
    catch (std::exception& e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }
    return 0;
#endif

}
