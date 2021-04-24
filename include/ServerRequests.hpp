#ifndef __SERVERREQUESTS_H__
#define __SERVERREQUESTS_H__

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

void requestHistory(tcp::socket &socket, const std::string &clientName, const std::string &participant);

bool requestToChat(tcp::socket &socket, const std::string &clientName, const std::string &recipient);

bool login(tcp::socket &socket, std::string &loggedinName);

bool registerUser(tcp::socket &socket, std::string &registeredName);

#endif // __SERVERREQUESTS_H__
