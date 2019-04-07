#include "object.h"

Object::Object() { }

Object::~Object() { }

std::string Object::toLocaleString()
{
    return this->toString();
}

std::string Object::toString()
{
    std::string out = "[Object]";
    return out;
}

std::ostream &Object::operator<<(std::ostream &os)
{
    return os << this->toLocaleString();
}
