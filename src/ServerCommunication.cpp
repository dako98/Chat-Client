#include "ServerCommunication.hpp"
#include "MessageBuilder.hpp"

int sendMessage(tcp::socket &socket, const Message &message)
{
    int code = 0;
    std::string bufferStr = "";
    std::string receiver = (char)message.getReceiver().length() + message.getReceiver();
    std::string sender = (char)message.getSender().length() + message.getSender();
    std::string text = (char)message.getContents().length() + message.getContents();
    bufferStr += receiver;
    bufferStr += sender;
    bufferStr += text;

    auto buffer = boost::asio::buffer(bufferStr);

    boost::asio::write(socket, buffer); // shorthand for loop doing socket.write_some()

    return code;
}

int receiveMessage(tcp::socket &socket, Message &message)
{
    messageReceiverV2(socket, message);
}


boost::system::error_code messageReceiverV2(tcp::socket &socket, Message &message)
{
    boost::system::error_code error;

    MessageBuilder receivedMessage;
    error = receivedMessage.setAll(socket);
    if (error)
        throw std::exception();

    message = receivedMessage.build();
    return error;
}

void startAsyncReceiver(tcp::socket &socket, ThreadSafeQueue<Message> &messageQueue)
{
    Message receivedMessage;

    for (;;)
    {
        receiveMessage(socket, receivedMessage);
        messageQueue.waitAndPush(receivedMessage);

        std::cout << receivedMessage << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
