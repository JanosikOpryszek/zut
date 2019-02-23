#ifndef CANDRIVERSERVER_HPP
#define CANDRIVERSERVER_HPP

#include <net/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include<pthread.h>
#include<stdint.h>
#include <sys/socket.h>
#include"ICandriverserver.hpp"
#include"imsgvermethod.hpp"
#include"ILogger.h"

namespace drv
{

class Candriverserver:public drv::ICandriverserver
{
public:
    Candriverserver(drv::ILogger &a_oLogger,drv::ImsgmethodPut &a_oMSGver);
    // destructor
    ~Candriverserver();
    // thread variable
    pthread_t m_Thread_id;


    eErrorCodes init();

    eErrorCodes mStop();

    eErrorCodes mRun();

    void *Work(void);

    static void *RunWork(void *);

    eErrorCodes send(std::string);

private:
    /// @brief  <Candriverserver copy constructor>
    Candriverserver(const Candriverserver &arg);
    /// @brief  <Candriverserver assignment constructor>
    Candriverserver& operator=(const Candriverserver &arg);
    /// @brief     buffer size variable
    static const uint16_t m_u16BuffSize=1024;

    /// @brief     error code variable
    eErrorCodes m_eRetEr;
    /// @brief     variables for sockets
    int32_t m_i32ServerSockfd;
    int32_t m_i32ServerSockfd2;
    static char m_cBufferSS[ ];      //send
    static char m_cBufferRR[ ];      //recieve
    struct sockaddr_can addr;
    struct can_frame m_soCanFrameSS;
    struct can_frame m_soCanFrameRR;
    struct ifreq m_soiFreq;
    static char  m_cBufferTmp[];
    static char m_soCanName[];

    /// @brief     for main loop, if false- stop
    bool m_bIsWorking;
    /// @brief     variable- if mRun was runned
    bool m_bWasRunned;
    /// @brief     variable- if Init was runned
    bool m_bWasInit;

    /// @brief     references to antoher objects
    drv::ILogger &m_LoggerRef;
    drv::ImsgmethodPut &m_MsgverRef;

};

}

#endif



