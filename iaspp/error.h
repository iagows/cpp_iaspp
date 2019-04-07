#ifndef ERROR_H
#define ERROR_H

#include "object.h"
#include "iaspp.h"

#include <exception>

#include <stdlib.h>

class IASPP_PUBLIC Error : public Object, public std::exception
{
public:
    Error(std::string message =  "", int id=0);
    virtual ~Error() throw() override;

    int errorID();

    std::string getMessage();
    void setMessage(std::string msg);

    std::string getName();
    void setName(std::string err);

    std::string getStackTrace();
    virtual std::string toString() override;

    virtual const char* what() const throw() { return this->err.c_str(); }

private:
    int id;
    std::string msg;
    std::string err;
};
#endif // ERROR_H
