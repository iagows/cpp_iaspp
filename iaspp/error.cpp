#include "error.h"

Error::Error(std::string message, int id)
{
    if(message.compare("") != 0)
        this->msg = message;
    this->id = id;
    this->err = "Error";
}

Error::~Error()
{

}

int Error::errorID()
{
    return this->id;
}

std::string Error::getMessage()
{
    return this->msg;
}

void Error::setMessage(std::string msg)
{
    this->msg = msg;
}

std::string Error::getName()
{
    return this->err;
}

void Error::setName(std::string err)
{
    this->err = err;
}

std::string Error::getStackTrace()
{
    //error_stacktrace / stacktrace.h .cpp
}

std::string Error::toString()
{
    std::string out = "Error";
    if(this->msg.compare("") != 0)
        out += ": " + this->msg;
    return out;
}
