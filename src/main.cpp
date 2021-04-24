#include <iostream>
#include <boost/asio.hpp>
#include <string>

#include "Message.hpp"
#include "MessageBuilder.hpp"

using boost::asio::ip::tcp;

const std::string ADDRESS("127.0.0.1");
const std::string PORT("42123");

std::string userName;
std::string password;
std::string receiver;
std::string message;

int sendMessage(tcp::socket &socket, const Message &message);
int receiveMessage(tcp::socket &socket, Message &message);
boost::system::error_code &messageReceiverV2(tcp::socket &socket, Message &message);

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

void requestHistory(tcp::socket &socket, const std::string &clientName, const std::string &participant)
{
    const std::string name = clientName, message = participant, receiver = "history";

    MessageBuilder messageBuilder;

    messageBuilder.setReceiver(receiver);
    messageBuilder.setSender(name);
    messageBuilder.setMessage(message);

    Message request = messageBuilder.build();

    sendMessage(socket, request);

    receiveMessage(socket, request);

    if (request.getSender() == "server")
    {
        if (request.getContents() == " ")
        {
            std::cout << "(X) Getting history error. Server responded with empty response.";
        }
        else
        {
            int messages = atoi(request.getContents().c_str());

            for (size_t i = 0; i < messages; i++)
            {
                receiveMessage(socket, request);
                std::cout << request << '\n';
            }
        }
    }
    else
    {
        std::cout << "Did not get response from server. Got \"" << request << "\" instead.";
        throw std::invalid_argument("Invalid response.");
    }
}

Message writeMessage(const std::string &clientName, const std::string &to)
{
    std::string text;
    std::cout << "Message (max 255): ";

    do
    {
        std::cin >> text;
    } while (text.length() > MAX_SIZE);

    MessageBuilder builder;
    builder.setMessage(message);
    builder.setReceiver(to);
    builder.setSender(clientName);

    return builder.build();
}

bool requestToChat(tcp::socket &socket, const std::string &clientName, const std::string &recipient)
{
    std::string name = clientName, message = recipient, receiver = "chat";
    bool valid = true;

    MessageBuilder messageBuilder;

    valid &= messageBuilder.setSender(name);
    valid &= messageBuilder.setMessage(message);

    messageBuilder.setReceiver(receiver);

    Message request = messageBuilder.build();

    if (valid)
    {
        sendMessage(socket, request);

        receiveMessage(socket, request);
    }

    if (request.getSender() == "server")
    {
        if (request.getContents() != " ")
        {
            std::cout << "(X) Login error. Server responded with: " << request.getContents() << '\n';
            valid = false;
        }
        else
        {
            valid = true;
        }
    }
    else
    {
        std::cout << "Did not get response from server. Got \"" << request << "\" instead.\n";
        throw std::invalid_argument("Invalid response.");
    }
    return valid;
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
    }

    Message sessionMessage;
    while (allowedToChat)
    {
        // receive session message
        receiveMessage(socket, sessionMessage);

        // display message
        std::cout << sessionMessage << '\n';

        // write message
        sessionMessage = writeMessage(clientName, recipient);

        if (sessionMessage.getContents() == "end")
        {
            allowedToChat = false;
            break;
        }

        // send message
        sendMessage(socket, sessionMessage);
    } // send/receive loop
}

bool login(tcp::socket &socket, std::string &loggedinName)
{
    std::string name, password, receiver = "login";
    bool valid = true, successfullLogin = false;

    MessageBuilder messageBuilder;
    do
    {        valid = true;
        std::cout << "Ender name: ";
        std::cin >> name;
        valid &= messageBuilder.setSender(name);
        std::cout << "Ender password: ";
        std::cin >> password;
        valid &= messageBuilder.setMessage(password);

    } while (!valid);

    messageBuilder.setReceiver(receiver);

    Message login = messageBuilder.build();

    sendMessage(socket, login);

    receiveMessage(socket, login);

    if (login.getSender() == "server")
    {
        if (login.getContents() != " ")
        {
            std::cout << "(X) Login error. Server responded with: " << login.getContents() << '\n';
            successfullLogin = false;
        }
        else
        {
            loggedinName = name;
            successfullLogin = true;
        }
    }
    else
    {
        std::cout << "Did not get response from server. Got \"" << login << "\" instead.";
        throw std::invalid_argument("Invalid response.");
    }
    return successfullLogin;
}

bool registerUser(tcp::socket &socket, std::string &registeredName)
{
    std::string name, password, receiver = "register";
    bool valid = true, successfullRegistration;

    MessageBuilder messageBuilder;
    do
    {
        std::cout << "Ender name (max 255): ";
        std::cin >> name;
        valid &= messageBuilder.setSender(name);
        std::cout << "Ender password (max 255): ";
        std::cin >> password;
        valid &= messageBuilder.setMessage(password);

    } while (!valid);

    messageBuilder.setReceiver(receiver);

    Message registration = messageBuilder.build();

    sendMessage(socket, registration);

    receiveMessage(socket, registration);

    if (registration.getSender() == "server")
    {
        if (registration.getContents() != " ")
        {
            std::cout << "(X) Registration error. Server responded with: " << registration.getContents() << '\n';
            successfullRegistration = false;
        }
        else
        {
            userName = registeredName;
            successfullRegistration = true;
        }
    }
    else
    {
        std::cout << "Did not get response from server. Got \"" << registration << "\" instead.";
        throw std::invalid_argument("Invalid response.");
    }
    return successfullRegistration;
}

void menu(tcp::socket &&socket)
{
    // While invalid
    std::string command;
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
                std::cout << "Registered. Type: login <name> <password>.";
            }
        }

        break;
        case Commands::LOGIN:
        {
            bool loginStatus = login(socket, userName);
            if (loginStatus)
            {
                std::cout << "Logged in as " << userName << ". Starting chat...\n";
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
    }

    while (commandCode != Commands::END);

    std::this_thread::sleep_for(std::chrono::seconds(100));
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

int receiveMessage(tcp::socket &socket, Message &message)
{
    messageReceiverV2(socket, message);
}

boost::system::error_code &messageReceiverV2(tcp::socket &socket, Message &message)
{
    boost::system::error_code error;

    MessageBuilder receivedMessage;
    error = receivedMessage.setAll(socket);
    if (error)
        throw std::exception();

    message = receivedMessage.build();
    return error;
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
