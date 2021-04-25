#include <iostream>
#include "Message.hpp"

Message::~Message()
{
#ifdef debug

#include <iostream>

    std::cout << "Message Destructor called, SENDER:\"" << sender
              << "\" RECEIVER:\"" << receiver
              << "\" CONTENTS:\"" << contents << "\"\n";
#endif
}

std::string Message::getSender() const
{
    return sender;
}
std::string Message::getReceiver() const
{
    return receiver;
}
std::string Message::getContents() const
{
    return contents;
}
std::ostream &operator<<(std::ostream &out, const Message &obj)
{
#ifdef debug
    out << "Message { Sender: " << (obj.sender)
        << " receiver: " << (obj.receiver)
        << " contents: " << (obj.contents) << " }";
#endif
#ifndef debug
    out << "[ " << obj.sender << " -> " << obj.receiver << " ]: " << obj.contents;
#endif

    return out;
}
