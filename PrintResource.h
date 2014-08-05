#ifndef PRINTRESOURCE_H
#define PRINTRESOURCE_H

#include <Wt/WResource>

using namespace Wt;

class PrintResource : public Wt::WResource
{
public:
    PrintResource(WContainerWidget *content,Wt::WObject *parent = 0);
    virtual ~PrintResource();

    void handleRequest(const Wt::Http::Request& request,
                       Wt::Http::Response& response);
protected:
    WContainerWidget *m_content;
private:
};

#endif // PRINTRESOURCE_H
