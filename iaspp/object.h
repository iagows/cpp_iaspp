#ifndef OBJECT_H
#define OBJECT_H

#include "iaspp_global.h"
#include "iaspp.h"

#include <locale.h>

class IASPP_PUBLIC Object
{
public:
    Object();
    virtual ~Object();

    virtual std::string toLocaleString();

    virtual std::string toString();

    std::ostream& operator<<(std::ostream& os);
};

#endif // OBJECT_H
