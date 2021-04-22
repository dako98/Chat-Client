#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define debug

#include <string>
#include <memory>

#include "User.hpp"

using usrType = std::string;

class Message
{

public:
    usrType getSender() const;
    usrType getReceiver() const;
    std::string getContents() const;

    Message()
        : contents(""), receiver(""), sender("")
    {
    }

    Message(const std::string &text,
            const User &sender,
            const User &receiver)
        : receiver(receiver.getName()), sender(sender.getName()), contents(text)
    {
    }
    Message(const std::string &text,
            const std::string &sender,
            const std::string &receiver)
        : receiver(receiver), sender(sender), contents(text)
    {
    }
    Message(const Message& other)
    {
        this->contents = other.contents;
        this->receiver = other.receiver;
        this->sender = other.sender;
    }

    friend std::ostream &operator<<(std::ostream &out, const Message &obj);

    ~Message();

private:

    std::string contents;
    usrType receiver;
    usrType sender;
};

#endif // __MESSAGE_H__
