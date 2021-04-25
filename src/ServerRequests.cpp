#include "ServerRequests.hpp"
#include "Message.hpp"
#include "MessageBuilder.hpp"
#include "ServerCommunication.hpp"

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
        if (request.getContents() == " ")
        {
            valid = true;
        }
        else
        {
            std::cout << "(X) Chat error. Server responded with: " << request.getContents() << '\n';
            valid = false;
        }
    }
    else
    {
        std::cout << "Did not get response from server. Got \"" << request << "\" instead.\n";
        throw std::invalid_argument("Invalid response.");
        valid = false;
    }
    return valid;
}

bool login(tcp::socket &socket, std::string &loggedinName)
{
    std::string name, password, receiver = "login";
    bool valid = true, successfullLogin = false;

    MessageBuilder messageBuilder;
    do
    {
        valid = true;
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
            registeredName = name;
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
