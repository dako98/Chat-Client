#ifndef __MESSAGEBUILDER_H__
#define __MESSAGEBUILDER_H__

#include <boost/asio.hpp>

#include "Message.hpp"

// We limit the scope of this so it doesn't leak into other classes
#define Socket boost::asio::ip::tcp::socket

class MessageBuilder
{
public:
    boost::system::error_code &setAll(Socket &socket);
    boost::system::error_code &setSender(Socket &socket);
    boost::system::error_code &setReceiver(Socket &socket);
    boost::system::error_code &setMessage(Socket &socket);
    Message &&build() const;

private:
    std::string sender;
    std::string receiver;
    std::string message;
    std::string rBuffer;
};


#undef Socket

#endif // __MESSAGEBUILDER_H__
