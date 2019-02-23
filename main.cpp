#include<iostream>
#include <gtk/gtk.h>
#include<string>
#include<pthread.h>
#include<unistd.h>  // sleep
#include <stdlib.h> // exit()             ??
#include <string.h> // memset strlen
#include <arpa/inet.h> // inet_pton inet_addr
#include <sys/socket.h> // AF_UNIX
#include <netinet/in.h> // sockaddr_in    ??
#include"Candriverserver.hpp"
#include"ICandriverserver.hpp"
#include"imsgvermethod.hpp"
#include"eErrorCodes.h"
#include"ILogger.h"
#include"Loger.hpp"

using namespace drv;

void sendmsg(drv::ICandriverserver* );  //function for emulating ECU (sending)
void change();                          //changing values using direction
static char msgTmp[3];
static int temp=23;                      //values
static int presure=0;
static int rpm=400;
static int speed=0;
static int airtemp=23;
static bool direction=1;                //1=increase values 0=decrease values during change
static std::string tekst;





int main ()
{
    //creating objects
    ImsgmethodPut* msgmetput=new ImsgmethodPut();   //obj for display recieved messages
    ILogger* loger=new Loger();                     //obj for logging errors etc
    ICandriverserver* mycandrv=new Candriverserver(*loger,*msgmetput);  //obj of can drv

    //init can drv
    mycandrv->init();


    bool version;
    std::cout<<"Sending (car emulation)       press 0"<<std::endl;
    std::cout<<"Receiving (tester emulation)  press 1: ";
    std::cin >> version;


    //if true - reading msg and display on commandline
    if(version)
    {
        mycandrv->mRun();
    }
    //if 0 - sending msg (car emulation)
    else
    {
        sendmsg(mycandrv);
    }

    //wait for key to exit
    char c;
    std::cout<<"press x + enter to exit! "<<std::endl;
    std::cin>>c;

    //exiting
    mycandrv->mStop();
    delete(mycandrv);
    delete(loger);
    delete(msgmetput);

    return 0;
}


void sendmsg(drv::ICandriverserver* mycandrv)
{

    while(1)
    {
        std::system("clear");
        std::cout<<"Engine started,sending: "<<std::endl;
        std::cout<<"----------------------- "<<std::endl;
        std::cout<<"Coolant temp: "<<temp <<std::endl;
        std::cout<<"Fuel pressure: "<<presure<<std::endl;
        std::cout<<"Engine RMP: "<<rpm<<std::endl;
        std::cout<<"Speed: "<<speed<<std::endl;
        std::cout<<"Air temp: "<<airtemp<<std::endl;
        std::cout<<"----------------------- "<<std::endl;
        std::cout<<"press Ctrl + C to exit! "<<std::endl;
        //send coolant temp
        tekst="105#";
        sprintf(msgTmp,"%d",(temp+40));    //sprintf - converts int to decimal base char array
        tekst+=msgTmp;
        mycandrv->send(tekst);
        tekst.clear();
        usleep(100000);
        //send fuler presure
        tekst="10a#";
        sprintf(msgTmp,"%d",(presure/3));
        tekst+=msgTmp;
        mycandrv->send(tekst);
        tekst.clear();
        usleep(100000);
        //send rpm
        tekst="10c#";
        sprintf(msgTmp,"%d",(4*(rpm/256)));
        tekst+=msgTmp;
        mycandrv->send(tekst);
        tekst.clear();
        usleep(100000);
        //send speed
        tekst="10d#";
        sprintf(msgTmp,"%d",speed);
        tekst+=msgTmp;
        mycandrv->send(tekst);
        tekst.clear();
        usleep(100000);
        //send airtemp
        tekst="10f#";
        sprintf(msgTmp,"%d",(airtemp+40));
        tekst+=msgTmp;
        mycandrv->send(tekst);
        tekst.clear();
        usleep(100000);
        //change values
        change();
    }

}

void change()
{
    if (direction )
    {
        if( temp<110 )
        {
            temp+=1;
            presure+=1;
            rpm+=50;
            speed+=2;

        }
        else
            direction=0;
    }
    else
    {
        if(temp>23)
        {
            temp-=1;
            presure-=1;
            rpm-=50;
            speed-=2;

        }
        else
            direction=1;
    }

}
