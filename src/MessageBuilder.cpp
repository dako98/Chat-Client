#include "MessageBuilder.hpp"

#define Socket boost::asio::ip::tcp::socket

boost::system::error_code &MessageBuilder::setSender(Socket &socket)
{
    boost::system::error_code error;
    int readLength = 0;

    rBuffer.resize(sizeof(int));

    while (readLength < sizeof(int))
        readLength += socket.read_some(boost::asio::buffer(rBuffer), error);
    readLength = 0;

    int senderLen = atoi(rBuffer.c_str());

    rBuffer.resize(senderLen);

    while (readLength < senderLen)
        readLength = socket.read_some(boost::asio::buffer(rBuffer), error);
    sender = rBuffer;

    return error;
}
boost::system::error_code &MessageBuilder::setReceiver(Socket &socket)
{
    boost::system::error_code error;
    int readLength = 0;

    rBuffer.resize(sizeof(int));

    while (readLength < sizeof(int))
        readLength += socket.read_some(boost::asio::buffer(rBuffer), error);
    readLength = 0;

    int receiverLen = atoi(rBuffer.c_str());

    rBuffer.resize(receiverLen);

    while (readLength < receiverLen)
        readLength = socket.read_some(boost::asio::buffer(rBuffer), error);
    receiver = rBuffer;

    return error;
}
boost::system::error_code &MessageBuilder::setMessage(Socket &socket)
{
    boost::system::error_code error;
    int readLength = 0;

    rBuffer.resize(sizeof(int));

    while (readLength < sizeof(int))
        readLength += socket.read_some(boost::asio::buffer(rBuffer), error);
    readLength = 0;

    int messageLen = atoi(rBuffer.c_str());

    rBuffer.resize(messageLen);

    while (readLength < messageLen)
        readLength = socket.read_some(boost::asio::buffer(rBuffer), error);
    message = rBuffer;

    return error;
}
Message &&MessageBuilder::build() const
{
    return Message(message, sender, receiver);
}

boost::system::error_code &MessageBuilder::setAll(Socket &socket)
{
    boost::system::error_code error;

    if (
        !(error = setReceiver(socket)) &&
        !(error = setSender(socket)) &&
        !(error = setMessage(socket)))
        return error;
}

#undef Socket
