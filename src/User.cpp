#include <iostream>

#include "User.hpp"

/*User::User()
{
    std::cout << "user ctor";
    throw std::exception();
}*/

bool online = false;

User::User(const std::string &name,
           const std::string &password)
    : name(name), password(password)
{
#ifdef debug

    std::cout << "User Constructor called, NAME:\"" << name
              << "\" PASSWORD:\"" << password << "\"\n";
#endif
}

User::User(const User &other)
    : name(other.name), password(other.password)
{
}

User &User::operator=(const User &other)
{
    this->name = (other.name);
    this->password = (other.password);
    return *this;
}

bool User::operator<(const User &other) const
{
    return this->name < other.name;
}

bool User::operator==(const User &other) const
{
    return this->name == other.name;
}

bool User::comparePassword(const std::string &string) const
{
    return password == string;
}

User::~User()
{
#ifdef debug

    std::cout << "User Destructor called, NAME:\"" << name
              << "\" PASSWORD:\"" << password << "\"\n";
#endif
}

void User::serialise() const
{
    throw std::exception();
}

std::string User::getName() const
{
    return name;
}

std::ostream &operator<<(std::ostream &out, const User &obj)
{
    return out << obj.name << ' ' << obj.password;
}
