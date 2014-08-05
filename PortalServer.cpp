
#include "PortalServer.h"

#include <Wt/WTimer>

//WTimer *PortalServer::m_timer = new WTimer();

PortalServer::PortalServer(const std::string &wtApplicationPath, const std::string &wtConfigurationFile):WServer(wtApplicationPath,wtConfigurationFile)
{
    //m_timer->setInterval(60000);
    //m_timer.start();
}

PortalServer::~PortalServer()
{
    //delete m_timer;
}

void PortalServer::sendOrders()
{
    //log("info")<<"******** SEND ORDERS *********"<<__LINE__;
}


void PortalServer::startTimer()
{
    //PortalServer::m_timer = new WTimer();
    //PortalServer::m_timer->setInterval(60000);
    //PortalServer::m_timer->timeout()->connect(boost::bind(&PortalServer::sendOrders, this));
    //std::cout<<" inter "<<m_timer.interval()<<std::endl;
    //PortalServer::m_timer->start();
    //std::cout<<"2"<<std::endl;

}

