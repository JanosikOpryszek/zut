#include "Loger.hpp"
#include<iostream>



eErrorCodes drv::Loger::errRet;
    
drv::Loger::Loger()
{
    std::cout<<"Logger created"<<std::endl;
}

drv::Loger::~Loger()
{

}


eErrorCodes drv::Loger::mLog_ERR(std::string a_strInput)
{
    std::cout<<a_strInput<<std::endl;
    errRet=OK;
    return errRet;
}


eErrorCodes drv::Loger::mLog_DBG(std::string a_strInput)
{
    std::cout<<a_strInput<<std::endl;
    errRet=OK;
    return errRet;
}




