#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/read.hpp>
#include <string>

//#define debug

#include "Message.hpp"
#include "MessageBuilder.hpp"
#include "ThreadsafeQueue.h"
#include "ServerRequests.hpp"
#include "ServerCommunication.hpp"

using boost::asio::ip::tcp;

const std::string ADDRESS("127.0.0.1");
const std::string PORT("42123");

enum Commands
{
    INVALID = -1,

    REGISTER,
    LOGIN,
    CHAT,
    END,

    COMMANDS_SIZE
};

static inline int commandToCode(const std::string &command)
{
    static std::array<std::string, Commands::COMMANDS_SIZE> commandsResolver =
        {"register",
         "login",
         "chat",
         "end"};

    int code = Commands::INVALID;

    for (int commandCode = 0; commandCode < Commands::COMMANDS_SIZE; ++commandCode)
    {
        if (command == commandsResolver[commandCode])
        {
            code = commandCode;
            break;
        }
    }
    return code;
}

Message writeMessage(const std::string &clientName, const std::string &to)
{
    std::string text;
    std::cout << "Message (max 255): ";

    do
    {
        // FIXME: use getline to get whole line, not just first word.
        std::cin >> text;
    } while (text.length() < 1 ||
             text.length() > MAX_SIZE);

    MessageBuilder builder;
    builder.setMessage(text);
    builder.setReceiver(to);
    builder.setSender(clientName);

    return builder.build();
}

// read from the async message queue and transfer to vector
std::vector<Message> receiveAsyncMessagesFromQueue(ThreadSafeQueue<Message> &messageQueue)
{
    int size = messageQueue.size();
    std::vector<Message> receivedMessages;
    receivedMessages.reserve(size);
    for (size_t i = 0; i < size; i++)
    {
        receivedMessages.push_back(messageQueue.waitAndPop());
    }
    return receivedMessages;
}

void printMessages(const std::vector<Message> &messages)
{
    for (auto &&i : messages)
    {
        std::cout << i << '\n';
    }
}

void receiveOnline(tcp::socket &socket)
{
    Message response;
    receiveMessage(socket, response);

    std::string onlineUsers;
    int count = atoi(response.getContents().c_str());
    for (size_t i = 0; i < count; i++)
    {
        receiveMessage(socket, response);
        onlineUsers += response.getContents() + " ";
    }
    std::cout << onlineUsers << std::endl;
}

void chat(tcp::socket &socket, const std::string &clientName)
{
    std::string recipient;

    std::cout << "Chat with: ";
    std::cin >> recipient;
    bool allowedToChat = requestToChat(socket, clientName, recipient);

    if (allowedToChat)
    {
        // receive chat history
        requestHistory(socket, clientName, recipient);

        std::cout << "Use \"end\" to quit chat." << std::endl;

        Message sessionMessage;
        ThreadSafeQueue<Message> messageQueue;

        auto readThread = std::thread(startAsyncReceiver, std::ref(socket), std::ref(messageQueue));

        while (allowedToChat)
        {
            // Print messages from async queue.
            //            printMessages(
            //               receiveAsyncMessagesFromQueue(messageQueue));

            // write message
            sessionMessage = writeMessage(clientName, recipient);

            if (sessionMessage.getContents() != "end")
            {
                // send message
                sendMessage(socket, sessionMessage);
            }
            else
            {
                allowedToChat = false;
                break;
            }

        } // send/receive loop

        readThread.join();
    }
    else
    {
        std::cout << "Chat denied by server.\n";
    }
}

void menu(tcp::socket &&socket)
{
    // While invalid
    std::string command;
    std::string userName;
    int commandCode;
    do
    {
        std::cout << "register (user, pass), login (user, pass)\n";
        std::cin >> command;
        commandCode = commandToCode(command);

        switch (commandCode)
        {
        case Commands::REGISTER:
        {
            bool registrationStatus = registerUser(socket, userName);
            if (registrationStatus)
            {
                std::cout << "Registered. Type: login <name> <password>.\n";
            }
        }

        break;
        case Commands::LOGIN:
        {
            bool loginStatus = login(socket, userName);
            if (loginStatus)
            {
                std::cout << "Logged in as " << userName << ". Online users: ";
                receiveOnline(socket);
                //TODO: receive message with online users and print it
                chat(socket, userName);
            }
        }

        break;
        case Commands::CHAT:
        {
            // TODO: Remove
            break;
        }
        case Commands::END:
        {
            std::cout << "Invalid command. You are not in a chat.\n";

            break;
        }
        case Commands::INVALID:
        {
            std::cout << "Invalid command.\n";
            break;
        }
        default:
        {
        }
        break;
        }
    } while (commandCode != Commands::END);

#ifdef debug
    std::cout << "Exiting in 100s." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(100));
#endif
}

int main()
{
    //    std::cout << "Username, password, receiver, message\n";
    //    std::cin >> userName >> password >> receiver >> message;

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
                menu(std::move(socket));
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
