#include<stdlib.h>  //atoi
#include <stdio.h>  //sprintf
#include<pthread.h>
#include<string>
#include<iostream>
#include<unistd.h>  // sleep
#include<string.h> // memset strlen strcpy
#include<sys/socket.h> // AF_UNIX
#include"Candriverserver.hpp"


char drv::Candriverserver::m_cBufferSS[ m_u16BuffSize ];      //send
char drv::Candriverserver::m_cBufferRR[ m_u16BuffSize ];      //recieve
char  drv::Candriverserver::m_cBufferTmp[m_u16BuffSize];
char drv::Candriverserver::m_soCanName[] = "can0";


drv::Candriverserver::Candriverserver(drv::ILogger &a_oLogger,drv::ImsgmethodPut &a_oMSGver ):
    m_bIsWorking(true),
    m_bWasRunned(false),
    m_bWasInit(false),
    m_LoggerRef(a_oLogger),
    m_MsgverRef(a_oMSGver)
{

}

drv::Candriverserver::~Candriverserver()
{

}

eErrorCodes drv::Candriverserver::init()
{
    m_eRetEr=OK;
    /// @brief checking if was init before
    if(!m_bWasInit)
    {

        /// @brief 1 SOCKET CREATE for reciewe
        m_i32ServerSockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (m_i32ServerSockfd <0 )
        {
            m_LoggerRef.mLog_ERR(std::string("CANdriver ERR - serverSocked creation error  - ERR"));
            m_eRetEr = DRIVER_ERROR;;
        }
        /// @brief 2 ACTIVATE STRUCTURE

        strcpy(m_soiFreq.ifr_name, m_soCanName);
        ioctl(m_i32ServerSockfd, SIOCGIFINDEX, &m_soiFreq);
        addr.can_family  = AF_CAN;
        addr.can_ifindex = m_soiFreq.ifr_ifindex;

        /// @brief 3 BINDING socket
        if ((bind( m_i32ServerSockfd,reinterpret_cast<struct sockaddr*>(&addr),sizeof(addr))) <0)
        {
            m_LoggerRef.mLog_ERR(std::string("CANdriver ERR - socked binding error  - ERR"));
            m_eRetEr = DRIVER_ERROR;;
        }

        /// @brief 1 SOCKET CREATE for sending
        m_i32ServerSockfd2 = socket(PF_CAN, SOCK_RAW, CAN_RAW);
        if (m_i32ServerSockfd2<0)
        {
             drv::Candriverserver::m_LoggerRef.mLog_ERR(std::string("CANdriver ERR - socked client create error  - ERR"));
             m_eRetEr=DRIVER_ERROR;
        }
    }
    else
    {
        m_LoggerRef.mLog_ERR(std::string("CANdriver ERR- init() runed AGAIN!!!  "));
        m_eRetEr = DRIVER_ERROR;
    }
    m_bWasInit=true;

    return m_eRetEr;
}

eErrorCodes drv::Candriverserver::mStop()
{
    m_eRetEr=OK;
    m_bIsWorking=false;
    m_LoggerRef.mLog_DBG(std::string("CANdriver DBG - got mStop, main loop stooped, ready to close- OK"));
    return m_eRetEr;
}


eErrorCodes drv::Candriverserver::mRun()
{
    if(m_bWasRunned)
        {
            m_LoggerRef.mLog_ERR(std::string("CANdriver ERR - mRun runned second time - ERR"));
            m_eRetEr=DRIVER_ERROR;
        }
        else
        {
            m_eRetEr=OK;
            m_bWasRunned=true;
            m_LoggerRef.mLog_DBG(std::string("CANdriver DBG - got mRun, main loop started - OK"));
            if((pthread_create(&m_Thread_id,0,&RunWork,this))<0)
            {
                m_LoggerRef.mLog_ERR(std::string("CANdriver ERR - initializess pthreat create error - ERR"));
                m_eRetEr=DRIVER_ERROR;
            }
        }
    return m_eRetEr;
}


void *drv::Candriverserver::Work()
{
    /// @brief M A I N  L O O P - reading canMsg

    while(m_bIsWorking)
    {

        memset( m_cBufferRR, 0, sizeof( m_cBufferRR ) );
        /// @brief recive checking if error
        if(recv( m_i32ServerSockfd, &m_soCanFrameRR, sizeof( struct can_frame ), 0)<0)
        {
            m_LoggerRef.mLog_ERR(std::string("CANdriver ERR - socked reciving error  - ERR"));
        }
        /// @brief     change 3character canID to 4character MsgID
        char msgTemp[3];
        sprintf(msgTemp,"%x",m_soCanFrameRR.can_id);    //sprintf - converts int to hexadecimal base char array

        m_cBufferRR[0]=msgTemp[0];
        m_cBufferRR[1]=msgTemp[1];
        m_cBufferRR[2]=msgTemp[2];
        m_cBufferRR[3]='#';
        /// @brief     check size of canMsg to iterate
        int size = m_soCanFrameRR.can_dlc;
        int i=0;
        /// @brief iterate to write readed DATA to Buffer
        for (;i<size;i++)
        {
            m_cBufferRR[i+4]=static_cast<char>(m_soCanFrameRR.data[i]);
        }
        m_cBufferRR[i+4]='\0';
        /// @brief CALL MSGVERYFICATOR INTERFACE to pass MSG
        m_MsgverRef.putMessage(std::string(m_cBufferRR));  //

    }
    return 0;
}



void *drv::Candriverserver::RunWork(void *context)
{
    return ( reinterpret_cast<drv::Candriverserver *>(context))->drv::Candriverserver::Work();
}



eErrorCodes drv::Candriverserver::send(std::string a_strTab)
{
    m_eRetEr=OK;
    /// @brief check if data in msg is not longer than 8
    uint8_t size=static_cast<uint8_t>(a_strTab.length());
    if (size>12)
    {
        m_LoggerRef.mLog_ERR(std::string("CANdriver ERR - MSG length more 12, so data longer 8, not sended  - ERR"));
        m_eRetEr=DRIVER_ERROR;
    }
    else
    {
        /// @brief COPY string msg to Can Struct (ID + data)
        strcpy (m_cBufferSS,a_strTab.c_str()  );    //c_str() copy string to char array Buffer

        unsigned long intindexx=std::stoul(a_strTab.substr(0,3),nullptr,16);   //stoul conver hex string to int, substring 0-2

        m_soCanFrameSS.can_id  = intindexx;

        /// @brief MsgID has 4 char, so -4=real size of pure DATA
        size-=4;
        /// @brief   check size of canMsg to iterate
        m_soCanFrameSS.can_dlc = size;
        /// @brief iterate Buffer to write DATA to CanStructure to send
        for (int i=0;i<size;i++)
        {
            m_soCanFrameSS.data[i] = static_cast<uint8_t>(m_cBufferSS[i+4]);
        }
        /// @brief SENDING
        if((sendto( m_i32ServerSockfd2, &m_soCanFrameSS, sizeof (struct can_frame ), 0,reinterpret_cast< struct sockaddr * >(& addr), sizeof(addr)))<0)
        {
            m_LoggerRef.mLog_ERR(std::string("CANdriver ERR - socked client SENDTO error  - ERR"));
            m_eRetEr=DRIVER_ERROR;
        }
        memset( m_cBufferSS, 0, sizeof( m_cBufferSS ) );
    }
    return m_eRetEr;
}




