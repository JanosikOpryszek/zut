#ifndef ILOGGER_H
#define ILOGGER_H

#include <string>
#include "eErrorCodes.h"


namespace drv
{

class ILogger
{
public:
    virtual ~ILogger(){}
    virtual eErrorCodes mLog_ERR(std::string a_strInput) = 0;
    virtual eErrorCodes mLog_DBG(std::string a_strInput) = 0;

};

}
#endif
