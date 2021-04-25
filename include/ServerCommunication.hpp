#include <boost/asio.hpp>

#include "ThreadsafeQueue.h"
#include "Message.hpp"
#include "MessageBuilder.hpp"

using boost::asio::ip::tcp;


int sendMessage(tcp::socket &socket, const Message &message);
int receiveMessage(tcp::socket &socket, Message &message);
boost::system::error_code messageReceiverV2(tcp::socket &socket, Message &message);
void startAsyncReceiver(tcp::socket &socket, ThreadSafeQueue<Message> &messageQueue, bool &keepAlive);
