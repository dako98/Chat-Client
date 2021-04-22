#include <iostream>
#include <boost/asio.hpp>
#include <string>

#include "Message.hpp"

using boost::asio::ip::tcp;

const std::string ADDRESS("127.0.0.1");
const std::string PORT("42123");
int sendMessage(tcp::socket &socket, const Message &message);

void chat(tcp::socket &&socket)
{
    Message initialMessage = Message("pass1","client1", "client2");
    sendMessage(socket, initialMessage);
    
    Message subsequent = Message("hello","client1", "client2");
    sendMessage(socket, subsequent);

    subsequent = Message("loopback","client1", "client1");
    sendMessage(socket, subsequent);
}

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

int main()
{

    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);

        tcp::resolver::query q(ADDRESS, PORT);
        tcp::resolver::iterator endpointsIterator = resolver.resolve(q);
        tcp::resolver::iterator endpointsEnd;

        tcp::socket socket(io_service);

//                chat(std::move(socket));


        for (; endpointsIterator != endpointsEnd; endpointsIterator++)
        {
            std::cout << "found " << endpointsIterator->endpoint().address().to_string()
                      << ":" << endpointsIterator->endpoint().port() << std::endl;
            boost::system::error_code e;
            socket.connect(*endpointsIterator, e);
            if (!e)
            {
                std::cout << "... and connected!" << std::endl;
                chat(std::move(socket));
                break;
            }
            else
            {
                std::cout << "... failed to connect due to: " << e.message() << std::endl;
                socket.close();
            }
        }

        std::cout << "closing" << std::endl;
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
