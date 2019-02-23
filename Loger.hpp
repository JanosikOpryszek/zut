
#ifndef LOGER_HPP
#define LOGER_HPP

#include"ILogger.h"

namespace drv
{

class Loger:public drv::ILogger
{
public:
    Loger();
    ~Loger();
    eErrorCodes mLog_ERR(std::string a_strInput);
    eErrorCodes mLog_DBG(std::string a_strInput);
private:
    static eErrorCodes errRet;

};

}

#endif

