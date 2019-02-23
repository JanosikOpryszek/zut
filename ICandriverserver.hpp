#ifndef ICANDRIVERSERVER_HPP
#define ICANDRIVERSERVER_HPP



#include <string>
#include "ILogger.h"
#include "imsgvermethod.hpp"
namespace drv
{

class ICandriverserver
{
    public:
    // <destructor>
    virtual ~ICandriverserver();
    virtual eErrorCodes init() = 0;
    virtual eErrorCodes mRun() = 0;
    virtual eErrorCodes mStop() = 0;
    virtual eErrorCodes send(std::string)=0;
};


}

#endif
