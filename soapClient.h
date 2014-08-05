#ifndef _SOAPCLIENT_H_
#define _SOAPCLIENT_H_

#include <vector>

#include <Wt/WObject>
#include <Wt/WString>

#define INLINE_WQUERY(...)  L ## #__VA_ARGS__

class InfoBaseSoapBindingProxy;

using namespace Wt;

class SoapClient: public WObject
{
    public:
        SoapClient(const std::string &endPoint);
        virtual ~SoapClient();
        std::vector<WString> getUserDept(const std::string userCode) const;
        std::vector<std::vector<WString> >getUnbackedDocs(std::string userCode) const;
        std::vector<std::vector<WString> >getLastOrder(std::string userCode) const;
        std::vector<std::vector<WString> >getLastShipment(std::string userCode) const;
        std::vector<std::vector<WString> >getDeptStucture(std::string userCode) const;
        std::vector<std::vector<WString> >getOrdersShipping(const WDate &beginDate,const WDate &endDate,const std::string userCode) const;
        std::vector<std::vector<WString> >getGoodsSales(const WDate &beginDate,const WDate &endDate,const std::string userCode) const;
        std::vector<std::vector<WString> >getMutualItog(const WDate &beginDate,const WDate &endDate,const std::string userCode) const;
        std::vector<std::vector<WString> >getMutual(const WDate &beginDate,const WDate &endDate,const std::string userCode) const;
        std::vector<std::vector<WString> >getDocTablel(const std::string docRef) const;
        WStandardItemModel * vectorToModel(std::vector<std::vector<WString> > params,WStandardItemModel *model);
        typedef std::map<std::string, std::string> ConnectionMap;
        static ConnectionMap m_soapConnectionMap;
    protected:
        InfoBaseSoapBindingProxy *m_client;
        std::string soapUserName;
        std::string soapUserPasswd;
    private:
};





#endif // _SOAPCLIENT_H_
