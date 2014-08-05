#ifndef PORTALSERVER_H
#define PORTALSERVER_H

#include <Wt/WServer>

using namespace Wt;

class PortalServer : public WServer
{
    public:
        PortalServer(const std::string &wtApplicationPath=std::string(), const std::string &wtConfigurationFile=std::string());
        virtual ~PortalServer();
        void startTimer();
        static WTimer *m_timer;// = new WTimer();
    protected:

    private:
        void sendOrders();
};

#endif // PORTALSERVER_H
