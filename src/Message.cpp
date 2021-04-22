#include <iostream>
#include "Message.hpp"
/*
Message::Message(const std::string &text,
                 const User &sender,
                 const User &receiver)
    : contents(text), sender(sender), receiver(receiver)
{
#ifdef debug

    std::cout << "Message Constructor called, SENDER:\"" << sender
              << "\" RECEIVER:\"" << receiver
              << "\" CONTENTS:\"" << contents << "\"\n";
#endif
}*/

Message::~Message()
{
#ifdef debug

#include <iostream>

    std::cout << "Message Destructor called, SENDER:\"" << sender
              << "\" RECEIVER:\"" << receiver
              //<< "\" CONTENTS:\"" << *Pcontents << "\"\n";
              << "\" CONTENTS:\"" << contents << "\"\n";
#endif
}

usrType Message::getSender() const
{
    return sender;
}
usrType Message::getReceiver() const
{
    return receiver;
}
std::string Message::getContents() const
{
    return contents;
}
std::ostream &operator<<(std::ostream &out, const Message &obj)
{
    return out
           //<< "Message { Sender: " << *(obj.Psender.get())
           //<< " receiver: " << *(obj.Preceiver.get())
           // << " contents: " << *(obj.Pcontents.get()) << " }\n";
           << "Message { Sender: " << (obj.sender)
           << " receiver: " << (obj.receiver)
           << " contents: " << (obj.contents) << " }\n";
}
