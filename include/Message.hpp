#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#include <string>
#include <memory>

#include "User.hpp"

class Message
{


public:
    std::string getSender() const;
    std::string getReceiver() const;
    std::string getContents() const;

    Message()
        : contents(""), receiver(""), sender("")
    {
    }

    /*   Message(const std::string &text,
            const User &sender,
            const User &receiver)
        : receiver(receiver.getName()), sender(sender.getName()), contents(text)
    {
    }*/

    Message(const std::string &text,
            const std::string &sender,
            const std::string &receiver)
        : receiver(receiver), sender(sender), contents(text)
    {
    }
    Message(const Message &other)
    {
        this->contents = other.contents;
        this->receiver = other.receiver;
        this->sender = other.sender;
    }

    friend std::ostream &operator<<(std::ostream &out, const Message &obj);

    ~Message();

    static const unsigned int MAX_FIELD_SIZE = 255;

private:
    std::string contents;
    std::string receiver;
    std::string sender;
};

#endif // __MESSAGE_H__
